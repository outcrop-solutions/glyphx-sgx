
#pragma once

#include "../platform.h"
#include <glm/glm.hpp>

namespace SynGlyphX
{
	namespace render
	{
		namespace color
		{
			SGXGRAPHICS_API glm::vec4 red();
			SGXGRAPHICS_API glm::vec4 green();
			SGXGRAPHICS_API glm::vec4 blue();
			SGXGRAPHICS_API glm::vec4 cyan();
			SGXGRAPHICS_API glm::vec4 magenta();
			SGXGRAPHICS_API glm::vec4 yellow();
			SGXGRAPHICS_API glm::vec4 black();
			SGXGRAPHICS_API glm::vec4 white();
			SGXGRAPHICS_API glm::vec4 clear_black();
		}
	}
}
