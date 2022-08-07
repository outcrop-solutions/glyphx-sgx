
#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <render/types.h>
#include <QtCore/QMap>
#include <QtCore/QList>
#include "legacyglyphplacement.h"

namespace SynGlyphX
{
	namespace hal { class texture; }
	class BaseImageRenderer;
	class GlyphScene;
	namespace render { class grid_renderer; }

	class SceneReader
	{
	public:

		struct StackedGlyph {

			QList<int> glyphIds;
			float posZ;
			float scaleZ;
			GlyphPlacementData gpd;
			double tagValue;

		};

		void read( const char* scenefilename, const char* countfilename, GlyphScene& scene, BaseImageRenderer& base_images, const std::vector<hal::texture*>& base_image_textures, hal::texture* default_base_texture, render::grid_renderer& grids );
		QMap<QString, StackedGlyph> getStackedGlyphMap() { return stackedGlyphs; }
		QMap<int, int> getFilteringIndexMap() { return filteringIndexMap; }
		QMap<int, QString> getIndexToUID() { return indexToUID; }
		//float getShortestGlyph() { return allScaleZ.at(0); }
		//float getTallestGlyph() { return allScaleZ.at(allScaleZ.size()-1); }
		//int getFirstStackedIndex() { return indexOfFirstStacked; }
		//int getLastStackedIndex() { return indexOfLastStacked; }

	private:
		void read_base_image( BaseImageRenderer& base_images, const std::vector<hal::texture*>& base_image_textures, hal::texture* default_base_texture, render::grid_renderer& grids );
		void read_glyph_element(GlyphScene& scene);
		void create_stacked_glyph(QString uid, GlyphPlacementData data, std::string tag, GlyphScene& scene);
		void read_link( GlyphScene& scene );

		float findClosest(int n, float target);
		float getClosest(float val1, float val2, float target);
		
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

		QMap<int, QString> indexToUID;
		QMap<QString, StackedGlyph> stackedGlyphs;
		QMap<float, render::packed_color> colorMapping;
		QList<float> allScaleZ;
		QMap<int, int> filteringIndexMap;
		//int indexOfFirstStacked;
		//int indexOfLastStacked;
		
	};
}
