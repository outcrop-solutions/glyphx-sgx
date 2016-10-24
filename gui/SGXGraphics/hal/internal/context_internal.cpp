
#include "context_internal.h"
#include "device_internal.h"
#include "gl.h"
#include "types_internal.h"
#include "effect.h"
#include "../debug.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace SynGlyphX
{
	namespace hal_gl
	{
		context_internal::context_internal()
			: bound_target_set( nullptr ), bound_effect( nullptr ), instancing_mesh( nullptr )
		{
			for ( unsigned int i = 0; i < max_bound_textures; ++i )
				bound_textures[i] = nullptr;
			glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy );

			reset_defaults();
		}

		context_internal::~context_internal()
		{
			if ( bound_target_set )
				device_internal::release( bound_target_set );
			if ( bound_effect )
				device_internal::release( bound_effect );
			for ( unsigned int i = 0; i < max_bound_textures; ++i )
				if ( bound_textures[i] ) device_internal::release( bound_textures[i] );
		}

		void context_internal::reset_defaults_internal()
		{
			glBindVertexArray( 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			glPixelStorei( GL_PACK_ALIGNMENT, 1 );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		}

		void context_internal::unbind_all_textures()
		{
			for ( int i = 0; i < max_bound_textures; ++i )
				bind( i, ( hal::texture* )nullptr );
		}

		void context_internal::bind( hal::render_target_set* set )
		{
			if ( bound_target_set != set )
			{
				if ( bound_target_set )
					device_internal::release( bound_target_set );

				if ( set )
				{
#ifdef _DEBUG
					for ( unsigned int i = 0u; i < max_bound_textures; ++i )
						for ( auto t : set->color_targets )
							hal::debug::_assert( bound_textures[i] != t, "trying to bind a render target set, but one of its targets is bound as a texture" );
#endif
					device_internal::addref( set ); // make sure the target doesn't get released while bound
					assert( set->color_targets.size() > 0u );
					GLenum buffers[16];
					for ( unsigned int i = 0u; i < set->color_targets.size(); ++i )
						buffers[i] = GL_COLOR_ATTACHMENT0 + i;

					glBindFramebuffer( GL_FRAMEBUFFER, set->fb );
					glDrawBuffers( set->color_targets.size(), buffers );
					assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
				}
				else
				{
					glBindFramebuffer( GL_FRAMEBUFFER, device_internal::get_default_render_target() );
				}

				bound_target_set = set;
			}
			hal::check_errors();
		}

		void context_internal::clear( hal::clear_type type, const glm::vec4& color, float depth )
		{
			if ( type & hal::clear_type::color ) glClearColor( color.r, color.g, color.b, color.a );
			if ( type & hal::clear_type::depth ) glClearDepth( depth );

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
			if ( bound_textures[index] != t )
			{
				assert( index < max_bound_textures );
				if ( bound_textures[index] ) device_internal::release( bound_textures[index] );
				if ( t ) t->addref();
				bound_textures[index] = t;

				auto handle = t ? t->handle : 0u;

#ifdef _DEBUG
				if ( bound_target_set )
					for ( auto rt : bound_target_set->color_targets )
						hal::debug::_assert( t != rt, "trying to bind a texture that is currently in use as a render target; this is unsupported" );
#endif

				glActiveTexture( GL_TEXTURE0 + index );
				glBindTexture( GL_TEXTURE_2D, handle );

				if ( t )
				{
					GLenum wrap = state.wrap == hal::texture_wrap::wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE;

					GLenum min_filter = state.filter == hal::texture_filter::point ? GL_POINT : ( t->has_mipchain ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR );
					GLenum mag_filter = state.filter == hal::texture_filter::point ? GL_POINT : GL_LINEAR;

					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter );
					if ( state.filter == hal::texture_filter::aniso )
						glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy );
				}
			}
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
				glyph = &get_glyph( f, *c );

				glm::vec2 kern;
				if ( prev ) kern = get_kerning( f, *prev, *c );
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
				hal::font_string new_str;
				new_str.length = strlen( text );
				new_str.instance_data = device_internal::create_cbuffer( sizeof( glm::vec4 ) * new_str.length );
				glm::vec2 pen;
				const char* prev = nullptr;
				const char* c = text;
				std::vector<glm::vec4> instances;
				for ( unsigned int i = 0; i < new_str.length; ++i )
				{
					auto glyph = get_glyph( f, *c );
					glm::vec2 kern;
					if ( prev ) kern = get_kerning( f, *prev, *c );
					pen += kern;
					glm::vec3 origin( pen.x + glyph.origin_x, pen.y - glyph.origin_y, 0.f );

					instances.push_back( glm::vec4( origin.x, origin.y, glyph.array_slice, 0.f ) );

					pen += glm::vec2( glyph.advance_x, glyph.advance_y );
					prev = c;
					++c;
				}
				assert( instances.size() == new_str.length );
				update_constant_block( new_str.instance_data, &instances[0], instances.size() * sizeof( glm::vec4 ), hal::cbuffer_usage::static_draw );
				it = f->string_cache.insert( std::make_pair( text, new_str ) ).first;
			}
			auto& font_str = it->second;
			font_str.last_use = device_internal::current_frame();

			hal::rasterizer_state rast{ false, true, false, false };
			set_rasterizer_state( rast );

			auto effect = device_internal::get_text_effect();
			set_blend_state( hal::blend_state::alpha );
			bind( effect );

            bind( 0, f->textures, { hal::texture_wrap::clamp, hal::texture_filter::linear } );
			bind( get_uniform_block_index( effect, "instance_data" ), font_str.instance_data );

			context::set_constant( effect, "shared_data", "color", color );
			context::set_constant( effect, "shared_data", "transform", transform );

			draw_instanced( f->glyph_mesh, font_str.length );
		}
        
		const hal::font_glyph& context_internal::get_glyph( hal::font* f, char c )
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
				g.origin_x = int16_t( glyph->bitmap_left );
				g.origin_y = int16_t( glyph->bitmap_top );
				g.advance_x = int16_t( glyph->advance.x >> 6 );
				g.advance_y = int16_t( glyph->advance.y >> 6 );

				g.array_slice = f->next_slice++;

                device_internal::clear_array_slice( f->textures, g.array_slice );
                if ( glyph->bitmap.buffer )
				{
					hal::pixel_rect rect{ 0u, 0u, glyph->bitmap.width, glyph->bitmap.rows };
					device_internal::update_array_slice( f->textures, g.array_slice, rect, glyph->bitmap.buffer );
				}

				FT_Glyph ftg = nullptr;
				FT_Get_Glyph( glyph, &ftg );
				FT_BBox bb;
				FT_Glyph_Get_CBox( ftg, FT_GLYPH_BBOX_PIXELS, &bb );
				FT_Done_Glyph( ftg );
				g.width = uint16_t( bb.xMax - bb.xMin );
				g.height = uint16_t( bb.yMax - bb.yMin );

				f->glyphs.insert( std::make_pair( c, g ) );
			}

			hal::check_errors();
			return f->glyphs.find( c )->second;
		}

		const glm::vec2 context_internal::get_kerning( hal::font* f, char left, char right )
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
