
#include "overheadcameracontroller.h"
#include <render/perspective_camera.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/compatibility.hpp>

namespace SynGlyphX
{
	OverheadCameraController::OverheadCameraController( render::perspective_camera* _camera )
		: camera( _camera ), turn_speed( 32.f ), move_speed( 32.f ), state( states::animating_in )
	{
	}

	void OverheadCameraController::doActivate()
	{
		original_heading = camera->get_forward();
		original_world_up = camera->get_world_up();
		state = states::animating_in;
		animation_alpha = 0.f;
	}

	void OverheadCameraController::doUpdate( float timeDelta )
	{
		if ( state == states::animating_in )
		{
			const glm::vec3 down_heading( 0.f, 0.f, -1.f );
			auto new_heading = glm::normalize( glm::lerp( original_heading, down_heading, animation_alpha ) );
			camera->set_forward( new_heading );

			const glm::vec3 new_world_up( 0.f, 1.f, 0.f );
			auto new_up = glm::normalize( glm::lerp( original_world_up, new_world_up, animation_alpha ) );
			camera->set_world_up( new_up );
		}

		const float animation_rate = 0.005f;
		animation_alpha += timeDelta * animation_rate;
		animation_alpha = std::min( animation_alpha, 1.f );
		if ( animation_alpha == 1.f )
			state = states::normal;
	}
}
