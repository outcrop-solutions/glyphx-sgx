
#include "gl.h"
#include "../log.h"
#include "../debug.h"

namespace SynGlyphX
{
	namespace hal
	{
		namespace
		{
			bool break_on_error = false;
		}

		void check_errors( bool ignore )
		{
			UNREFERENCED_PARAMETER( ignore );
#ifdef _DEBUG
			// OpenGL errors.
			while ( auto error = glGetError() )
			{
				if ( !ignore )
				{
					const char* errstr = reinterpret_cast<const char*>( gluErrorString( error ) );
					hal::log::error( "OpenGL Error: ", errstr );
					if ( break_on_error )
					{
						debug::print( "OpenGL Error: " );
						debug::print( errstr );
						debug::print( "\n" );
						debug::debug_break();
					}
				}
			}
#endif // DEBUG
		}

		void set_break_on_error( bool val )
		{
			break_on_error = val;
		}
	}
}
