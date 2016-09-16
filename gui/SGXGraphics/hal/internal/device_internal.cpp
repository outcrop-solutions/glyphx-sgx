
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
			std::vector<std::string> forced_includes;
			
			hal::effect* text_effect = nullptr;
			FT_Library freetype;
			hal::vertex_format text_format;

			const char* text_vert =
				"#version 330 core\n"
				"layout( std140 ) uniform instance_data\n"
				"{\n"
				"mat4 transform;\n"
				"vec4 color;\n"
				"};\n"
				"layout( location = 0 ) in vec3 position;\n"
				"layout( location = 1 ) in vec2 texcoord;\n"
				"out vec2 _uv;\n"
				"out vec4 _color;\n"
				"void main() {\n"
				"_uv = texcoord; _color = color;\n"
				"gl_Position = transform * vec4( position, 1 );\n"
				"}\n";
			const char* text_frag =
				"#version 330 core\n"
				"layout( location = 0 ) out vec4 outputF;\n"
				"in vec2 _uv;\n"
				"in vec4 _color;\n"
				"uniform sampler2D base_texture;\n"
				"void main() {\n"
				"float coverage = texture( base_texture, _uv ).r;\n"
				"outputF = vec4( _color.rgb, _color.a * coverage );\n"
				"}\n";
		}

		bool device_internal::init()
		{
			glewExperimental = GL_TRUE;
			auto glew_result = glewInit();
			hal::check_errors( true );
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
			hal::check_errors();
			glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &maximum_cbuffer_size );
			default_context = new context_internal();
			auto error = FT_Init_FreeType( &freetype );
			hal::debug::_assert( !error, "error initializing freetype" );
			// todo: inline this shader once it's stable (shouldn't have to exist in app folder since it's part of the graphics library).
			text_effect = create_effect( text_vert, nullptr, text_frag );
			text_format.add_stream( hal::stream_info( hal::stream_type::float32, 3, hal::stream_semantic::position, 0 ) );
			text_format.add_stream( hal::stream_info( hal::stream_type::float32, 2, hal::stream_semantic::texcoord, 0 ) );
			return true;
		}

		void device_internal::shutdown()
		{
			assert( default_context );
			FT_Done_FreeType( freetype );
			delete default_context;
			default_context = nullptr;
		}

		hal::context* device_internal::get_default_context()
		{
			return default_context;
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
				glVertexAttribPointer( i, stream.count, GL_FLOAT, GL_FALSE, fmt.vertex_size_in_bytes(), (GLvoid*)offset );

				offset += fmt.element_size_in_bytes( stream.data_type ) * stream.count;
			}

			glBindVertexArray( 0 );

			if ( support_readback )
			{
				m->vertex_data_copy = new char[vertex_data_size];
				memcpy_s( m->vertex_data_copy, vertex_data_size, vertices, vertex_data_size );

				m->index_data_copy = new char[index_data_size];
				memcpy_s( m->index_data_copy, index_data_size, indices, index_data_size );
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
			delete zeroes;
			glBindBuffer( GL_UNIFORM_BUFFER, 0 );
			return c;
		}

		hal::texture* device_internal::create_texture( unsigned int w, unsigned int h, hal::texture_format fmt, uint8_t* data )
		{
			hal::texture* tex = new hal::texture;
			tex->w = w;
			tex->h = h;
			tex->fmt = fmt;
			glPixelStorei( GL_PACK_ALIGNMENT, 1 );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
			glGenTextures( 1, &tex->handle );
			glBindTexture( GL_TEXTURE_2D, tex->handle );
			GLenum gl_fmt = GL_RGB, gl_internal_fmt = GL_RGB, gl_type = GL_UNSIGNED_BYTE;
			switch ( fmt )
			{
				case hal::texture_format::r8:
					gl_fmt = GL_RED; gl_internal_fmt = GL_R8; gl_type = GL_UNSIGNED_BYTE; break;
				case hal::texture_format::rgb8:
					gl_fmt = GL_RGB; gl_internal_fmt = GL_RGB8; gl_type = GL_UNSIGNED_BYTE; break;
				case hal::texture_format::rgba8:
					gl_fmt = GL_RGBA; gl_internal_fmt = GL_RGBA8; gl_type = GL_UNSIGNED_BYTE; break;
				default:
					hal::debug::_assert( false, "unknown texture format" );
			}
			glTexImage2D( GL_TEXTURE_2D, 0, gl_fmt, w, h, 0, gl_fmt, gl_type, data );
			glGenerateMipmap( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glPixelStorei( GL_PACK_ALIGNMENT, 4 );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
			hal::check_errors();
			return tex;
		}

		hal::texture* device_internal::load_texture( const char* file )
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
			}

			return tex;
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

		void device_internal::release( hal::font* f )
		{
			release_refcounted( f, [f]() {
				for ( auto g : f->glyphs )
					release( g.second.texture );
				FT_Done_Face( f->face );
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

			return font;
		}

		hal::effect* device_internal::get_text_effect()
		{
			return text_effect;
		}

		const hal::font_glyph& device_internal::get_glyph( hal::font* f, char c )
		{
			assert( f );

			// Load and render the glyph if we don't have it already.
			if ( f->glyphs.find( c ) == f->glyphs.end() )
			{
				auto idx = FT_Get_Char_Index( f->face, c );
				hal::debug::_assert( idx != 0, "FT_GetCharIndex failed for font %s and char code %i", f->file.c_str(), c );
				auto error = FT_Load_Glyph( f->face, idx, FT_LOAD_TARGET_LIGHT );
				auto glyph = f->face->glyph;
				hal::debug::_assert( error == 0, "FT_Load_Glyph failed for font %s and char code %i (char_index %i)", f->file.c_str(), c, idx );
				if ( glyph->format != FT_GLYPH_FORMAT_BITMAP )
				{
					error = FT_Render_Glyph( glyph, FT_RENDER_MODE_LIGHT );
					hal::debug::_assert( error == 0, "FT_Render_Glyph failed for font %s and char code %i (char_index %i )", f->file.c_str(), c, idx );
				}
				hal::font_glyph g;
				assert( glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY );
				g.texture = hal_gl::device_internal::create_texture( glyph->bitmap.width, glyph->bitmap.rows, hal::texture_format::r8, glyph->bitmap.buffer );
				g.origin_x = int16_t( glyph->bitmap_left );
				g.origin_y = int16_t( glyph->bitmap_top );
				g.advance_x = int16_t( glyph->advance.x >> 6 );
				g.advance_y = int16_t( glyph->advance.y >> 6 );

				FT_Glyph ftg = nullptr;
				FT_Get_Glyph( glyph, &ftg );
				FT_BBox bb;
				FT_Glyph_Get_CBox( ftg, FT_GLYPH_BBOX_PIXELS, &bb );
				FT_Done_Glyph( ftg );
				g.width = uint16_t( bb.xMax - bb.xMin );
				g.height = uint16_t( bb.yMax - bb.yMin );

				float glyph_vertices[]
				{
					0.f, 0.f, 0.f, 0.f, 0.f,
					float( glyph->bitmap.width ), 0.f, 0.f, 1.f, 0.f,
					float( glyph->bitmap.width ), float( glyph->bitmap.rows ), 0.f, 1.f, 1.f,
					0.f, float( glyph->bitmap.rows ), 0.f, 0.f, 1.f,
				};
				unsigned int glyph_indices[]
				{
					0u, 1u, 2u, 0u, 2u, 3u
				};
				g.mesh = create_mesh( text_format, hal::primitive_type::triangle_list, 4u, glyph_vertices, 2u, glyph_indices, false );

				f->glyphs.insert( std::make_pair( c, g ) );
			}

			hal::check_errors();
			return f->glyphs.find( c )->second;
		}

		const glm::vec2 device_internal::get_kerning( hal::font* f, char left, char right )
		{
			glm::vec2 result;
			if ( FT_HAS_KERNING( f->face ) )
			{
				auto l = FT_Get_Char_Index( f->face, left );
				auto r = FT_Get_Char_Index( f->face, right );
				FT_Vector kern;
				FT_Get_Kerning( f->face, l, r, FT_KERNING_DEFAULT, &kern );
				result = glm::vec2( kern.x >> 6, kern.y >> 6 );
			}
			return result;
		}
	}
}
