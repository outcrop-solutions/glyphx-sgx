
#pragma once

#include <functional>
#include <unordered_map>
#include <render/renderer.h>

namespace SynGlyphX
{
    namespace hal { class mesh; class effect; class texture; class font; }
	namespace render { class model; class perspective_camera; }
	class GlyphScene;

	enum class SuperimposedGadgetMode
	{
		Always,
		OnSelection,
	};

	class SuperimposedGroupManager
	{
	public:
		SuperimposedGroupManager( GlyphScene& scene );
		SuperimposedGroupManager( const SuperimposedGroupManager& ) = delete;
		void operator=( const SuperimposedGroupManager& ) = delete;
		~SuperimposedGroupManager();

		// returns {group id, distance}
		std::pair<unsigned int, float> pick( const render::perspective_camera* camera, const glm::vec3& origin, const glm::vec3& dir, float max_distance );
		void create( unsigned int group_idx, const glm::vec3& position, float scale );
		void clear();

		void render( hal::context* context, render::perspective_camera* camera );
		void setMode( SuperimposedGadgetMode _mode ) { mode = _mode; }

	private:
		struct gadget
		{
			unsigned int group;
			glm::vec3 position;
			float scale;
			float exploded_offset;
			float bound_alpha, switch_alpha;
		};
		std::unordered_map<unsigned int, gadget> gadgets;
		hal::mesh* billboard_mesh;
		hal::effect* effect;
		hal::effect* switch_effect;
		GlyphScene& scene;

		render::model* gadget_model;
		render::model* switch_model;

		hal::font* font;

		hal::texture* explode_icon;
		hal::texture* collapse_icon;

		SuperimposedGadgetMode mode;
		bool groupInSelection( unsigned int group );
		unsigned int selectedGroup();

		glm::mat4 compute_gadget_transform( const gadget& g );
		float compute_gadget_alpha( const gadget& g, const glm::vec3& cam_pos, const glm::vec2& fade_dist );
		glm::mat4 compute_switch_transform( const render::perspective_camera* camera, const gadget& g );

		void update( render::perspective_camera* camera );
	};
}
