
#pragma once

#include <glm/glm.hpp>
#include "../platform.h"
#include "camera.h"

namespace SynGlyphX
{
	namespace render
	{
		class SGXGRAPHICS_API ortho_camera : public camera
		{
		public:
			ortho_camera( unsigned int _viewport_w = 512u, unsigned int _viewport_h = 512u, float near_clip = -1.f, float far_clip = 1.f );
			void update_viewport_size( unsigned int w, unsigned int h ) { viewport_w = w; viewport_h = h; dirty = true; }

			void set_clip_planes( float _near, float _far ) { near_clip = _near; far_clip = _far; dirty = true; }
			const glm::vec3& get_position() const override { return pos; }

			const glm::mat4& get_view() const override { recalculate(); return view; }
			const glm::mat4& get_proj() const override { recalculate(); return proj; }

			virtual bool pt_behind_camera( const glm::vec3& world_pt ) const override { return world_pt.z < near_clip; }
			glm::vec2 world_pt_to_window_pt( const glm::vec3& world_pt ) const override { return glm::vec2( world_pt.x, float( viewport_h ) - world_pt.y ); }

		private:
			void recalculate() const;
			unsigned int viewport_w, viewport_h;
			float near_clip, far_clip;
			glm::vec3 pos;

			// Cached view/proj matrices; mutable because they don't affect the logical const-ness of the camera.
			mutable glm::mat4 view, proj;
			mutable bool dirty;
		};
	}
}
