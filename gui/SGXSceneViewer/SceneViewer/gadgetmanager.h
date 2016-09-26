
#pragma once

#include <functional>
#include <render/renderer.h>

namespace SynGlyphX
{
	namespace hal { class mesh; class effect; }
	namespace render { class model; class perspective_camera; }

	class GadgetManager
	{
	public:
		GadgetManager();
		GadgetManager( const GadgetManager& ) = delete;
		~GadgetManager();

		void pick( const glm::vec3& origin, const glm::vec3& dir );
		void create( std::function<void( void )> on_click, const glm::vec3& position );
		void clear();

		void render( hal::context* context, render::perspective_camera* camera );

	private:
		struct gadget
		{
			render::model* model;
			glm::vec3 position;
			std::function<void( void )> on_click;
		};
		std::vector<gadget> gadgets;
		hal::mesh* billboard_mesh;
		hal::effect* effect;
		hal::texture* texture;
		render::renderer renderer;
	};
}
