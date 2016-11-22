
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

		void setOrbitTarget( const glm::vec3& pos, float min_dist, bool reset_distance );
		void setOrbitMinDistance( const float dist ) { orbit_min_dist = dist; }
		void setOrbitMaxDistance( const float dist ) { orbit_max_dist = dist; }

		void flyToTarget();
		void cancelFlyToTarget();
		bool flyingToTarget() { return flying_to_target; }

	protected:
		void doActivate() override;
		void doUpdate( float timeDelta ) override;

	private:
		render::perspective_camera* camera;
		glm::vec3 orbit_target;
		float orbit_min_dist, orbit_max_dist;
		float orbit_cur_dist;
		float move_speed, turn_speed;

		glm::vec3 flight_origin, flight_destination;
		bool begin_flying_to_target, flying_to_target;
		float flight_param;

		bool sliding_to_target;
		float slide_state;
		glm::vec3 camera_slide_origin, camera_slide_target;
	};
}
