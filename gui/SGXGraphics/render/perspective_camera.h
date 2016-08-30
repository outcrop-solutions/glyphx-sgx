
#pragma once

#include <glm/glm.hpp>
#include "../platform.h"
#include "camera.h"

namespace SynGlyphX
{
	namespace render
	{
		class SGXGRAPHICS_API perspective_camera : public camera
		{
		public:
			perspective_camera( unsigned int _viewport_w = 512u, unsigned int _viewport_h = 512u );
			void update_viewport_size( unsigned int w, unsigned int h ) { viewport_w = w; viewport_h = h; }

			const glm::vec3& get_position() const { return pos; }
			void set_position( const glm::vec3& _pos ) { pos = _pos; dirty = true; }

			const glm::vec3& get_world_up() const { return world_up; }
			void set_world_up( const glm::vec3& _up ) { world_up = glm::normalize( _up ); dirty = true; }

			const glm::vec3& get_forward() const { return fwd; }
			void set_forward( const glm::vec3& _fwd ) { fwd = glm::normalize( _fwd ); dirty = true; }

			glm::vec3 get_right() const { return glm::normalize( glm::cross( fwd, world_up ) ); }
			glm::vec3 get_up() const { return glm::normalize( glm::cross( glm::cross( fwd, world_up ), fwd ) ); }

			void set_clip_planes( float _near, float _far ) { near_clip = _near; far_clip = _far; dirty = true; }
			void set_fov( float _fov ) { fov = _fov; dirty = true; }

			float get_far_clip() const { return far_clip; }
			float get_near_clip() const { return near_clip; }

			const glm::mat4& get_view() const override { recalculate(); return view; }
			const glm::mat4& get_proj() const override { recalculate(); return proj; }
			const glm::vec3& get_eye() const override { return fwd; }

			unsigned int get_viewport_w() { return viewport_w; }
			unsigned int get_viewport_h() { return viewport_h; }

			glm::vec3 unproject( unsigned int x, unsigned int y, float distance );
			void viewport_pt_to_ray( unsigned int x, unsigned int y, glm::vec3& ray_origin_out, glm::vec3& ray_dir_out );
			virtual bool pt_behind_camera( const glm::vec3& world_pt ) const override;
			glm::vec2 world_pt_to_window_pt( const glm::vec3& world_pt ) const override;

		private:
			void recalculate() const;
			glm::vec3 pos, fwd, world_up;
			unsigned int viewport_w, viewport_h;
			float near_clip, far_clip, fov;

			// Cached view/proj matrices; mutable because they don't affect the logical const-ness of the camera.
			mutable glm::mat4 view, proj;
			mutable bool dirty;
		};
	}
}


