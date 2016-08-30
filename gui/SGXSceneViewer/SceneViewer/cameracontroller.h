
#pragma once

#include <glm/glm.hpp>
#include "smoother.h"

namespace SynGlyphX
{
	namespace render { class perspective_camera; }

	class CameraController
	{
	public:
		virtual ~CameraController() { };

		void update( float timeDelta )
		{
			pos_smoother.update( pos_motion, timeDelta );
			angle_smoother.update( angle_motion, timeDelta );
			doUpdate( timeDelta );
			angle_motion = glm::vec2();
			pos_motion = glm::vec3();
		}

		void activate()
		{
			pos_smoother.reset();
			angle_smoother.reset();
			doActivate();
		}

		virtual void turn( const glm::vec2& angleDelta ) = 0;
		virtual void move( const glm::vec3& relativeMotion ) = 0;

		virtual void setMovementSpeed( float speed ) = 0;
		virtual void setTurnSpeed( float speed ) = 0;

	protected:
		virtual void doUpdate( float timeDelta ) = 0;
		virtual void doActivate() = 0;

		void updatePosMotion( const glm::vec3& motion ) { pos_motion += motion; }
		void updateAngleMotion( const glm::vec2& motion ) { angle_motion += motion; }

		glm::vec3 posMotion() { return pos_smoother.smoothed_value(); }
		glm::vec2 angleMotion() { return angle_smoother.smoothed_value(); }

	private:
		glm::vec3 pos_motion;
		glm::vec2 angle_motion;
		Smoother<glm::vec3> pos_smoother;
		Smoother<glm::vec2> angle_smoother;
	};
}
