
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
			FT_Library freetype;
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
			return true;
		}

		void device_internal::shutdown()
		{
			assert( default_context );
			delete default_context;
			default_context = nullptr;
		}

		hal::context* device_internal::get_default_context()
		{
			return default_context;
		}

		hal::effect* device_internal::create_effect( const char* vs, const char* gs, const char* ps )
		{
			hal::effect* e = hal::effect::create( vs, gs, ps, forced_includes );
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

		hal::texture* device_internal::load_texture( const char* file )
		{
			assert( file );

			hal::texture* tex = nullptr;
			int w, h, depth;
			unsigned char* data = stbi_load( file, &w, &h, &depth, 0 );

			if ( data )
			{
				assert( depth == 3 || depth == 4 );
				tex = new hal::texture;
				tex->external = false;
				tex->w = w;
				tex->h = h;

				glGenTextures( 1, &tex->handle );
				//glActiveTexture( GL_TEXTURE0 );//todo: necessary?
				glBindTexture( GL_TEXTURE_2D, tex->handle );

				if ( depth == 3 )
					glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
				else if ( depth == 4 )
					glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

				glGenerateMipmap( GL_TEXTURE_2D );

				glBindTexture( GL_TEXTURE_2D, 0 );
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
				if ( !t->external )
					glDeleteTextures( 1, &t->handle );
				delete t;
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

		hal::font* device_internal::load_font( const char* file )
		{
			UNREFERENCED_PARAMETER( file );
			return nullptr;
		}
	}
}
