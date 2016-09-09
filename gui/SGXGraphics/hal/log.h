
#pragma once

#include <string>
#include "../platform.h"

namespace SynGlyphX
{
	namespace hal
	{
		typedef std::pair< std::string, std::string > message;

		class SGXGRAPHICS_API log
		{
		public:
			static void error( const char* err, const char* detail = "", ... );
			static bool has_errors();
			static message next_error();
		};
	}
}
