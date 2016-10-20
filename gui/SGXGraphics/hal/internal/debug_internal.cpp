
#include "../debug.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>

#ifdef WIN32
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

namespace SynGlyphX
{
	namespace hal
	{
		namespace debug
		{
            namespace
            {
                const unsigned int buf_size = 1024u;
            }
            
			void print( const char* message, ... )
			{
				char buf0[buf_size];
				va_list args;
				va_start( args, message );
#ifdef WIN32
				vsprintf_s( buf0, message, args );
#else
                vsnprintf( buf0, buf_size, message, args );
#endif
				va_end( args );
				strcat( buf0, "\n" );

#ifdef WIN32
                OutputDebugStringA( buf0 );
#endif
			}

			void print_noln( const char* message, ... )
			{
				char buf0[buf_size];
				va_list args;
				va_start( args, message );
#ifdef WIN32
				vsprintf_s( buf0, message, args );
#else
                vsnprintf( buf0, buf_size, message, args );
#endif
				va_end( args );

#ifdef WIN32
				OutputDebugStringA( buf0 );
#endif
			}

			void debug_break()
			{
#if defined(WIN32)
				__debugbreak();
#elif defined(__APPLE__)
                __asm__("int $3");  // debug break on OSX
#endif
			}

			void _assert( bool condition, const char* message, ... )
			{
				if ( !condition )
				{
					char buf0[buf_size];
					va_list args;
					va_start( args, message );
#ifdef WIN32
					vsprintf_s( buf0, message, args );
#else
                    vsnprintf( buf0, buf_size, message, args );
#endif
					va_end( args );
					strcat( buf0, "\n" );

#ifdef WIN32
					OutputDebugStringA( buf0 );
#endif
					
#ifdef HAL_BREAK_ON_ASSERT
					debug_break();
#endif
				}
			}

			profile_timer::profile_timer()
#ifdef WIN32
            : start( timeGetTime() )
#endif
			{
			}

			float profile_timer::elapsed_s()
			{
#ifdef WIN32
				return float( timeGetTime() - start ) / 1000.f;
#else
                return 0.f; // not yet implemented
#endif
			}

			unsigned int profile_timer::elapsed_ms()
			{
#ifdef WIN32
                return timeGetTime() - start;
#else
                return 0u; // not yet implemented
#endif
			}

			void profile_timer::print_ms_to_debug( const char* message, ... )
			{
				char buf0[buf_size];
				va_list args;
				va_start( args, message );
#ifdef WIN32
				vsprintf_s( buf0, message, args );
#else
                vsnprintf( buf0, buf_size, message, args );
#endif
				va_end( args );

#ifdef WIN32
				char buf1[buf_size];
				sprintf_s( buf1, "%s: %i ms\n", buf0, elapsed_ms() );
				OutputDebugStringA( buf1 );
#endif
			}
		}
	}
}
