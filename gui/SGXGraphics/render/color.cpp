
#include "color.h"

namespace SynGlyphX
{
	namespace render
	{
		packed_color pack_color( const glm::vec4& color )
		{
			uint8_t r = uint8_t( color.r * 255.f );
			uint8_t g = uint8_t( color.g * 255.f );
			uint8_t b = uint8_t( color.b * 255.f );
			uint8_t a = uint8_t( color.a * 255.f );
			return a | ( b << 8 ) | ( g << 16 ) | ( r << 24 );
		}

		glm::vec4 unpack_color( packed_color color )
		{
			glm::vec4 c;
			uint8_t* pc8 = reinterpret_cast<uint8_t*>( &color );
			c.a = pc8[0] / 255.f;
			c.b = pc8[1] / 255.f;
			c.g = pc8[2] / 255.f;
			c.r = pc8[3] / 255.f;
			return c;
		}

		namespace color
		{
			glm::vec4 red() { return glm::vec4( 1.f, 0.f, 0.f, 1.f ); }
			glm::vec4 green() { return glm::vec4( 0.f, 1.f, 0.f, 1.f ); }
			glm::vec4 blue() { return glm::vec4( 0.f, 0.f, 1.f, 1.f ); }

			glm::vec4 cyan() { return glm::vec4( 0.f, 1.f, 1.f, 1.f ); }
			glm::vec4 magenta() { return glm::vec4( 1.f, 0.f, 1.f, 1.f ); }
			glm::vec4 yellow() { return glm::vec4( 1.f, 1.f, 0.f, 1.f ); }

			glm::vec4 black() { return glm::vec4( 0.f, 0.f, 0.f, 1.f ); }
			glm::vec4 white() { return glm::vec4( 1.f, 1.f, 1.f, 1.f ); }
			glm::vec4 clear_black ( ) { return glm::vec4( 0.f, 0.f, 0.f, 0.f ); }
		}
	}
}
