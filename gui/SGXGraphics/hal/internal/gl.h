
#pragma once

#include <gl/glew.h>
#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <gl/GL.h>
#elif defined(__APPLE__)
#include <OpenGL/GL.h>
#endif

namespace SynGlyphX
{
	namespace hal
	{
		void set_break_on_error( bool val );

		// Check for OpenGL errors. Should be called at the end of every function that calls OpenGL API functions.
		// On release builds this does nothing.
		// Pass ignore=true to work around known GL errors in libraries etc (e.g., glewInit causing a bad enum error).
		void check_errors( bool ignore = false );
	}
}
