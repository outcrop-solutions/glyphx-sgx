
#pragma once

#include "sgxutility.h"
#include <functional>

namespace SynGlyphX
{
	// Interface to get information about loaded glyphs from the engine.
	class LoadedGlyphDataInterface
	{
	public:
		virtual void enumGlyphStrings( std::function<void( int id, int parent_id, int filtering_idx, const char* tag, const char* url, const char* desc )> ) const = 0;
	};
}
