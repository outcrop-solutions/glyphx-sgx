
#pragma once

#include "../platform.h"
#include "cameracontroller.h"

namespace SynGlyphX
{
	namespace render { class perspective_camera; }

	class OrbitCameraController : public CameraController
	{
	public:
		OrbitCameraController( render::perspective_camera* camera );
		OrbitCameraController( const OrbitCameraController& ) = delete;

		void turn( const glm::vec2& angleDelta ) override;
		void move( const glm::vec3& relativeMotion ) override;

		void setMovementSpeed( float speed ) override { move_speed = speed; }
		void setTurnSpeed( float speed ) override { turn_speed = speed; }

		void setOrbitTarget( const glm::vec3& pos ) { if ( orbit_target != pos ) { orbit_target = pos; turning_to_target = true; } }
		void setOrbitMinDistance( const float dist ) { orbit_min_dist = dist; }
		void setOrbitMaxDistance( const float dist ) { orbit_max_dist = dist; }

		void flyToTarget() { flying_to_target = true; }
		void cancelFlyToTarget() { flying_to_target = false; }

	protected:
		void doActivate() override;
		void doUpdate( float timeDelta ) override;

	private:
		render::perspective_camera* camera;
		glm::vec3 orbit_target;
		float orbit_min_dist, orbit_max_dist;
		float orbit_cur_dist;
		bool flying_to_target, turning_to_target;
		float move_speed, turn_speed;
	};
}
