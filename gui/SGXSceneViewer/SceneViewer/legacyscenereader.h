
#pragma once

#include "../platform.h"
#include <vector>

namespace SynGlyphX
{
	class GlyphScene;
	class GlyphGeometryDB;
	class BaseImageRenderer;
    namespace hal { class texture; }
	namespace render { class grid_renderer; }

	namespace LegacySceneReader
	{
		SGXSCENEVIEWER_API bool LoadLegacyScene( GlyphScene& scene, const GlyphGeometryDB& db, BaseImageRenderer& base_images, render::grid_renderer& grids, hal::texture* default_base_texture, const char* mainCSV, const char* tagCSV, const std::vector<hal::texture*>& base_image_textures );
	}
}
