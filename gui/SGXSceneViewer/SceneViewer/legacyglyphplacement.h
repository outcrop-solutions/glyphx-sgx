
#pragma once

#include "../platform.h"
#include <glm/glm.hpp>

namespace SynGlyphX
{
	class PlacementPolicy;

	struct GlyphCSVData
	{
		bool is_root;
		int geom_type, topo;
		int id, parent_id;
		glm::vec4 color;
		glm::vec3 pos, rot, scale;
		int color_index;
		float ratio;
		glm::vec3 rotation_rates;
	};

	PlacementPolicy* ChoosePlacementPolicy( const GlyphCSVData& glyphData );
}
