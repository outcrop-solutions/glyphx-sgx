
#pragma once

#include "../platform.h"

namespace SynGlyphX
{
	class GlyphScene;
	class BaseImageRenderer;
	class LinksRenderer;
	namespace render { class grid_renderer; }

	namespace LegacySceneReader
	{
		SGXSCENEVIEWER_API bool LoadLegacyScene( GlyphScene& scene, BaseImageRenderer& base_images, LinksRenderer& links, render::grid_renderer& grids, hal::texture* default_base_texture, const char* mainCSV, const char* tagCSV, const std::vector<hal::texture*>& base_image_textures );
	}
}
