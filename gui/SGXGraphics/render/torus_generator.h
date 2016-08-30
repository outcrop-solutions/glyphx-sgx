
#pragma once

#include "../platform.h"

namespace SynGlyphX
{
	namespace render
	{
		class model;

		SGXGRAPHICS_API render::model* generate_torus( float radius, float ring_radius, unsigned int segments, unsigned int rings, bool support_readback = false );
	}
}
