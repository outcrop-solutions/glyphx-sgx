
#pragma once

#include <glm/glm.hpp>
#include "../platform.h"

namespace SynGlyphX
{
	namespace render
	{
		class SGXGRAPHICS_API sphere_bound
		{
		public:
			sphere_bound() : radius( 0.f ) { }
			sphere_bound( const glm::vec3& _center, float _radius ) : center( _center ), radius( _radius ) { }
			static sphere_bound from_points( glm::vec3* first, unsigned int stride_in_bytes, unsigned int count );

			const glm::vec3& get_center() const { return center; }
			void set_center( const glm::vec3& _center ) { center = _center; }

			float get_radius() const { return radius; }
			void set_radius( float _radius ) { radius = _radius; }

			int intersects_ray( const glm::vec3& p, const glm::vec3& d, float &t, glm::vec3& q ) const;
		private:
			glm::vec3 center;
			float radius;
		};

		SGXGRAPHICS_API bool intersects( const sphere_bound& a, const sphere_bound& b );
		SGXGRAPHICS_API sphere_bound transform_bound( const sphere_bound& b, const glm::mat4& matrix );
		SGXGRAPHICS_API sphere_bound combine_bounds( const sphere_bound& a, const sphere_bound& b );

		class SGXGRAPHICS_API box_bound
		{
		public:
			box_bound() { }
			box_bound( const glm::vec3& _min, const glm::vec3& _max ) : min( _min ), max( _max ) { }
			static box_bound from_points( glm::vec3* first, unsigned int stride_in_bytes, unsigned int count, const glm::mat4& base_transform = glm::mat4() );

			const glm::vec3& get_min() const { return min; }
			const glm::vec3& get_max() const { return max; }

			glm::vec3 get_center() const { return min + ( max - min ) * 0.5f; }
			glm::vec3 get_size() const { return max - min; }

			bool contains( const sphere_bound& sphere );
			bool contains( const glm::vec3& pt );

			void dilate( const glm::vec3& distance );

			bool intersects_ray( const glm::vec3& ray_origin, const glm::vec3& ray_dir ) const;

		private:
			glm::vec3 min, max;
		};

		SGXGRAPHICS_API box_bound combine_bounds( const box_bound& b, const sphere_bound& s );
		SGXGRAPHICS_API box_bound combine_bounds( const box_bound& a, const box_bound& b );
		SGXGRAPHICS_API bool intersects( const sphere_bound& s, const box_bound& b );
	}
}
