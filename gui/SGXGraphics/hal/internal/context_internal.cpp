
#include "context_internal.h"
#include "device_internal.h"
#include "gl.h"
#include "types_internal.h"
#include "effect.h"
#include "../debug.h"

namespace SynGlyphX
{
	namespace hal_gl
	{
		context_internal::context_internal()
			: bound_effect( nullptr ), instancing_mesh( nullptr )
		{
			glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy );

			reset_defaults();
		}

		context_internal::~context_internal()
		{
			if ( bound_effect )
				device_internal::release( bound_effect );
		}

		void context_internal::reset_defaults_internal()
		{
			glBindVertexArray( 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		void context_internal::clear( hal::clear_type type, const glm::vec4& color )
		{
			if ( type & hal::clear_type::color ) glClearColor( color.r, color.g, color.b, color.a );

			GLbitfield ct = 0u;
			if ( type & hal::clear_type::color ) ct |= GL_COLOR_BUFFER_BIT;
			if ( type & hal::clear_type::depth ) ct |= GL_DEPTH_BUFFER_BIT;
			if ( type & hal::clear_type::stencil ) ct |= GL_STENCIL_BUFFER_BIT;
			glClear( ct );

			hal::check_errors();
		}

		void context_internal::set_rasterizer_state( const hal::rasterizer_state& state )
		{
			if ( state.multisample ) glEnable( GL_MULTISAMPLE );
			else glDisable( GL_MULTISAMPLE );

			if ( state.cull_backfaces ) glEnable( GL_CULL_FACE );
			else glDisable( GL_CULL_FACE );

			if ( state.wireframe ) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

			if ( state.depth_bias )
			{
				glEnable( GL_POLYGON_OFFSET_FILL );
				glPolygonOffset( 1.f, 1.f );
			}
			else
			{
				glDisable( GL_POLYGON_OFFSET_FILL );
				glPolygonOffset( 0.f, 0.f );
			}
		}

		void context_internal::set_depth_state( hal::depth_state state )
		{
			switch ( state )
			{
				case hal::depth_state::disabled:
					glDisable( GL_DEPTH_TEST );
					glDepthMask( GL_FALSE );
					break;
				case hal::depth_state::read_only:
					glEnable( GL_DEPTH_TEST );
					glDepthMask( GL_FALSE );
					break;
				case hal::depth_state::read_write:
					glEnable( GL_DEPTH_TEST );
					glDepthMask( GL_TRUE );
					break;
			}
		}

		void context_internal::set_blend_state( hal::blend_state state )
		{
			switch ( state )
			{
				case hal::blend_state::disabled:
					glDisable( GL_BLEND );
					break;
				case hal::blend_state::additive:
					glEnable( GL_BLEND );
					glBlendFunc( GL_SRC_ALPHA, GL_ONE );
					break;
				case hal::blend_state::alpha:
					glEnable( GL_BLEND );
					glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
					break;
				case hal::blend_state::premultiplied_alpha:
					glEnable( GL_BLEND );
					glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
					break;
			}
		}

		void context_internal::bind( unsigned int index, hal::texture* t, const hal::sampler_state& state )
		{
			glActiveTexture( GL_TEXTURE0 + index );
			glBindTexture( GL_TEXTURE_2D, t->handle );

			GLenum wrap = state.wrap == hal::texture_wrap::wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE;

			GLenum min_filter = state.filter == hal::texture_filter::point ? GL_POINT : GL_LINEAR_MIPMAP_LINEAR;
			GLenum mag_filter = state.filter == hal::texture_filter::point ? GL_POINT : GL_LINEAR;

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter );
			if ( state.filter == hal::texture_filter::aniso )
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy );

			hal::check_errors();
		}

		void context_internal::bind( unsigned int index, hal::texture_array* t, const hal::sampler_state& state )
		{
			glActiveTexture( GL_TEXTURE0 + index );
			glBindTexture( GL_TEXTURE_2D_ARRAY, t->handle );

			GLenum wrap = state.wrap == hal::texture_wrap::wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE;

			GLenum min_filter = state.filter == hal::texture_filter::point ? GL_POINT : GL_LINEAR_MIPMAP_LINEAR;
			GLenum mag_filter = state.filter == hal::texture_filter::point ? GL_POINT : GL_LINEAR;

			glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrap );
			glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrap );
			glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, min_filter );
			glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, mag_filter );
			if ( state.filter == hal::texture_filter::aniso )
				glTexParameterf( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy );

			hal::check_errors();
		}

		void context_internal::bind( hal::effect* e )
		{
			assert( e );

			hal::effect* previous_effect = bound_effect;

			e->bind();
			bound_effect = e;
			device_internal::addref( e );

			if ( previous_effect ) device_internal::release( previous_effect );
			hal::check_errors();
		}

		void context_internal::set_constant( hal::effect* e, const char* block_name, const char* uniform_name, const void* data, size_t size )
		{
			assert( e );
			e->set_uniform( block_name, uniform_name, data, size );
		}

		void context_internal::set_constant_block( hal::effect* e, const char* block_name, const void* data, size_t size )
		{
			assert( e );
			e->set_uniform_block( block_name, data, size );
		}

		void context_internal::bind( unsigned int index, hal::cbuffer* c )
		{
			assert( c );
			glBindBufferBase( GL_UNIFORM_BUFFER, index, c->buffer );
			hal::check_errors();
		}

		void context_internal::update_constant_block( hal::cbuffer* c, void* data, size_t data_size, hal::cbuffer_usage usage )
		{
			assert( c );
			assert( data );
			assert( data_size <= c->size );
			glBindBuffer( GL_UNIFORM_BUFFER, c->buffer );
			glBufferData( GL_UNIFORM_BUFFER, data_size, data, usage == hal::cbuffer_usage::static_draw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW );
			glBindBuffer( GL_UNIFORM_BUFFER, 0 );
			hal::check_errors();
		}

		unsigned int context_internal::get_uniform_block_index( hal::effect* e, const char* block_name )
		{
			assert( e );
			return e->get_uniform_block_index( block_name );
		}

		void context_internal::draw( hal::mesh* m )
		{
			assert( m );
			assert( bound_effect );
			if ( bound_effect->compiled() )
			{
				bound_effect->update_uniforms();
				glBindVertexArray( m->vao );
				glBindBuffer( GL_ARRAY_BUFFER, m->vbo );
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m->ebo );
				glDrawElements( gl_prim_type( m->prim ), m->primitive_count * indices_per_primitive( m->prim ), GL_UNSIGNED_INT, nullptr );
				hal::check_errors();
			}
		}

		void context_internal::draw_instanced( hal::mesh* m, unsigned int count )
		{
			if ( bound_effect->compiled() )
			{
				assert( m );
				assert( bound_effect );
				bound_effect->update_uniforms();
				glBindVertexArray( m->vao );
				glBindBuffer( GL_ARRAY_BUFFER, m->vbo );
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m->ebo );
				glDrawElementsInstanced( gl_prim_type( m->prim ), m->primitive_count * indices_per_primitive( m->prim ), GL_UNSIGNED_INT, nullptr, count );
				hal::check_errors();
			}
		}

		void context_internal::begin_instancing( hal::mesh* m )
		{
			assert( m );
			assert( !instancing_mesh );
			assert( bound_effect );
			glBindVertexArray( m->vao );
			glBindBuffer( GL_ARRAY_BUFFER, m->vbo );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m->ebo );
			instancing_mesh = m;
		}

		void context_internal::draw_instances( unsigned int count )
		{
			if ( bound_effect->compiled() )
			{
				bound_effect->update_uniforms();
				glDrawElementsInstanced( gl_prim_type( instancing_mesh->prim ), instancing_mesh->primitive_count * indices_per_primitive( instancing_mesh->prim ), GL_UNSIGNED_INT, nullptr, count );
			}
		}

		void context_internal::end_instancing()
		{
			instancing_mesh = nullptr;
			hal::check_errors();
		}

		hal::mesh_readback context_internal::readback_mesh( hal::mesh* m )
		{
			assert( m );
			assert( m->vertex_data_copy );
			return hal::mesh_readback{ m->fmt, m->vertex_data_copy, m->index_data_copy, m->primitive_count * indices_per_primitive( m->prim ), m->vertex_count };
		}

		glm::vec2 context_internal::measure_text( hal::font* f, const char* text )
		{
			glm::vec2 pen;
			glm::vec2 min( std::numeric_limits<float>::max(), std::numeric_limits<float>::max() );
			glm::vec2 max( std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() );

			const char* prev = nullptr;
			const char* c = text;
			const hal::font_glyph* glyph;
			while ( *c != '\0' )
			{
				glyph = &device_internal::get_glyph( f, *c );

				glm::vec2 kern;
				if ( prev ) kern = device_internal::get_kerning( f, *prev, *c );
				pen += kern;

				glm::vec2 glyph_min( pen.x + glyph->origin_x, pen.y - glyph->origin_y );
				glm::vec2 glyph_max( glyph_min.x + glyph->width, glyph_min.y + glyph->height );

				min = glm::min( min, glyph_min );
				max = glm::max( max, glyph_max );

				pen += glm::vec2( glyph->advance_x, glyph->advance_y );
				prev = c;
				++c;
			}

			return max - min;
		}

		void context_internal::draw( hal::font* f, const glm::mat4& transform, const glm::vec4& color, const char* text )
		{
			auto it = f->string_cache.find( text );
			if ( it == f->string_cache.end() )
			{
				hal::debug::print( "Building string '%s'", text );
				hal::font_string new_str;
				new_str.length = strlen( text );
				new_str.instance_data = device_internal::create_cbuffer( sizeof( glm::vec4 ) * new_str.length );
				glm::vec2 pen;
				const char* prev = nullptr;
				const char* c = text;
				std::vector<glm::vec4> instances;
				for ( unsigned int i = 0; i < new_str.length; ++i )
				{
					auto glyph = device_internal::get_glyph( f, *c );
					glm::vec2 kern;
					if ( prev ) kern = device_internal::get_kerning( f, *prev, *c );
					pen += kern;
					glm::vec3 origin( pen.x + glyph.origin_x, pen.y - glyph.origin_y, 0.f );

					instances.push_back( glm::vec4( origin.x, origin.y, glyph.array_slice, 0.f ) );

					pen += glm::vec2( glyph.advance_x, glyph.advance_y );
					prev = c;
					++c;
				}
				assert( instances.size() == new_str.length );
				update_constant_block( new_str.instance_data, &instances[0], instances.size() * sizeof( glm::vec4 ), hal::cbuffer_usage::static_draw );
				f->string_cache.insert( std::make_pair( text, new_str ) );

				// todo: is there a way to avoid this second lookup? really clunky...
				it = f->string_cache.find( text );
			}
			auto& font_str = it->second;
			font_str.last_use = device_internal::current_frame();

			hal::rasterizer_state rast{ false, true, false, false };
			set_rasterizer_state( rast );

			auto effect = device_internal::get_text_effect();
			set_blend_state( hal::blend_state::alpha );
			bind( effect );

			bind( 0, f->textures );
			bind( get_uniform_block_index( effect, "instance_data" ), font_str.instance_data );

			context::set_constant( effect, "shared_data", "color", color );
			context::set_constant( effect, "shared_data", "transform", transform );

			draw_instanced( f->glyph_mesh, font_str.length );
		}
	}
}
