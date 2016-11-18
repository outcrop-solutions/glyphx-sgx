
#include "bounds.h"
#include <algorithm>
#include <glm/gtx/norm.hpp>

namespace SynGlyphX
{
	namespace render
	{
		sphere_bound sphere_bound::from_points( glm::vec3* first, unsigned int stride_in_bytes, unsigned int count )
		{
			// A simple, quick two-pass algorithm. (It may be worth exploring more complex algorithms that produce
			// tighter-fitting bounds, e.g. http://www.idt.mdh.se/personal/tla/publ/FastBoundingSpheres_Sigrad2008.pdf.)

			char* byte_ptr = reinterpret_cast<char*>( first );

			// Find the center of the AABB containing the points and use that as the center of our sphere.
			glm::vec3 min( FLT_MAX, FLT_MAX, FLT_MAX );
			glm::vec3 max( FLT_MIN, FLT_MIN, FLT_MIN );
			for ( unsigned int i = 0; i < count; ++i )
			{
				const glm::vec3& pt = *( reinterpret_cast<glm::vec3*>( byte_ptr + ( stride_in_bytes * i ) ) );
				if ( pt.x < min.x ) min.x = pt.x;
				if ( pt.y < min.y ) min.y = pt.y;
				if ( pt.z < min.z ) min.z = pt.z;

				if ( pt.x > max.x ) max.x = pt.x;
				if ( pt.y > max.y ) max.y = pt.y;
				if ( pt.z > max.z ) max.z = pt.z;
			}
			glm::vec3 center( ( max.x + min.x ) * 0.5f, ( max.y + min.y ) * 0.5f, ( max.z + min.z ) * 0.5f );

			// The radius will be the distance from the center to the furthest point.
			float radius = 0.f;
			for ( unsigned int i = 0; i < count; ++i )
			{
				const glm::vec3& pt = *( reinterpret_cast<glm::vec3*>( byte_ptr + ( stride_in_bytes * i ) ) );
				float dist = glm::distance( pt, center );
				if ( dist > radius ) radius = dist;
			}

			return sphere_bound( center, radius );
		}

		// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting, 
		// returns t value of intersection and intersection point q 
		int sphere_bound::intersects_ray( const glm::vec3& p, const glm::vec3& d, float &t, glm::vec3& q ) const
		{
			glm::vec3 m = p - get_center();
			float b = glm::dot( m, d );
			float c = glm::dot( m, m ) - get_radius() * get_radius();

			// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
			if ( c > 0.f && b > 0.f ) return 0;
			float discr = b*b - c;

			// A negative discriminant corresponds to ray missing sphere 
			if ( discr < 0.0f ) return 0;

			// Ray now found to intersect sphere, compute smallest t value of intersection
			t = -b - sqrtf( discr );

			// If t is negative, ray started inside sphere so clamp t to zero 
			if ( t < 0.f ) t = 0.f;
			q = p + t * d;

			return 1;
		}

		sphere_bound transform_bound( const sphere_bound& b, const glm::mat4& matrix )
		{
			// Extract the scale from the matrix (largest scale if nonuniform, because we
			// want to keep the bound spherical).
			auto scale3 = glm::vec3(
				glm::length2( glm::vec3( matrix[0] ) ),
				glm::length2( glm::vec3( matrix[1] ) ),
				glm::length2( glm::vec3( matrix[2] ) ) );
			float scale = sqrtf( std::max( scale3.x, std::max( scale3.y, scale3.z ) ) );

			sphere_bound result = b;
			result.set_radius( b.get_radius() * scale );
			result.set_center( glm::vec3( matrix * glm::vec4( b.get_center(), 1.f ) ) );

			return result;
		}

		bool intersects( const sphere_bound& a, const sphere_bound& b )
		{
			return glm::distance( a.get_center(), b.get_center() ) < ( a.get_radius() + b.get_radius() );
		}

		sphere_bound combine_bounds( const sphere_bound& a, const sphere_bound& b )
		{
			auto ac = a.get_center(), bc = b.get_center();

			// Find the line between the two centers and use it to find the two most distant points between
			// the spheres. (Special case: if they have a common center any direction will work, so just pick
			// one arbitrarily.)
			auto a_to_b = ac != bc ? glm::normalize( bc - ac ) : glm::vec3( 1.f, 0.f, 0.f );

			auto b_extent_1 = bc + ( a_to_b * b.get_radius() );
			auto b_extent_2 = ac + ( a_to_b * a.get_radius() );
			auto b_extent = glm::distance( b_extent_1, ac ) > glm::distance( b_extent_2, ac ) ? b_extent_1 : b_extent_2;

			auto a_extent_1 = ac + ( -a_to_b * a.get_radius() );
			auto a_extent_2 = bc + ( -a_to_b * b.get_radius() );
			auto a_extent = glm::distance( a_extent_1, bc ) > glm::distance( a_extent_2, bc ) ? a_extent_1 : a_extent_2;

			// The new center will be halfway between the most distant points on the spheres, and the
			// new radius will be half the distance between them.
			auto new_radius = glm::length( b_extent - a_extent ) * 0.5f;
			auto new_center = ( a_extent + b_extent ) * 0.5f;

			return sphere_bound( new_center, new_radius );
		}

		namespace
		{
			float plane_distance( const glm::vec3& point, const glm::vec3& plane_pt, const glm::vec3& plane_nml )
			{
				return glm::dot( point - plane_pt, plane_nml );
			}

			bool sphere_inside_plane( const sphere_bound& sphere, const glm::vec3& plane_pt, const glm::vec3& plane_nml )
			{
				return -plane_distance( sphere.get_center(), plane_pt, plane_nml ) > sphere.get_radius();
			}
		}

		bool box_bound::contains( const sphere_bound& sphere )
		{
			if ( !sphere_inside_plane( sphere, min, glm::vec3( -1.f, 0.f, 0.f ) ) ) return false;
			if ( !sphere_inside_plane( sphere, min, glm::vec3( 0.f, -1.f, 0.f ) ) ) return false;
			if ( !sphere_inside_plane( sphere, min, glm::vec3( 0.f, 0.f, -1.f ) ) ) return false;

			if ( !sphere_inside_plane( sphere, max, glm::vec3( 1.f, 0.f, 0.f ) ) ) return false;
			if ( !sphere_inside_plane( sphere, max, glm::vec3( 0.f, 1.f, 0.f ) ) ) return false;
			if ( !sphere_inside_plane( sphere, max, glm::vec3( 0.f, 0.f, 1.f ) ) ) return false;

			return true;
		}

		void box_bound::dilate( const glm::vec3& distance )
		{
			min -= distance;
			max += distance;
		}

		bool box_bound::intersects_ray( const glm::vec3& ray_origin, const glm::vec3& ray_dir ) const
		{
			glm::vec3 n_inv = 1.f / ray_dir;

			float tx1 = ( min.x - ray_origin.x ) * n_inv.x;
			float tx2 = ( max.x - ray_origin.x ) * n_inv.x;

			float tmin = std::min( tx1, tx2 );
			float tmax = std::max( tx1, tx2 );

			float ty1 = ( min.y - ray_origin.y ) * n_inv.y;
			float ty2 = ( max.y - ray_origin.y ) * n_inv.y;

			tmin = std::max( tmin, std::min( ty1, ty2 ) );
			tmax = std::min( tmax, std::max( ty1, ty2 ) );

			float tz1 = ( min.z - ray_origin.z ) * n_inv.z;
			float tz2 = ( max.z - ray_origin.z ) * n_inv.z;

			tmin = std::max( tmin, std::min( tz1, tz2 ) );
			tmax = std::min( tmax, std::max( tz1, tz2 ) );

			return tmax >= tmin && tmax >= 0.f;
		}
		
		box_bound box_bound::from_points( glm::vec3* first, unsigned int stride_in_bytes, unsigned int count, const glm::mat4& base_transform )
		{
			char* byte_ptr = reinterpret_cast<char*>( first );

			// Find the center of the AABB containing the points and use that as the center of our sphere.
			glm::vec3 min( FLT_MAX, FLT_MAX, FLT_MAX );
			glm::vec3 max( FLT_MIN, FLT_MIN, FLT_MIN );
			for ( unsigned int i = 0; i < count; ++i )
			{
				const glm::vec3& pt = *( reinterpret_cast<glm::vec3*>( byte_ptr + ( stride_in_bytes * i ) ) );
				glm::vec4 pt4 = base_transform * glm::vec4( pt, 1.f );
				if ( pt4.x < min.x ) min.x = pt4.x;
				if ( pt4.y < min.y ) min.y = pt4.y;
				if ( pt4.z < min.z ) min.z = pt4.z;

				if ( pt4.x > max.x ) max.x = pt4.x;
				if ( pt4.y > max.y ) max.y = pt4.y;
				if ( pt4.z > max.z ) max.z = pt4.z;
			}

			return box_bound( min, max );
		}

		box_bound combine_bounds( const box_bound& b, const sphere_bound& s )
		{
			auto min = b.get_min();
			auto max = b.get_max();

			auto c = s.get_center();
			auto r = s.get_radius();

			if ( min.x > c.x - r ) min.x = c.x - r;
			if ( min.y > c.y - r ) min.y = c.y - r;
			if ( min.z > c.z - r ) min.z = c.z - r;

			if ( max.x < c.x + r ) max.x = c.x + r;
			if ( max.y < c.y + r ) max.y = c.y + r;
			if ( max.z < c.z + r ) max.z = c.z + r;

			return box_bound( min, max );
		}

		box_bound combine_bounds( const box_bound& a, const box_bound& b )
		{
			return box_bound( glm::min( a.get_min(), b.get_min() ), glm::max( a.get_max(), b.get_max() ) );
		}

		namespace
		{
			float dist_squared_to_box( const glm::vec3& p, const box_bound& b )
			{
				auto check = [&](
					const float pn,
					const float bmin,
					const float bmax )
				{
					float out = 0.f;
					float v = pn;

					if ( v < bmin )
					{
						float val = ( bmin - v );
						out += val * val;
					}

					if ( v > bmax )
					{
						float val = ( v - bmax );
						out += val * val;
					}

					return out;
				};

				auto min = b.get_min(), max = b.get_max();
				float sq = 0.f;
				sq += check( p.x, min.x, max.s );
				sq += check( p.y, min.y, max.y );
				sq += check( p.z, min.z, max.z );

				return sq;
			}
		}

		bool intersects( const sphere_bound& s, const box_bound& b )
		{
			float sq_dist = dist_squared_to_box( s.get_center(), b );
			return sq_dist < ( s.get_radius() * s.get_radius() );
		}
	}
}
