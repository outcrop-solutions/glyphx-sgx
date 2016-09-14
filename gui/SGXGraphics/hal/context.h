
#pragma once

#include "../platform.h"
#include "types.h"

namespace SynGlyphX
{
	namespace hal
	{
		class SGXGRAPHICS_API context
		{
		public:
			virtual ~context() {}

			void reset_defaults();

			virtual void clear( clear_type type, const glm::vec4& color = glm::vec4( 0.f, 0.f, 0.f, 1.f ) ) = 0;

			virtual void set_depth_state( depth_state state ) = 0;
			virtual void set_blend_state( blend_state state ) = 0;
			virtual void set_rasterizer_state( const rasterizer_state& state ) = 0;

			virtual void bind( unsigned int index, texture* t, const sampler_state& state = sampler_state() ) = 0;

			virtual void bind( effect* e ) = 0;
			template<typename T> void set_constant( effect* e, const char* block_name, const char* uniform_name, const T& data );
			virtual void set_constant( effect* e, const char* block_name, const char* uniform_name, const void* data, size_t size ) = 0;
			virtual void set_constant_block( effect* e, const char* block_name, const void* data, size_t size ) = 0;

			// This API allows finer-grained control over when constant blocks are created and set, by allowing the user to update and bind
			// them seperately from effects. Since it's more complex it should only be used when necessary for performance. (Example use cases
			// for this API would be: sharing a constant block between two effects, or issuing multiple draws with the same effect but
			// different constant blocks (without having to repeatedly update the constant data).
			// NOTE: you MUST call device::set_cbuffer_external on the uniform block you want in order to use this feature!
			virtual void bind( unsigned int index, cbuffer* c ) = 0;
			virtual void update_constant_block( cbuffer* c, void* data, size_t data_size, cbuffer_usage usage ) = 0;
			virtual unsigned int get_uniform_block_index( effect* e, const char* block_name ) = 0;

			virtual void draw( mesh* m ) = 0;
			virtual void draw_instanced( mesh* m, unsigned int count ) = 0;

			virtual void begin_instancing( mesh* m ) = 0;
			virtual void draw_instances( unsigned int count ) = 0;
			virtual void end_instancing() = 0;

			virtual mesh_readback readback_mesh( mesh* m ) = 0;
			
			virtual void draw( font* f, const glm::mat4& transform, const char* text ) = 0;
			virtual texture* get_glyph_texture( font* f, char c ) = 0;

		protected:
			virtual void reset_defaults_internal() { }
		};

		template<typename T> void context::set_constant( effect* e, const char* block_name, const char* uniform_name, const T& data )
		{
			set_constant( e, block_name, uniform_name, &data, sizeof( data ) );
		}
	}
}
