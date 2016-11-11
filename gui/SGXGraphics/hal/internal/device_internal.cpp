
#include "device_internal.h"
#include "context_internal.h"
#include "types_internal.h"
#include "effect.h"
#include "../vertex_format.h"
#include "../log.h"
#include "../debug.h"
#include <string>
#include <fstream>
#include <queue>
#include <functional>
#include <unordered_set>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace SynGlyphX
{
	namespace hal_gl
	{
		namespace
		{
			context_internal* default_context = nullptr;
			int maximum_cbuffer_size = -1;
			std::unordered_set<hal::effect*> effects;
			std::unordered_set<hal::font*> fonts;
			std::vector<std::string> forced_includes;
			GLuint default_render_target;
			uint64_t frame = 0ull;

			hal::effect* text_effect = nullptr;
			FT_Library freetype;
			hal::vertex_format text_format;

			std::string gl_vendor, gl_renderer, gl_version;

			bool initialized = false;

			const char* text_vert =
				"#version 330 core\n"
				"layout( std140 ) uniform shared_data\n"
				"{\n"
				"mat4 transform;\n"
				"vec4 color;\n"
				"};\n"
				"layout( std140 ) uniform instance_data\n"
				"{\n"
				"vec4 string[1024];\n"	// x, y, array slice, unused
				"};\n"
				"layout( location = 0 ) in vec3 position;\n"
				"layout( location = 1 ) in vec2 texcoord;\n"
				"out vec3 _uv;\n"
				"out vec4 _color;\n"
				"void main() {\n"
				"vec4 ch = string[gl_InstanceID];"
				"_uv = vec3( texcoord.xy, ch.z ); _color = color;\n"
				"gl_Position = transform * vec4( position + vec3( ch.xy, 0 ), 1 );\n"
				"}\n";
			const char* text_frag =
				"#version 330 core\n"
				"layout( location = 0 ) out vec4 outputF;\n"
				"in vec3 _uv;\n"
				"in vec4 _color;\n"
				"uniform sampler2DArray base_texture_array;\n"
				"void main() {\n"
				"float coverage = texture( base_texture_array, _uv ).r;\n"
				"outputF = vec4( _color.rgb, _color.a * coverage );\n"
				"}\n";
		}

		hal::device_info device_internal::get_device_info()
		{
			gl_vendor = initialized ? std::string( reinterpret_cast<const char*>( glGetString( GL_VENDOR ) ) ) : "uninitialized";
			gl_renderer = initialized ? std::string( reinterpret_cast<const char*>( glGetString( GL_RENDERER ) ) ) : "uninitialized";
			gl_version = initialized ? std::string( reinterpret_cast<const char*>( glGetString( GL_VERSION ) ) ) : "uninitialized";

			GLint major, minor;
			glGetIntegerv( GL_MAJOR_VERSION, &major );
			glGetIntegerv( GL_MINOR_VERSION, &minor );

			return hal::device_info{ initialized, gl_vendor.c_str(), gl_renderer.c_str(), gl_version.c_str(), major, minor };
		}

		bool device_internal::init()
		{
			glewExperimental = GL_TRUE;
			auto glew_result = glewInit();
			if ( glew_result != GLEW_OK )
			{
				hal::log::error( "Failed to initialize GLEW.", reinterpret_cast<const char*>( glewGetErrorString( glew_result ) ) );
				return false;
			}
			if ( !glewIsSupported( "GL_VERSION_3_3" ) )
			{
				hal::log::error( "GLEW reports GL 3.3 is not supported by this context." );
				return false;
			}
			assert( !default_context );
			while ( glGetError() );	// workaround in case GLEW threw up any GL errors in its init (it raises 'invalid enumerant' occasionally)
			glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &maximum_cbuffer_size );
			default_context = new context_internal();
			auto error = FT_Init_FreeType( &freetype );
			hal::debug::_assert( !error, "error initializing freetype" );
			text_effect = create_effect( text_vert, nullptr, text_frag );
			set_cbuffer_external( text_effect, "instance_data" );
			text_format.add_stream( hal::stream_info( hal::stream_type::float32, 3, hal::stream_semantic::position, 0 ) );
			text_format.add_stream( hal::stream_info( hal::stream_type::float32, 2, hal::stream_semantic::texcoord, 0 ) );
			default_render_target = 0u;
			initialized = true;
			return true;
		}

		void device_internal::shutdown()
		{
			assert( default_context );
			FT_Done_FreeType( freetype );
			delete default_context;
			default_context = nullptr;
			initialized = false;
		}

		void device_internal::end_frame()
		{
			++frame;

			// If we haven't used a string in a few frames, we probably won't use it again in the near future, so release it.
			const auto max_age = 4ull;
			for ( const auto& f : fonts )
			{
				auto it = f->string_cache.begin();
				while ( it != f->string_cache.end() )
				{
					if ( frame - it->second.last_use > max_age )
					{
						release( it->second.instance_data );
						it = f->string_cache.erase( it );
					}
					else
					{
						++it;
					}
				}
			}
		}

		hal::context* device_internal::get_default_context()
		{
			return default_context;
		}
		
		hal::context* device_internal::get_new_context()
		{
			return new context_internal;
		}

		void device_internal::release_context( hal::context* c )
		{
			delete c;
		}

		void device_internal::set_external_default_render_target( unsigned int rt )
		{
			default_render_target = rt;
		}

		GLuint device_internal::get_default_render_target()
		{
			return default_render_target;
		}

		uint64_t device_internal::current_frame()
		{
			return frame;
		}

		hal::render_target_set* device_internal::create_render_target_set( unsigned int w, unsigned int h )
		{
			auto* set = new hal::render_target_set;
			set->w = w; set->h = h;
			set->depth_target = nullptr;
			glGenFramebuffers( 1, &set->fb );
			hal::check_errors();
			return set;
		}

		unsigned int device_internal::add_color_target( hal::render_target_set* set, hal::texture_format fmt )
		{
			assert( set );
			assert( !is_depth_format( fmt ) );
			glBindFramebuffer( GL_FRAMEBUFFER, set->fb );
			auto idx = set->color_targets.size();
			assert( idx < 16u );	// theoretical maximum; todo: check ACTUAL maximum on current driver
			hal::texture* t = create_texture( set->w, set->h, fmt, nullptr );
			set->color_targets.push_back( t );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx, GL_TEXTURE_2D, t->handle, 0 );
			assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			hal::check_errors();
			return idx;
		}

		void device_internal::add_depth_target( hal::render_target_set* set, hal::texture_format fmt )
		{
			assert( set );
			assert( is_depth_format( fmt ) );
			assert( !set->depth_target );
			glBindFramebuffer( GL_FRAMEBUFFER, set->fb );
			set->depth_target = create_texture( set->w, set->h, fmt, nullptr );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, set->depth_target->handle, 0 );
			assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			hal::check_errors();
		}

		hal::texture* device_internal::get_target_texture( hal::render_target_set* set, unsigned int index )
		{
			assert( set );
			assert( index < set->color_targets.size() );
			return set->color_targets[index];
		}

		hal::texture* device_internal::get_target_depth_texture( hal::render_target_set* set )
		{
			assert( set );
			return set->depth_target;
		}

		hal::effect* device_internal::load_effect( const char* vs_file, const char* gs_file, const char* ps_file )
		{
			hal::effect* e = hal::effect::load( vs_file, gs_file, ps_file, forced_includes );
			effects.insert( e );
			return e;
		}

		hal::effect* device_internal::create_effect( const char* vs, const char* gs, const char* ps )
		{
			hal::effect* e = hal::effect::create( vs, gs, ps );
			effects.insert( e );
			return e;
		}

		hal::mesh* device_internal::create_mesh( const hal::vertex_format& fmt, const hal::primitive_type prim, unsigned int vertex_count, void* vertices, unsigned int primitive_count, void* indices, bool support_readback )
		{
			unsigned int vertex_data_size = fmt.vertex_size_in_bytes() * vertex_count;
			unsigned int index_data_size = primitive_count * indices_per_primitive( prim ) * sizeof( GLuint );

			hal::mesh* m = new hal::mesh;
			m->fmt = fmt;
			m->prim = prim;
			m->vertex_count = vertex_count;
			m->primitive_count = primitive_count;

			glGenVertexArrays( 1, &m->vao );
			glGenBuffers( 1, &m->vbo );
			glGenBuffers( 1, &m->ebo );

			glBindVertexArray( m->vao );
			glBindBuffer( GL_ARRAY_BUFFER, m->vbo );
			glBufferData( GL_ARRAY_BUFFER, vertex_data_size, vertices, GL_STATIC_DRAW );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m->ebo );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, index_data_size, indices, GL_STATIC_DRAW );	// only support 4-byte uint indices for now

			unsigned int offset = 0u;
			for ( unsigned int i = 0; i < fmt.stream_count(); ++i )
			{
				auto& stream = fmt.get_stream_info( i );
				assert( stream.data_type == hal::stream_type::float32 );	// only support 4-byte floats for now
				glEnableVertexAttribArray( i );
				glVertexAttribPointer( i, stream.count, GL_FLOAT, GL_FALSE, fmt.vertex_size_in_bytes(), reinterpret_cast<GLvoid*>( offset ) );

				offset += fmt.element_size_in_bytes( stream.data_type ) * stream.count;
			}

			glBindVertexArray( 0 );

			if ( support_readback )
			{
				m->vertex_data_copy = new char[vertex_data_size];
				memcpy( m->vertex_data_copy, vertices, vertex_data_size );

				m->index_data_copy = new char[index_data_size];
				memcpy( m->index_data_copy, indices, index_data_size );
			}
			else
			{
				m->vertex_data_copy = nullptr;
				m->index_data_copy = nullptr;
			}

			hal::check_errors();

			return m;
		}

		hal::cbuffer* device_internal::create_cbuffer( unsigned int size )
		{
			hal::cbuffer* c = new hal::cbuffer;
			c->size = size;
			glGenBuffers( 1, &c->buffer );
			glBindBuffer( GL_UNIFORM_BUFFER, c->buffer );
			char* zeroes = new char[size];
			memset( zeroes, 0, size );
			glBufferData( GL_UNIFORM_BUFFER, size, zeroes, GL_DYNAMIC_DRAW );
			delete[] zeroes;
			glBindBuffer( GL_UNIFORM_BUFFER, 0 );
			return c;
		}

		namespace
		{
			void get_gl_fmt( hal::texture_format fmt, GLenum& gl_fmt, GLenum& gl_internal_fmt, GLenum& gl_type )
			{
				gl_fmt = GL_RGB, gl_internal_fmt = GL_RGB, gl_type = GL_UNSIGNED_BYTE;
				switch ( fmt )
				{
					case hal::texture_format::r8:
						gl_fmt = GL_RED; gl_internal_fmt = GL_R8; gl_type = GL_UNSIGNED_BYTE; break;
					case hal::texture_format::rgb8:
						gl_fmt = GL_RGB; gl_internal_fmt = GL_RGB8; gl_type = GL_UNSIGNED_BYTE; break;
					case hal::texture_format::rgba8:
						gl_fmt = GL_RGBA; gl_internal_fmt = GL_RGBA8; gl_type = GL_UNSIGNED_BYTE; break;
					case hal::texture_format::d24:
						gl_fmt = GL_DEPTH_COMPONENT; gl_internal_fmt = GL_DEPTH_COMPONENT24; gl_type = GL_FLOAT; break;
					default:
						hal::debug::_assert( false, "unknown texture format" );
				}
			}
            
            unsigned int get_fmt_size( hal::texture_format fmt )
            {
                switch ( fmt )
                {
                    case hal::texture_format::r8: return 1u; break;
                    case hal::texture_format::rgb8: return 3u; break;
                    case hal::texture_format::rgba8: return 4u; break;
                    case hal::texture_format::d24: return 3u; break;
                    default: hal::debug::_assert( false, "unknown texture format" ); return 4u;
                }
            }
		}

		hal::texture* device_internal::create_texture( unsigned int w, unsigned int h, hal::texture_format fmt, uint8_t* data )
		{
			hal::texture* tex = new hal::texture;
			tex->w = w;
			tex->h = h;
			tex->fmt = fmt;
			tex->has_mipchain = false;
			glGenTextures( 1, &tex->handle );
			glBindTexture( GL_TEXTURE_2D, tex->handle );
			GLenum gl_fmt = GL_RGB, gl_internal_fmt = GL_RGB, gl_type = GL_UNSIGNED_BYTE;
			get_gl_fmt( fmt, gl_fmt, gl_internal_fmt, gl_type );
			glTexImage2D( GL_TEXTURE_2D, 0, gl_fmt, w, h, 0, gl_fmt, gl_type, data );
			glBindTexture( GL_TEXTURE_2D, 0 );
			hal::check_errors();
			return tex;
		}

		hal::texture* device_internal::load_texture( const char* file, bool generate_mips )
		{
			assert( file );

			hal::texture* tex = nullptr;
			int w, h, depth;
			unsigned char* data = stbi_load( file, &w, &h, &depth, 0 );

			if ( data )
			{
				assert( depth == 3 || depth == 4 );
				tex = create_texture( w, h, depth == 3 ? hal::texture_format::rgb8 : hal::texture_format::rgba8, data );
				stbi_image_free( data );
				if ( generate_mips )
				{
					glBindTexture( GL_TEXTURE_2D, tex->handle );
					glGenerateMipmap( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, 0 );
					tex->has_mipchain = true;
				}
			}

			return tex;
		}

		hal::texture_array* device_internal::create_texture_array( unsigned int w, unsigned int h, unsigned int layers, hal::texture_format fmt )
		{
			hal::texture_array* tex = new hal::texture_array;
			tex->w = w;
			tex->h = h;
			tex->d = layers;
			tex->fmt = fmt;

			GLenum gl_fmt = GL_RGB, gl_internal_fmt = GL_RGB, gl_type = GL_UNSIGNED_BYTE;
			get_gl_fmt( fmt, gl_fmt, gl_internal_fmt, gl_type );

			glGenTextures( 1, &tex->handle );
			glBindTexture( GL_TEXTURE_2D_ARRAY, tex->handle );
			glTexStorage3D( GL_TEXTURE_2D_ARRAY, 1, gl_internal_fmt, w, h, layers );

			hal::check_errors();
			return tex;
		}

		void device_internal::update_array_slice( hal::texture_array* t, unsigned int layer, const hal::pixel_rect& rect, uint8_t* data )
		{
			assert( t );
			assert( layer < t->d );
			assert( data );

			GLenum gl_fmt = GL_RGB, gl_internal_fmt = GL_RGB, gl_type = GL_UNSIGNED_BYTE;
			get_gl_fmt( t->fmt, gl_fmt, gl_internal_fmt, gl_type );

			glBindTexture( GL_TEXTURE_2D_ARRAY, t->handle );
			glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, rect.x, rect.y, layer, rect.w, rect.h, 1, gl_fmt, gl_type, data );
			glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );
			hal::check_errors();
		}
        
        void device_internal::clear_array_slice( hal::texture_array* t, unsigned int layer )
        {
            assert( t );
            assert( layer < t->d );
            
            const unsigned int stack_limit = 512;
            unsigned int size = t->w * t->h * get_fmt_size( t->fmt );
            bool heap_allocated = false;
            unsigned char* mem = nullptr;
            
            if ( size > stack_limit )
            {
                heap_allocated = true;
                mem = new unsigned char[size];
            }
            else
            {
                mem = static_cast<unsigned char*>( alloca( size ) );
            }
            memset( mem, 0, size );
            
            update_array_slice( t, layer, { 0u, 0u, t->w, t->h }, mem );
            
            if ( heap_allocated )
                delete [] mem;
        }

		void device_internal::addref( hal::render_target_set* r )
		{
			r->addref();
		}

		void device_internal::addref( hal::mesh* m )
		{
			m->addref();
		}

		void device_internal::addref( hal::effect* e )
		{
			e->addref();
		}

		void device_internal::addref( hal::texture* t )
		{
			t->addref();
		}

		namespace
		{
			void release_refcounted( hal::ref_counted* rc, std::function<void( void )> delete_fn )
			{
				if ( rc->ref_count() > 0 ) rc->decref();
				if ( rc->ref_count() == 0 ) delete_fn();
			}
		}

		void device_internal::release( hal::effect* e )
		{
			release_refcounted( e, [e]() {
				auto e_it = effects.find( e );
				assert( e_it != effects.end() );	// asked to release effect that isn't in the list. double release?
				effects.erase( e_it );
				delete e;
			} );
		}

		void device_internal::release( hal::mesh* m )
		{
			release_refcounted( m, [m]() {
				glDeleteVertexArrays( 1, &m->vao );
				glDeleteBuffers( 1, &m->vbo );
				glDeleteBuffers( 1, &m->ebo );
				hal::check_errors();
				delete m->vertex_data_copy;
				delete m->index_data_copy;
				delete m;
			} );
		}

		void device_internal::release( hal::cbuffer* c )
		{
			release_refcounted( c, [c]() {
				glDeleteBuffers( 1, &c->buffer );
				delete c;
			} );
		}

		void device_internal::release( hal::texture* t )
		{
			release_refcounted( t, [t]() {
				glDeleteTextures( 1, &t->handle );
				delete t;
			} );
		}

		void device_internal::release( hal::render_target_set* r )
		{
			release_refcounted( r, [r]() {
				glDeleteFramebuffers( 1, &r->fb );
				if ( r->depth_target ) release( r->depth_target );
				for ( auto t : r->color_targets )
					release( t );
				delete r;
			} );
		}

		void device_internal::release( hal::texture_array* t )
		{
			release_refcounted( t, [t]() {
				glDeleteTextures( 1, &t->handle );
				delete t;
			} );
		}

		void device_internal::release( hal::font* f )
		{
			release_refcounted( f, [f]() {
				release( f->textures );
				release( f->glyph_mesh );
				for ( auto s : f->string_cache )
					release( s.second.instance_data );
				FT_Done_Face( f->face );
				auto f_it = fonts.find( f );
				assert( f_it != fonts.end() );	// asked to release font that isn't in the list. double release?
				fonts.erase( f_it );
				delete f;
			} );
		}

		unsigned int device_internal::maximum_constant_buffer_size()
		{
			return static_cast<unsigned int>( maximum_cbuffer_size );
		}

		void device_internal::set_break_on_error( bool val )
		{
			hal::set_break_on_error( val );
		}

		void device_internal::rebuild_effects()
		{
			for ( hal::effect* e : effects )
				e->recompile();
		}

		void device_internal::add_forced_effect_include( const char* filename )
		{
			forced_includes.push_back( filename );
		}

		void device_internal::set_cbuffer_external( hal::effect* e, const char* block_name )
		{
			e->set_cbuffer_external( block_name );
		}

		void device_internal::set_cbuffer_usage( hal::effect* e, const char* block_name, hal::cbuffer_usage usage )
		{
			e->set_cbuffer_usage( block_name, usage );
		}

		hal::font* device_internal::load_font( const char* file, unsigned int size )
		{
			hal::font* font = nullptr;
			FT_Face face = nullptr;

			auto error = FT_New_Face( freetype, file, 0, &face );
			if ( !error )
			{
				hal::debug::_assert( face->charmap != nullptr, "font %s does not contain a unicode charmap; this is unsupported", file );
				font = new hal::font;
				font->face = face;
				font->file = std::string( file );
				font->size = size;
			}
			else if ( error == FT_Err_Unknown_File_Format )
			{
				hal::debug::_assert( false, "unknown font file format in %s", file );
			}
			else
			{
				hal::debug::_assert( false, "error %i loading font %s", error, file );
			}

			error = FT_Set_Pixel_Sizes( face, 0, size );
			hal::debug::_assert( !error, "error %i setting pixel size %i for font %s", error, size, file );

			font->textures = create_texture_array( size, size, 256, hal::texture_format::r8 );
			font->next_slice = 0u;

			float glyph_vertices[]
			{
				0.f, 0.f, 0.f, 0.f, 0.f,
				float( size ), 0.f, 0.f, 1.f, 0.f,
				float( size ), float( size ), 0.f, 1.f, 1.f,
				0.f, float( size ), 0.f, 0.f, 1.f,
			};
			unsigned int glyph_indices[]
			{
				0u, 2u, 1u, 0u, 3u, 2u
			};
			font->glyph_mesh = create_mesh( text_format, hal::primitive_type::triangle_list, 4u, glyph_vertices, 2u, glyph_indices, false );

			fonts.insert( font );

			return font;
		}

		hal::effect* device_internal::get_text_effect()
		{
			return text_effect;
		}
	}
}
