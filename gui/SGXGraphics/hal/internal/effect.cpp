
#include "effect.h"
#include <string>
#include <fstream>
#include "gl.h"
#include "../log.h"

namespace SynGlyphX
{
	namespace hal
	{
		namespace
		{
			std::string get_shader_info_log( GLuint shader )
			{
				int length = 0;
				int written = 0;
				char *log;
				std::string result;

				glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );

				if ( length > 0 )
				{
					log = (char *)malloc( length );
					glGetShaderInfoLog( shader, length, &written, log );
					result = log;
					free( log );
				}
				check_errors();

				return result;
			}

			std::string get_program_info_log( GLuint program )
			{
				int length = 0;
				int written = 0;
				char *log;
				std::string result;

				glGetProgramiv( program, GL_INFO_LOG_LENGTH, &length );

				if ( length > 0 )
				{
					log = (char *)malloc( length );
					glGetProgramInfoLog( program, length, &written, log );
					result = log;
					free( log );
				}
				check_errors();

				return result;
			}

			bool check_compile_status( GLuint shader )
			{
				GLint status;
				glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
				if ( status != GL_TRUE )
				{
					log::error( "Shader compile failed.", get_shader_info_log( shader ).c_str() );
				}
				check_errors();
				return status != 0;
			}

			bool check_link_status( GLuint program )
			{
				GLint status;
				glGetProgramiv( program, GL_LINK_STATUS, &status );
				if ( status != GL_TRUE )
				{
					log::error( "Shader link failed.", get_program_info_log( program ).c_str() );
				}
				check_errors();
				return status != 0;
			}

			std::string read_file( std::ifstream& in )
			{
				std::string contents;
				in.seekg( 0, std::ios::end );
				contents.reserve( static_cast<unsigned int>( in.tellg() ) );
				in.seekg( 0, std::ios::beg );
				std::copy( ( std::istreambuf_iterator<char>( in ) ), std::istreambuf_iterator<char>(), std::back_inserter( contents ) );
				in.close();
				return( contents );
			}

			GLuint load_shader( GLenum type, const char* file, std::vector<std::string>& forced_includes )
			{
				std::ifstream infile( file );
				if ( !infile )
				{
					log::error( "Shader source file not found.", file );
					return 0u;
				}

				GLuint id = glCreateShader( type );
				if ( id == 0 )
				{
					log::error( "glCreateShader() failed.", "" );
					return 0u;
				}

				std::string src;
				for ( const std::string& s : forced_includes )
				{
					std::ifstream include( s.c_str() );
					src += read_file( include );
				}

				src += read_file( infile );
				const char* csrc = src.c_str();

				glShaderSource( id, 1, &csrc, nullptr );
				glCompileShader( id );

				if ( !check_compile_status( id ) )
				{
					log::error( "Shader failing compile: ", file );
					glDeleteShader( id );
					id = 0;
				}

				check_errors();

				return id;
			}
		}

		effect::effect()
			: _program( 0 ), vs( 0 ), gs( 0 ), ps( 0 ), compile_success( false )
		{
		}

		void effect::compile( effect* e )
		{
			e->_program = glCreateProgram();

			bool fail = false;

			auto compile_and_attach = [e, &fail]( GLenum type, const std::string& src ) {
				if ( !src.empty() )
				{
					auto id = load_shader( type, src.c_str(), e->forced_includes );
					if ( id )
						glAttachShader( e->program(), id );
					else
						fail = true;
					return id;
				}
				else
				{
					return 0u;
				}
			};

			compile_and_attach( GL_VERTEX_SHADER, e->vs_src );
			compile_and_attach( GL_GEOMETRY_SHADER, e->gs_src );
			compile_and_attach( GL_FRAGMENT_SHADER, e->ps_src );

			if ( fail )
			{
				e->compile_success = false;
				return;
			}

			glLinkProgram( e->_program );

			auto program_log = get_program_info_log( e->_program );
			if ( !check_link_status( e->_program ) )
			{
				log::error( "glLinkProgram() failed.", program_log.c_str() );
				e->compile_success = false;
				return;
			}

			e->setup_uniform_blocks();

			check_errors();
			e->compile_success = true;
			e->setup_external_uniform_blocks();
			return;
		}

		effect* effect::create( const char* vs, const char* gs, const char* ps, const std::vector<std::string>& forced_header_includes )
		{
			effect* e = new effect;
			e->vs_src = vs ? vs : "";
			e->gs_src = gs ? gs : "";
			e->ps_src = ps ? ps : "";
			e->forced_includes = forced_header_includes;

			compile( e );
			if ( !e->compiled() ) e->release_internals();
			
			return e;
		}

		void effect::recompile()
		{
			compile( this );
		}

		void effect::setup_uniform_blocks()
		{
			GLint block_count = 0u;
			glGetProgramiv( _program, GL_ACTIVE_UNIFORM_BLOCKS, &block_count );

			for ( int i = 0; i < block_count; ++i )
			{
				const size_t bufsize = 256;
				GLchar name[bufsize];
				GLsizei length = 0;
				glGetActiveUniformBlockName( _program, i, bufsize, &length, name );

				uniform_block block;
				block.dirty = true;
				block.external = false;
				block.usage = cbuffer_usage::dynamic_draw;
				block.index = glGetUniformBlockIndex( _program, name );
				glGetActiveUniformBlockiv( _program, block.index, GL_UNIFORM_BLOCK_DATA_SIZE, &block.size );

				// For now, bind each block to the binding point equal to its index in the program.
				// (Eventually if we want to share uniform blocks between programs, we'll need to come up with some
				// scheme for assigning the binding points.)
				block.binding_pt = block.index;
				glUniformBlockBinding( _program, block.index, block.binding_pt );

				glGenBuffers( 1, &block.buffer );
				glBindBuffer( GL_UNIFORM_BUFFER, block.buffer );
				block.backing_buffer = new char[block.size];
				memset( block.backing_buffer, 0, sizeof( block.backing_buffer ) );
				glBufferData( GL_UNIFORM_BUFFER, block.size, block.backing_buffer, GL_DYNAMIC_DRAW );
				glBindBuffer( GL_UNIFORM_BUFFER, 0 );

				uniform_blocks.insert( std::make_pair( name, block ) );
			}
			check_errors();
		}

		void effect::set_uniform( const char* block_name, const char* uniform_name, const void* data, size_t size )
		{
			if ( compiled() )
			{
				auto b = uniform_blocks.find( block_name );
				if ( b != uniform_blocks.end() )
				{
					auto& block = b->second;
					if ( !block.external )
					{
						GLuint index;
						GLint offset;
						glGetUniformIndices( _program, 1, &uniform_name, &index );
						if ( index == GL_INVALID_INDEX )
						{
							log::error( "set_uniform: Uniform not found in block:", uniform_name );
							return;
						}
						glGetActiveUniformsiv( _program, 1, &index, GL_UNIFORM_OFFSET, &offset );
						memcpy_s( block.backing_buffer + offset, size, data, size );
						block.dirty = true;
						check_errors();
					}
					else
					{
						log::error( "set_uniform_block: Uniform block is marked as external:", block_name );
					}
				}
				else
				{
					log::error( "set_uniform: Uniform block not found:", block_name );
				}
			}
		}

		void effect::set_uniform_block( const char* block_name, const void* data, size_t size )
		{
			if ( compiled() )
			{
				auto b = uniform_blocks.find( block_name );
				if ( b != uniform_blocks.end() )
				{
					auto& block = b->second;
					if ( !block.external )
					{
						assert( size <= size_t( block.size ) );
						memcpy_s( block.backing_buffer, block.size, data, size );
						block.dirty = true;
					}
					else
					{
						log::error( "set_uniform_block: Uniform block is marked as external:", block_name );
					}
				}
				else
				{
					log::error( "set_uniform: Uniform block not found:", block_name );
				}
			}
		}

		void effect::bind()
		{
			if ( compiled() )
			{
				glUseProgram( _program );
				check_errors();
			}
		}

		void effect::update_uniforms()
		{
			if ( compiled() )
			{
				for ( auto& b : uniform_blocks )
				{
					auto& block = b.second;
					if ( !block.external )
					{
						if ( block.dirty )
						{
							glBindBuffer( GL_UNIFORM_BUFFER, block.buffer );
							glBufferData( GL_UNIFORM_BUFFER, block.size, block.backing_buffer, block.usage == hal::cbuffer_usage::dynamic_draw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
							block.dirty = false;
						}
						glBindBufferBase( GL_UNIFORM_BUFFER, block.binding_pt, block.buffer );
					}
				}
				check_errors();
			}
		}

		void effect::set_cbuffer_external( const char* block_name )
		{
			if ( external_uniform_block_names.find( block_name ) == external_uniform_block_names.end() )
				external_uniform_block_names.insert( block_name );
			setup_external_uniform_blocks();
		}

		void effect::set_cbuffer_usage( const char* block_name, cbuffer_usage usage )
		{
			if ( compiled() )
			{
				auto b = uniform_blocks.find( block_name );
				if ( b != uniform_blocks.end() )
					b->second.usage = usage;
			}
		}

		void effect::setup_external_uniform_blocks()
		{
			for ( const auto& name : external_uniform_block_names )
			{
				auto b = uniform_blocks.find( name );
				if ( b != uniform_blocks.end() )
				{
					b->second.external = true;
					delete[] b->second.backing_buffer;
					b->second.backing_buffer = nullptr;
				}
			}
		}

		unsigned int effect::get_uniform_block_index( const char* block_name )
		{
			if ( compiled() )
			{
				unsigned int index = UINT_MAX;
				auto b = uniform_blocks.find( block_name );
				if ( b != uniform_blocks.end() )
					index = b->second.binding_pt;
				return index;
			}
			else
			{
				return 0u;
			}
		}

		effect::~effect()
		{
			release_internals();
			check_errors();
		}

		void effect::release_internals()
		{
			for ( auto& block : uniform_blocks )
			{
				delete[] block.second.backing_buffer;
				glDeleteBuffers( 1, &block.second.buffer );
			}

			if ( compiled() )
			{
				glDeleteProgram( _program );
				glDeleteShader( vs );
				glDeleteShader( gs );
				glDeleteShader( ps );
			}
		}
	}
}
