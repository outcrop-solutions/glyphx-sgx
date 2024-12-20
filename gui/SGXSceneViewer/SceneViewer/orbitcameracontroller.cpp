
#include "orbitcameracontroller.h"
#include <render/perspective_camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/spline.hpp>
#include <hal/debug.h>

namespace SynGlyphX
{
	namespace
	{
		const float fly_target_dist_mult = 3.5f;

		float smootherstep( float edge0, float edge1, float x )
		{
			// Scale, and clamp x to 0..1 range
			x = glm::clamp( ( x - edge0 ) / ( edge1 - edge0 ), 0.f, 1.f );
			// Evaluate polynomial
			return x*x*x*( x*( x * 6.f - 15.f ) + 10.f );
		}
	}

	OrbitCameraController::OrbitCameraController( render::perspective_camera* _camera )
		: camera( _camera ), orbit_max_dist( 1000.f ), move_speed( 32.f ),
		turn_speed( 32.f ), begin_flying_to_target( false ), flying_to_target( false ), sliding_to_target( false )
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

		if ( flying_to_target )
		{
			glm::vec3 desired_fwd = glm::normalize( orbit_target - cam_pos );
			auto fwd = camera->get_forward();
			auto interp = glm::normalize( glm::lerp( fwd, desired_fwd, 0.075f ) );
			camera->set_forward( interp );

			if ( flight_param <= 1.f )
			{
				float eased_flight_param = smootherstep( 0.f, 1.f, flight_param );
				camera->set_position( glm::lerp( flight_origin, flight_destination, eased_flight_param ) );
				const float fly_time = 1200.f;	// in ms
				flight_param += ( 1.f / fly_time ) * timeDelta;
			}

			float angle_to_target = acosf( glm::dot( desired_fwd, fwd ) );

			if ( flight_param >= 1.f && angle_to_target < 0.0001f )
				cancelFlyToTarget();
		}
		else if ( sliding_to_target )
		{
			glm::vec3 new_cam_pos = glm::lerp( camera_slide_origin, camera_slide_target, glm::smoothstep( 0.f, 1.f, slide_state ) );
			const float slide_speed = 0.005f;
			slide_state = slide_state + timeDelta * slide_speed;
			if ( slide_state >= 1.f )
				sliding_to_target = false;
			camera->set_position( new_cam_pos );
		}
		else
		{
			// Slightly clunky way to restrict pitching the camera into a singularity. If the current vertical angle delta will
			// put us close to 90 degrees, clamp it to just under.
			auto unit_target_to_camera = glm::normalize( target_to_camera );
			auto fwd = camera->get_forward();
			auto fwd_proj_xy = glm::normalize( glm::vec3( fwd.x, fwd.y, 0.f ) );
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

	void OrbitCameraController::flyToTarget()
	{
		begin_flying_to_target = true;
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

	void OrbitCameraController::setOrbitTarget( const glm::vec3& pos, float min_dist, bool reset_distance )
	{
		setOrbitMinDistance( min_dist );
		if ( orbit_target != pos || reset_distance )
		{
			auto old_orbit_target = orbit_target;
			camera_slide_origin = camera->get_position();
			orbit_target = pos;
			sliding_to_target = true;
			slide_state = 0.f;

			// maintain camera orientation and try to maintain distance
			float target_dist;
			if ( reset_distance )
				target_dist = glm::distance( camera->get_position(), pos );
			else
				target_dist = std::max( glm::distance( camera->get_position(), old_orbit_target ), orbit_min_dist );
			camera_slide_target = orbit_target + -camera->get_forward() * target_dist;
		}

		if ( begin_flying_to_target )
		{
			flying_to_target = true;
			flight_origin = camera->get_position();
			flight_destination = orbit_target + glm::normalize( flight_origin - orbit_target ) * ( orbit_min_dist * fly_target_dist_mult );
			begin_flying_to_target = false;
			flight_param = 0.f;
		}
	}

	void OrbitCameraController::cancelFlyToTarget()
	{
		begin_flying_to_target = false;
		flying_to_target = false;
		sliding_to_target = false;
	}
}
