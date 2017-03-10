
#pragma once

#include "../platform.h"
#include <glm/glm.hpp>
#include <render/color.h>

namespace SynGlyphX
{
	class PlacementPolicy;

	struct GlyphPlacementData
	{
		bool is_root;
		int geom_type, topo;
		int id, parent_id, label;
		render::packed_color color;
		glm::vec3 pos, rot, scale;
		float ratio;
		glm::vec3 rotation_rates;
	};

	PlacementPolicy* ChoosePlacementPolicy( const GlyphPlacementData& glyphData );
}
