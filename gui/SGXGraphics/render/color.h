
#pragma once

#include "../platform.h"
#include <glm/glm.hpp>

namespace SynGlyphX
{
	namespace render
	{
		typedef uint32_t packed_color;
		SGXGRAPHICS_API packed_color pack_color( const glm::vec4& color );
		SGXGRAPHICS_API packed_color pack_color( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
		SGXGRAPHICS_API glm::vec4 unpack_color( packed_color packed_color );

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
