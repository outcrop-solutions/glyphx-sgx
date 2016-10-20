
#include "../debug.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>

#ifdef WIN32
#pragma warning(disable: 4996)		// windows wants us to use strcat_s but osx doesn't have it
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
				unsigned int ms_to_uint( const std::chrono::milliseconds& ms )
				{
					return static_cast<unsigned int>( ms.count() );	// may be a narrowing cast but >32bit ms are not relevant for profiling
				}

				void debug_print( const char* str )
				{
#ifdef WIN32
					OutputDebugStringA( str );
#else
					printf( "%s", str );
#endif
				}

				void vprint( char* s, size_t n, const char* fmt, va_list arg )
				{
#ifdef WIN32
					vsprintf_s( s, n, fmt, arg );
#else
					vsnprintf( s, n, fmt, arg );
#endif
				}
			}
            
			void print( const char* message, ... )
			{
				char buf0[buf_size];
				va_list args;
				va_start( args, message );
                vprint( buf0, buf_size, message, args );
				va_end( args );
				strcat( buf0, "\n" );

                debug_print( buf0 );
			}

			void print_noln( const char* message, ... )
			{
				char buf0[buf_size];
				va_list args;
				va_start( args, message );
                vprint( buf0, buf_size, message, args );
				va_end( args );

				debug_print( buf0 );
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
                    vprint( buf0, buf_size, message, args );
					va_end( args );
					strcat( buf0, "\n" );

					debug_print( buf0 );
					
#ifdef HAL_BREAK_ON_ASSERT
					debug_break();
#endif
				}
			}

			profile_timer::profile_timer()
            : start( std::chrono::high_resolution_clock::now() )
			{
			}

			float profile_timer::elapsed_s()
			{
				return float( elapsed_ms() ) / 1000.f;
			}

			unsigned int profile_timer::elapsed_ms()
			{
				return ms_to_uint( std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - start ) );
			}

			void profile_timer::print_ms_to_debug( const char* message, ... )
			{
				char buf0[buf_size];
				va_list args;
				va_start( args, message );
                vprint( buf0, buf_size, message, args );
				va_end( args );

				char buf1[buf_size];
				sprintf( buf1, "%s: %i ms\n", buf0, elapsed_ms() );
				debug_print( buf1 );
			}
		}
	}
}

#ifdef WIN32
#pragma warning(default: 4996)
#endif
