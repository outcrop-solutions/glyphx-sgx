
#include "debug.h"

namespace SynGlyphX
{
	namespace hal
	{
		namespace debug
		{
			void print_val( const glm::mat4& mat )
			{
				for ( int i = 0; i < 4; ++i )
					print( "%8.4f  %8.4f  %8.4f  %8.4f", mat[i][0], mat[i][1], mat[i][2], mat[i][3] );
			}

			void print_val( const glm::vec2& vec )
			{
				print( "%8.4f  %8.4f", vec.x, vec.y );
			}

			void print_val( const glm::vec3& vec )
			{
				print( "%8.4f  %8.4f  %8.4f", vec.x, vec.y, vec.z );
			}

			void print_val( const glm::vec4& vec )
			{
				print( "%8.4f  %8.4f  %8.4f  %8.4f", vec.x, vec.y, vec.z, vec.w );
			}
		}
	}
}