
#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include "glyphnode.h"
#include "glyphscene.h"
#include "../platform.h"

namespace SynGlyphX
{
	class Glyph3DNode;
	namespace hal { class context; class mesh; class effect; class cbuffer; };
	namespace render { class perspective_camera; }

	class SGXSCENEVIEWER_API GlyphRenderer
	{
	public:
		GlyphRenderer();
		GlyphRenderer( const GlyphRenderer& ) = delete;
		virtual ~GlyphRenderer();

		void setScene( GlyphScene* _scene ) { scene = _scene; }
		void clear();

		void render_solid( hal::context* context, render::perspective_camera* camera, float elapsed_seconds );
		void render_blended( hal::context* context, render::perspective_camera* camera, float elapsed_seconds );

		void setWireframeMode( bool val ) { global_wireframe = val; }
		void enableAnimation( bool val ) { animation = val; }

		void setFilterMode( FilteredResultsDisplayMode mode, float alpha = 0.5f, float wire_alpha = 0.1f ) { filter_mode = mode; filter_alpha = alpha; wire_filter_alpha = wire_alpha; }

		void enableBoundVis( bool enable );
		bool boundVisEnabled() { return bound_vis_enabled; }

	private:
		class glyph_bucket
		{
		public:
			~glyph_bucket();
			void add_instance( hal::mesh* mesh, const glm::mat4& transform, const glm::vec4& color_or_bound, const glm::vec3& anim_axis, float anim_rate );
			void update_instances( hal::context* context );
			void draw( hal::context* context, unsigned int transform_binding_point, unsigned int material_binding_point, unsigned int anim_binding_point );
			void clear();
		private:
			struct instance_data
			{
				std::vector<glm::mat4> transforms;
				std::vector<glm::vec4> colors_or_bounds;	// colors for regular glyphs, bounds for selection effect
				std::vector<glm::vec4> animation;
				hal::cbuffer* transform_buffer;
				hal::cbuffer* color_bound_buffer;
				hal::cbuffer* anim_buffer;
				bool dirty;
			};
			typedef std::vector<instance_data> instance_data_list;
			std::unordered_map<hal::mesh*, instance_data_list> instances;
		};

		const unsigned int FILTERED = 0u, UNFILTERED = 1u;
		glyph_bucket solid[2], wireframe[2], blended[2], wireframe_blended[2];
		glyph_bucket selection;

		hal::effect* glyph_effect;
		hal::effect* selection_effect;
		unsigned int transform_binding_point, material_binding_point, anim_binding_point;
		unsigned int sel_transform_binding_point, sel_bound_binding_point, sel_anim_binding_point;
		float selection_anim_max_scale;
		bool bound_vis_enabled;

		void add( const GlyphScene& scene );

		void update_instances( hal::context* context );

		bool global_wireframe, animation;

		FilteredResultsDisplayMode filter_mode;
		float filter_alpha, wire_filter_alpha;

		GlyphScene* scene;

		void renderSelection( hal::context* context, render::perspective_camera* camera, float elapsed_seconds );
	};
}
