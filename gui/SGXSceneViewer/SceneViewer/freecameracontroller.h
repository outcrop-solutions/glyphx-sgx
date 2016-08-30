
#pragma once

#include "../platform.h"
#include "cameracontroller.h"

namespace SynGlyphX
{
	namespace render { class perspective_camera; }

	class FreeCameraController : public CameraController
	{
	public:
		FreeCameraController( render::perspective_camera* camera );
		FreeCameraController( const FreeCameraController& ) = delete;

		void turn( const glm::vec2& angleDelta ) override;
		void move( const glm::vec3& relativeMotion ) override;

		void setMovementSpeed( float speed ) override { move_speed = speed; }
		void setTurnSpeed( float speed ) override { turn_speed = speed; }

		void doActivate() override;

	protected:
		void doUpdate( float timeDelta ) override;

	private:
		render::perspective_camera* camera;
		glm::vec2 cam_angles;
		float move_speed, turn_speed;
	};
}
