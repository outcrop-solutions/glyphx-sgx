
#pragma once

#include "../platform.h"
#include "types.h"
#include <functional>

namespace SynGlyphX
{
	namespace hal
	{
		SGXGRAPHICS_API void for_each_triangle( const mesh_readback& m, std::function<void( const glm::vec3&, const glm::vec3&, const glm::vec3& )> fn );
	}
}
