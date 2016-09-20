
#pragma once

#include "../context.h"

namespace SynGlyphX
{
	namespace hal { struct font_glyph; }

	namespace hal_gl
	{
		class context_internal : public hal::context
		{
		public:
			context_internal();
			context_internal( const context_internal& ) = delete;
			virtual ~context_internal();

			void bind( hal::render_target_set* set ) override;
			void clear( hal::clear_type type, const glm::vec4& color, float depth ) override;

			void set_depth_state( hal::depth_state state ) override;
			void set_blend_state( hal::blend_state state ) override;
			void set_rasterizer_state( const hal::rasterizer_state& state ) override;

			void bind( unsigned int index, hal::texture* t, const hal::sampler_state& state = hal::sampler_state() ) override;
			void bind( unsigned int index, hal::texture_array* t, const hal::sampler_state& state = hal::sampler_state() ) override;

			void bind( hal::effect* e ) override;
			void set_constant( hal::effect* e, const char* block_name, const char* uniform_name, const void* data, size_t size ) override;
			void set_constant_block( hal::effect* e, const char* block_name, const void* data, size_t size ) override;

			void bind( unsigned int index, hal::cbuffer* c ) override;
			void update_constant_block( hal::cbuffer* c, void* data, size_t data_size, hal::cbuffer_usage usage ) override;
			unsigned int get_uniform_block_index( hal::effect* e, const char* block_name ) override;

			void draw( hal::mesh* m ) override;
			void draw_instanced( hal::mesh* m, unsigned int count ) override;

			void begin_instancing( hal::mesh* m ) override;
			void draw_instances( unsigned int count ) override;
			void end_instancing() override;

			hal::mesh_readback readback_mesh( hal::mesh* m ) override;

			void draw( hal::font* f, const glm::mat4& transform, const glm::vec4& color, const char* text ) override;
			glm::vec2 measure_text( hal::font* f, const char* text ) override;

			hal::effect* get_bound_effect() { return bound_effect; }

		protected:
			void reset_defaults_internal() override;

		private:
			hal::effect* bound_effect;
			hal::mesh* instancing_mesh;
			float max_anisotropy;
		};
	}
}
