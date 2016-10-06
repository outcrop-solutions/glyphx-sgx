
#include "log.h"
#include <queue>
#include <cstdarg>

namespace SynGlyphX
{
	namespace hal
	{
		namespace
		{
			std::queue< message > errors;
		}

		void log::error( const char* err, const char* detail, ... )
		{
			char buf[2048];
			va_list args;
			va_start( args, detail );
			vsprintf_s( buf, detail, args );
			va_end( args );

			errors.push( std::make_pair( err, buf ) );
		}

		bool log::has_errors()
		{
			return errors.size() > 0;
		}

		message log::next_error()
		{
			auto& err = errors.front();
			auto result = message( err.first, err.second );
			errors.pop();
			return result;
		}
	}
}
