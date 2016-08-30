
#include "model.h"
#include <hal/hal.h>
#include <hal/readback_utils.h>
#include <glm/gtx/intersect.hpp>

namespace SynGlyphX
{
	namespace render
	{
		model_part::model_part( hal::mesh* _mesh, const glm::mat4& _transform, const sphere_bound& _bound, const char* _name )
			: mesh( _mesh ), transform( _transform ), bound( _bound ), name( _name )
		{
			hal::device::addref( mesh );
		}

		model_part::model_part( hal::mesh* _mesh, const glm::mat4& _transform, const char* _name )
			: mesh( _mesh ), transform( _transform ), name( _name )
		{
			hal::device::addref( mesh );
		}
		model_part::~model_part()
		{
			hal::device::release( mesh );
		}

		model::~model()
		{
			for ( auto* part : parts )
				delete part;
		}

		void model::add_part( model_part* p )
		{
			parts.push_back( p );
		}

		const sphere_bound& model::get_bound() const
		{
			if ( needs_bound_recompute )
			{
				if ( parts.size() == 0 ) bound = sphere_bound();
				sphere_bound cumulative = parts[0]->get_local_bound();
				for ( unsigned int i = 1; i < parts.size(); ++i )
					cumulative = combine_bounds( cumulative, parts[1]->get_local_bound() );
				bound = transform_bound( cumulative, transform );
				needs_bound_recompute = false;
			}
			return bound;
		}

		model_part* model::pick( const glm::vec3& ray_origin, const glm::vec3& ray_dir, const glm::mat4& world, glm::vec3& intersection_pt ) const
		{
			model_part* best_part = nullptr;
			float best_dist = FLT_MAX;

			// TODO: Should check sphere bounds (no need to assume caller did it).

			for ( auto* part : parts )
			{
				auto world_transform = world * transform * part->get_transform();

				// TODO: should NOT use default context here (breaks if we ever have multiple contexts)
				auto mesh_data = hal::device::get_default_context()->readback_mesh( part->get_mesh() );

				hal::for_each_triangle( mesh_data, [&]( const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2 ) {
					auto wv0 = glm::vec3( world_transform * glm::vec4( v0, 1.f ) );
					auto wv1 = glm::vec3( world_transform * glm::vec4( v1, 1.f ) );
					auto wv2 = glm::vec3( world_transform * glm::vec4( v2, 1.f ) );
					glm::vec3 barypos;
					if ( glm::intersectRayTriangle( ray_origin, ray_dir, wv0, wv1, wv2, barypos ) )
					{
						auto isect = ray_origin + ray_dir * barypos.z;
						if ( barypos.z < best_dist )
						{
							best_dist = barypos.z;
							best_part = part;
							intersection_pt = isect;
						}
					}
				} );
			}

			return best_part;
		}
	}
}
