
#pragma once

#include <vector>
#include "../platform.h"
#include <hal/types.h>
#include "bounds.h"

namespace SynGlyphX
{
	namespace render
	{
		class SGXGRAPHICS_API model_part
		{
		public:
			model_part( hal::mesh* mesh, const glm::mat4& transform, const sphere_bound& bound, const char* name = "" );
			model_part( hal::mesh* mesh, const glm::mat4& transform, const char* name = "" );
			model_part( const model_part& ) = delete;
			~model_part();

			const glm::mat4& get_transform() const { return transform; }
			void set_transform( const glm::mat4& new_transform ) { transform = new_transform; }

			sphere_bound get_local_bound() const { return transform_bound( bound, transform ); }

			hal::mesh* get_mesh() { return mesh; }

		private:
			hal::mesh* mesh;
			glm::mat4 transform;
			sphere_bound bound;
			std::string name;
		};

		class SGXGRAPHICS_API model
		{
		public:
			model() : needs_bound_recompute( true ) { }
			model( const model& ) = delete;
			~model();

			void add_part( model_part* part );
			const std::vector<model_part*>& get_parts() const { return parts; }

			const glm::mat4& get_transform() const { return transform; }
			void set_transform( const glm::mat4& new_transform ) { transform = new_transform; }

			const char* get_name() { return name.c_str(); }
			void set_name( const char* _name ) { name = _name; }

			const sphere_bound& get_bound() const;

			model_part* pick( const glm::vec3& ray_origin, const glm::vec3& ray_dir, const glm::mat4& world, glm::vec3& intersection_pt ) const;

		private:
			std::string name;
			std::vector<model_part*> parts;
			glm::mat4 transform;

			// bound caching; mutable because this doesn't affect logical const-ness
			mutable sphere_bound bound;
			mutable bool needs_bound_recompute;
		};
	}
}

