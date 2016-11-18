
#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <render/types.h>

namespace SynGlyphX
{
	namespace hal { class texture; }
	class BaseImageRenderer;
	class GlyphScene;
	namespace render { class grid_renderer; }

	class SceneReader
	{
	public:
		void read( const char* scenefilename, const char* countfilename, GlyphScene& scene, BaseImageRenderer& base_images, const std::vector<hal::texture*>& base_image_textures, hal::texture* default_base_texture, render::grid_renderer& grids );
	private:
		void read_base_image( BaseImageRenderer& base_images, const std::vector<hal::texture*>& base_image_textures, hal::texture* default_base_texture, render::grid_renderer& grids );
		void read_glyph_element(GlyphScene& scene);
		void read_link( GlyphScene& scene );
		
		int read_int();
		float read_float();
		uint8_t read_byte();
		std::string read_string();
		glm::vec3 read_vec3();
		render::packed_color read_packed_color();
		render::packed_color read_packed_solid_color();
		FILE* file;

		unsigned int root_count;
		int next_filtering_index, next_id;
	};
}
