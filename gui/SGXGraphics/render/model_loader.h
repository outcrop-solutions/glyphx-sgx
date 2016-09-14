
#pragma once

#include "../platform.h"

namespace SynGlyphX
{
	namespace render
	{
		class model;

		struct model_loader_settings
		{
			bool support_readback;
		};
		SGXGRAPHICS_API model_loader_settings model_loader_defaults();

		SGXGRAPHICS_API model* load_model( const char* file, const model_loader_settings& settings = model_loader_defaults() );
	}
}
