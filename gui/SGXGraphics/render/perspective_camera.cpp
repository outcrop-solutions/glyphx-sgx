
#include "perspective_camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace SynGlyphX
{
	namespace render
	{
		perspective_camera::perspective_camera( unsigned int _viewport_w, unsigned int _viewport_h )
			: fwd( glm::vec3( 0.f, 1.f, 0.f ) ), world_up( glm::vec3( 0.f, 0.f, 1.f ) ), viewport_w( _viewport_w ), viewport_h( _viewport_h ),
			near_clip( 0.1f ), far_clip( 1000.f ), fov( glm::pi<float>() * 0.15f )
		{
		}

		void perspective_camera::recalculate() const
		{
			if ( dirty )
			{
				view = glm::lookAt( pos, pos + fwd, world_up );
				proj = glm::perspectiveFov<float>( fov, float( viewport_w ), float( viewport_h ), near_clip, far_clip );
				dirty = false;
			}
		}

		glm::vec3 perspective_camera::unproject( unsigned int x, unsigned int y, float d )
		{
			recalculate();
			glm::mat4 unproject = glm::inverse( proj * view );
			glm::vec4 normalized_pt( ( float( x ) / float( viewport_w - 1u ) ) * 2.f - 1.f,
				( 1.f - ( float( y ) / float( viewport_h - 1u ) ) ) * 2.f - 1.f,
				d * 2.f - 1.f,
				1.f );
			glm::vec4 result = unproject * normalized_pt;
			result.x /= result.w;
			result.y /= result.w;
			result.z /= result.w;
			result.w = 1.f;
			return glm::vec3( result );
		}

		void perspective_camera::viewport_pt_to_ray( unsigned int x, unsigned int y, glm::vec3& ray_origin_out, glm::vec3& ray_dir_out )
		{
			ray_origin_out = unproject( x, y, 0.f );
			auto distant = unproject( x, y, 1.f );
			ray_dir_out = glm::normalize( distant - ray_origin_out );
		}

		glm::vec2 perspective_camera::world_pt_to_window_pt( const glm::vec3& world_pt ) const
		{
			glm::vec4 pt( world_pt, 1.f );
			glm::vec4 result = ( proj * view ) * pt;
			result.x /= result.w;
			result.y /= result.w;

			result.x += 1.f;
			result.x *= 0.5f;
			result.x *= float( viewport_w );

			result.y += 1.f;
			result.y *= 0.5f;
			result.y *= float( viewport_h );

			return glm::vec2( result.x, viewport_h - result.y );
		}

		bool perspective_camera::pt_behind_camera( const glm::vec3& world_pt ) const
		{
			return glm::dot( get_eye(), get_position() - world_pt ) > 0.f;
		}
	}
}
