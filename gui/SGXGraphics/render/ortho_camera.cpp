
#include "ortho_camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace SynGlyphX
{
	namespace render
	{
		namespace
		{
			glm::vec3 eye( 0.f, 0.f, 1.f );
		}

		ortho_camera::ortho_camera( unsigned int _viewport_w, unsigned int _viewport_h, float _near_clip, float _far_clip )
			: viewport_w( _viewport_w ), viewport_h( _viewport_h ), near_clip( _near_clip ), far_clip( _far_clip )
		{
		}

		void ortho_camera::recalculate() const
		{
			if ( dirty )
			{
				view = glm::mat4();
				proj = glm::ortho( 0.f, float( viewport_w ), float( viewport_h ), 0.f, near_clip, far_clip );
				dirty = false;
			}
		}

		const glm::vec3& ortho_camera::get_eye() const
		{
			return eye;
		}
	}
}


