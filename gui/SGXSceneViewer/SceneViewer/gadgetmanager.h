
#pragma once

#include <functional>
#include <render/renderer.h>

namespace SynGlyphX
{
	namespace hal { class mesh; class effect; }
	namespace render { class model; class perspective_camera; }
	class GlyphScene;

	class GadgetManager
	{
	public:
		GadgetManager( GlyphScene& scene );
		GadgetManager( const GadgetManager& ) = delete;
		~GadgetManager();

		// returns {group id, distance}
		std::pair<unsigned int, float> pick( const render::perspective_camera* camera, const glm::vec3& origin, const glm::vec3& dir, float max_distance );
		void create( std::function<void( void )> on_click, unsigned int group_idx, const glm::vec3& position, float scale );
		void clear();

		void render( hal::context* context, render::perspective_camera* camera );

	private:
		struct gadget
		{
			unsigned int group;
			glm::vec3 position;
			float scale;
			float exploded_offset;
			std::function<void( void )> on_click;
		};
		std::vector<gadget> gadgets;
		hal::mesh* billboard_mesh;
		hal::effect* effect;
		hal::effect* switch_effect;
		hal::texture* texture;
		render::renderer renderer, switch_renderer;
		GlyphScene& scene;

		render::model* gadget_model;
		render::model* switch_model;

		glm::mat4 compute_gadget_transform( const gadget& g );
		glm::mat4 compute_switch_transform( const render::perspective_camera* camera, const gadget& g );
	};
}
