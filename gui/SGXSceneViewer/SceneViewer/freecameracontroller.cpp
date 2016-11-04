
#include "freecameracontroller.h"
#include <render/perspective_camera.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SynGlyphX
{
	FreeCameraController::FreeCameraController( render::perspective_camera* _camera )
		: camera( _camera ), move_speed( 32.f ), turn_speed( 32.f )
	{
	}

	void FreeCameraController::doUpdate( float /* timeDelta */ )
	{
		// todo: this should use timeDelta

		auto pos_motion = posMotion();
		auto angle_motion = angleMotion();

		cam_angles += angle_motion * turn_speed;

		// Clamp angle to prevent flipping the camera.
		cam_angles.y = std::min( cam_angles.y, 1.56f );
		cam_angles.y = std::max( cam_angles.y, -1.56f );

		// Move into range (0, 2pi)
		while ( cam_angles.x > glm::two_pi<float>() ) cam_angles.x -= glm::two_pi<float>();
		while ( cam_angles.x < 0.f ) cam_angles.x += glm::two_pi<float>();

		// Apply new orientation to camera.
		glm::mat4 rot;
		rot = glm::rotate( rot, -cam_angles.x, glm::vec3( 0.f, 0.f, 1.f ) );
		rot = glm::rotate( rot, -cam_angles.y, glm::vec3( 1.f, 0.f, 0.f ) );
		glm::vec4 new_fwd = rot * glm::vec4( 0.f, 1.f, 0.f, 0.f );
		camera->set_forward( glm::vec3( new_fwd ) );

		// Apply linear motion to camera.
		camera->set_position( camera->get_position() + pos_motion * move_speed );
	}

	void FreeCameraController::turn( const glm::vec2& angleDelta )
	{
		const float rotate_rate = 0.005f;
		updateAngleMotion( angleDelta * rotate_rate );
	}

	void FreeCameraController::move( const glm::vec3& relativeMotion )
	{
		updatePosMotion( relativeMotion );
	}

	void FreeCameraController::doActivate()
	{
		auto& fwd = camera->get_forward();
		float ax = atan2f( fwd.x, fwd.y );
		glm::vec3 proj( fwd.x, fwd.y, 0.f );
		float ay = acosf( glm::dot( glm::normalize( proj ), camera->get_forward() ) );
		if ( camera->get_forward().z > 0.f ) ay = -ay;
		cam_angles = glm::vec2( ax, ay );
	}
}
