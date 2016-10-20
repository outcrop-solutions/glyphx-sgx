
#pragma once

#include "../platform.h"
#include <glm/glm.hpp>
#include <chrono>

// #define HAL_BREAK_ON_ASSERT

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (P)
#endif

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
			SGXGRAPHICS_API void print_val( const glm::mat4& mat );
			SGXGRAPHICS_API void print_val( const glm::vec3& vec );
			SGXGRAPHICS_API void print_val( const glm::vec4& vec );

			class SGXGRAPHICS_API profile_timer
			{
			public:
				profile_timer();
				float elapsed_s();
				unsigned int elapsed_ms();
				void print_ms_to_debug( const char* message, ... );
			private:
				std::chrono::high_resolution_clock::time_point start;
			};
		}
	}
}
