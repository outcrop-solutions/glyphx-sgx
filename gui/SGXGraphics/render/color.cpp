
#include "color.h"

namespace SynGlyphX
{
	namespace render
	{
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
