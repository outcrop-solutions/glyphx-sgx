
#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace SynGlyphX
{
	namespace hal { class texture; }
	class BaseImageRenderer;
	namespace render { class grid_renderer; }

	class SceneReader
	{
	public:
		void read( const char* filename, BaseImageRenderer& base_images, const std::vector<hal::texture*>& base_image_textures, hal::texture* default_base_texture, render::grid_renderer& grids );
	private:
		int read_int();
		float read_float();
		std::string read_string();
		glm::vec3 read_vec3();
		glm::vec4 read_color();
		glm::vec4 read_solid_color();
		FILE* file;
	};
}
