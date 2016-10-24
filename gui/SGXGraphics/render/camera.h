
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../platform.h"

namespace SynGlyphX
{
	namespace render
	{
		class SGXGRAPHICS_API camera
		{
		public:
            virtual ~camera() { }
            
			virtual const glm::mat4& get_view() const = 0;
			virtual const glm::mat4& get_proj() const = 0;

			virtual const glm::vec3& get_position() const = 0;

			virtual glm::vec2 world_pt_to_window_pt( const glm::vec3& world_pt ) const = 0;
			virtual bool pt_behind_camera( const glm::vec3& world_pt ) const = 0;
		};
	}
}

