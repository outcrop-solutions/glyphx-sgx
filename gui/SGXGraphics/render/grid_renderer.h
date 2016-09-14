
#pragma once

#include "../platform.h"
#include <glm/glm.hpp>

namespace SynGlyphX
{
	namespace hal { class context; class mesh; class effect; }

	namespace render
	{
		class camera;

		class SGXGRAPHICS_API grid_renderer
		{
		public:
			grid_renderer();
			grid_renderer( const grid_renderer& ) = delete;
			~grid_renderer();

			void add( const glm::mat4& transform, const glm::vec2& dimensions, unsigned int divisions_x, unsigned int divisions_y, const glm::vec4& base_color );
			void clear();
			void draw( hal::context* context, render::camera* camera, const glm::vec4& color );
		private:
			hal::effect* effect;
			struct grid_info
			{
				hal::mesh* mesh;
				glm::vec4 base_color;
				glm::mat4 transform;
			};
			std::vector<grid_info> grids;
		};
	}
}

