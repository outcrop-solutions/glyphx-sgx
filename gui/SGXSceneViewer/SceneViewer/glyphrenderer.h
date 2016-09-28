
#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include "glyphnode.h"
#include "glyphscene.h"
#include "../platform.h"

namespace SynGlyphX
{
	class Glyph3DNode; class GlyphGeomteryDB;
	namespace hal { class context; class mesh; class effect; class cbuffer; };
	namespace render { class perspective_camera; }

	class SGXSCENEVIEWER_API GlyphRenderer
	{
	public:
		GlyphRenderer( GlyphGeometryDB& db );
		GlyphRenderer( const GlyphRenderer& ) = delete;
		virtual ~GlyphRenderer();

		void setScene( GlyphScene* _scene ) { scene = _scene; }
		void clear();

		void render_solid( hal::context* context, render::perspective_camera* camera, float elapsed_seconds, bool active_group_only = false );
		void render_blended( hal::context* context, render::perspective_camera* camera, float elapsed_seconds, bool active_group_only = false );

		void setWireframeMode( bool val ) { global_wireframe = val; }
		void enableAnimation( bool val ) { animation = val; }

		void setFilterAlpha( float alpha = 0.5f, float wire_alpha = 0.1f ) { filter_alpha = alpha; wire_filter_alpha = wire_alpha; }

		enum BoundVisMode
		{
			Individual,
			Combined,
		};

		void enableBoundVis( bool enable );
		void setBoundVisMode( BoundVisMode mode );
		BoundVisMode getBoundVisMode() { return bound_vis_mode; }
		bool boundVisEnabled() { return bound_vis_enabled; }

		void enableSelectionEffect( bool val ) { sel_effect_enabled = val; }
		void resetSelectionAnimation() { selection_animation_state = 0.f; }

	private:
		class glyph_bucket
		{
		public:
			~glyph_bucket();
			void add_instance( hal::mesh* mesh, const glm::mat4& transform, const glm::vec4& color_or_bound, const glm::vec3& anim_axis, float anim_rate, const glm::vec3& anim_center, const glm::vec3& alt_pos = glm::vec3(), float alt_pos_group = 0.f );
			void update_instances( hal::context* context, bool& updates_done );
			void draw( hal::context* context, unsigned int transform_binding_point, unsigned int material_binding_point, unsigned int anim_binding_point, unsigned int alt_pos_binding_point );
			void clear();
		private:
			struct instance_data
			{
				std::vector<glm::mat4> transforms;
				std::vector<glm::vec4> colors_or_bounds;	// colors for regular glyphs, bounds for selection effect
				std::vector<glm::vec4> animation;
				std::vector<glm::vec4> alternate_positions;
				hal::cbuffer* transform_buffer;
				hal::cbuffer* color_bound_buffer;
				hal::cbuffer* anim_buffer;
				hal::cbuffer* alt_pos_buffer;
				bool dirty;
			};
			typedef std::vector<instance_data> instance_data_list;
			std::unordered_map<hal::mesh*, instance_data_list> instances;
		};

		std::unordered_map<uint32_t, glyph_bucket> buckets;
		glyph_bucket& get_bucket( uint16_t bucket_id, uint16_t group_id );
		void process_buckets( uint16_t flags_on, uint16_t flags_off, uint16_t group_id, uint16_t group_exclude, std::function<void(glyph_bucket&)> fn );
		void draw_buckets( hal::context* context, uint16_t flags, uint16_t group_id, uint16_t group_exclude, unsigned int transform_binding_point, unsigned int material_binding_point, unsigned int anim_binding_point, unsigned int alt_pos_binding_point );

		hal::effect* glyph_effect;
		hal::effect* selection_effect;
		unsigned int transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point;
		unsigned int sel_transform_binding_point, sel_bound_binding_point, sel_anim_binding_point, sel_alt_pos_binding_point;
		float selection_anim_max_scale;
		bool bound_vis_enabled;
		BoundVisMode bound_vis_mode;
		bool sel_effect_enabled;

		void add( const GlyphScene& scene );
		void add_bound_to_bucket( const Glyph3DNode& glyph, glyph_bucket& bucket );

		void update_instances( hal::context* context );

		bool global_wireframe, animation;
		float filter_alpha, wire_filter_alpha;

		GlyphScene* scene;

		void renderSelection( hal::context* context, render::perspective_camera* camera, float elapsed_seconds );
		float selection_animation_time, selection_animation_state;
		float previous_frame_time;
		bool updates_done = false;

		GlyphGeometryDB& db;
	};
}
