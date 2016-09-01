
#include "../debug.h"
#include <cstdarg>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

namespace SynGlyphX
{
	namespace hal
	{
		namespace debug
		{
			void print( const char* message, ... )
			{
				char buf0[1024u];
				va_list args;
				va_start( args, message );
				vsprintf_s( buf0, message, args );
				va_end( args );
				strcat_s( buf0, "\n" );

				OutputDebugStringA( buf0 );
			}

			void print_noln( const char* message, ... )
			{
				char buf0[1024u];
				va_list args;
				va_start( args, message );
				vsprintf_s( buf0, message, args );
				va_end( args );

				OutputDebugStringA( buf0 );
			}

			void debug_break()
			{
				__debugbreak();
			}

			void _assert( bool condition, const char* message, ... )
			{
				if ( !condition )
				{
					char buf0[1024u];
					va_list args;
					va_start( args, message );
					vsprintf_s( buf0, message, args );
					va_end( args );
					strcat_s( buf0, "\n" );

					OutputDebugStringA( buf0 );
					
#ifdef HAL_BREAK_ON_ASSERT
					debug_break();
#endif
				}
			}

			profile_timer::profile_timer() : start( timeGetTime() )
			{
			}

			float profile_timer::elapsed_s()
			{
				return float( timeGetTime() - start ) / 1000.f;
			}

			unsigned int profile_timer::elapsed_ms()
			{
				return timeGetTime() - start;
			}

			void profile_timer::print_ms_to_debug( const char* message, ... )
			{
				char buf0[1024u];
				va_list args;
				va_start( args, message );
				vsprintf_s( buf0, message, args );
				va_end( args );

				char buf1[1024u];
				sprintf_s( buf1, "%s: %i ms\n", buf0, elapsed_ms() );
				OutputDebugStringA( buf1 );
			}
		}
	}
}
