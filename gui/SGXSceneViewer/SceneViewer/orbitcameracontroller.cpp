
#include "orbitcameracontroller.h"
#include <render/perspective_camera.h>
#include <glm/gtx/euler_angles.hpp>

namespace SynGlyphX
{
	OrbitCameraController::OrbitCameraController( render::perspective_camera* _camera )
		: camera( _camera ), turn_speed( 32.f ), move_speed( 32.f ), flying_to_target( false ), turning_to_target( false ), orbit_max_dist( 1000.f )
	{
	}

	void OrbitCameraController::doUpdate( float timeDelta )
	{
		auto pos_motion = posMotion() * move_speed;
		auto angle_motion = angleMotion();
		glm::vec2 cur_angle_motion = angle_motion * turn_speed;

		glm::vec3 cam_pos = camera->get_position();
		glm::vec3 target_to_camera = cam_pos - orbit_target;
		orbit_cur_dist = glm::length( target_to_camera );

		// Zoom logic from ANTz for consistency...
		float orbit_new_dist = orbit_cur_dist * ( 1.0f + pos_motion.z * 0.005f );

		// ...but clamped to reasonable limits.
		orbit_new_dist = std::max( orbit_new_dist, orbit_min_dist );
		orbit_new_dist = std::min( orbit_new_dist, orbit_max_dist );

		glm::vec3 desired_fwd = glm::normalize( orbit_target - cam_pos );
		auto fwd = camera->get_forward();
		auto interp = glm::normalize( glm::lerp( fwd, desired_fwd, 0.25f ) );
		camera->set_forward( interp );

		if ( turning_to_target && acosf( glm::dot( fwd, desired_fwd ) ) < 0.001f )
			turning_to_target = false;
		if ( glm::length( angle_motion ) > 1.f )
			turning_to_target = false;

		if ( flying_to_target )
		{
			float fly_target_dist = orbit_min_dist * 10.f;
			glm::vec3 camera_to_object = -glm::normalize( cam_pos - orbit_target );
			float dist_to_travel = orbit_cur_dist - fly_target_dist;
			if ( dist_to_travel > 0.f )	// don't go backward
				camera->set_position( camera->get_position() + camera_to_object * ( dist_to_travel * 0.05f ) );
			if ( glm::length( camera->get_position() - orbit_target ) < ( fly_target_dist + 0.1f ) )
				cancelFlyToTarget();
		}
		else if ( !turning_to_target )
		{
			// Slightly clunky way to restrict pitching the camera into a singularity. If the current vertical angle delta will
			// put us close to 90 degrees, clamp it to just under.
			glm::vec3 unit_target_to_camera = glm::normalize( target_to_camera );
			glm::vec3 fwd_proj_xy = glm::normalize( glm::vec3( fwd.x, fwd.y, 0.f ) );
			float pitch = acosf( dot( unit_target_to_camera, -fwd_proj_xy ) );
			if ( unit_target_to_camera.z < 0.f ) pitch = -pitch;
			const float pitch_limit = 1.5f;
			if ( pitch + cur_angle_motion.y > pitch_limit ) cur_angle_motion.y = std::min( 0.f, pitch + cur_angle_motion.y - pitch_limit );
			if ( pitch + cur_angle_motion.y < -pitch_limit ) cur_angle_motion.y = std::max( 0.f, pitch + cur_angle_motion.y + pitch_limit );

			glm::mat4 rot;
			rot = glm::rotate( rot, -cur_angle_motion.x, glm::vec3( 0.f, 0.f, 1.f ) );
			rot = glm::rotate( rot, -cur_angle_motion.y, camera->get_right() );

			glm::vec3 new_unit_target_to_camera = glm::vec3( rot * glm::normalize( glm::vec4( target_to_camera, 0.f ) ) );
			camera->set_position( orbit_target + new_unit_target_to_camera * orbit_new_dist );
			camera->set_forward( -new_unit_target_to_camera );
		}
	}

	void OrbitCameraController::turn( const glm::vec2& angleDelta )
	{
		if ( glm::length( angleDelta ) > 0.01f )
			cancelFlyToTarget();
		const float rotate_rate = 0.005f;
		updateAngleMotion( angleDelta * rotate_rate );
	}

	void OrbitCameraController::move( const glm::vec3& relativeMotion )
	{
		if ( glm::length( relativeMotion ) > 0.01f )
			cancelFlyToTarget();
		updatePosMotion( relativeMotion );
	}

	void OrbitCameraController::doActivate()
	{
	}
}
