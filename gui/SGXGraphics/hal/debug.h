
#pragma once

#include "../platform.h"
#include <glm/glm.hpp>

// #define HAL_BREAK_ON_ASSERT

namespace SynGlyphX
{
	namespace hal
	{
		namespace debug
		{
			SGXGRAPHICS_API void print( const char* message, ... );
			SGXGRAPHICS_API void print_noln( const char* message, ... );
			SGXGRAPHICS_API void debug_break();
			SGXGRAPHICS_API void _assert( bool condition, const char* message, ... );
			SGXGRAPHICS_API void print_matrix( const glm::mat4& mat );

			class SGXGRAPHICS_API profile_timer
			{
			public:
				profile_timer();
				float elapsed_s();
				unsigned int elapsed_ms();
				void print_ms_to_debug( const char* message, ... );
			private:
				unsigned int start;
			};
		}
	}
}
