
#pragma once

#include "../platform.h"
#include "cameracontroller.h"

namespace SynGlyphX
{
	namespace render { class perspective_camera; }

	class OverheadCameraController : public CameraController
	{
	public:
		OverheadCameraController( render::perspective_camera* camera );
		OverheadCameraController( const OverheadCameraController& ) = delete;

		void turn( const glm::vec2& angleDelta ) override {}
		void move( const glm::vec3& relativeMotion ) override {}

		void setMovementSpeed( float speed ) override { move_speed = speed; }
		void setTurnSpeed( float speed ) override { turn_speed = speed; }

	protected:
		void doActivate() override;
		void doUpdate( float timeDelta ) override;

	private:
		render::perspective_camera* camera;
		float move_speed, turn_speed;
		enum class states {
			animating_in,
			normal,
		} state;
		float animation_alpha;
		glm::vec3 original_heading, original_world_up;
	};
}
