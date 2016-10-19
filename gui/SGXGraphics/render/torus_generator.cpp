
#include "torus_generator.h"
#include <hal/hal.h>
#include <hal/debug.h>
#include <glm/gtc/constants.hpp>
#include "model.h"

namespace SynGlyphX
{
	namespace render
	{
		render::model* generate_torus( float radius, float ring_radius, unsigned int segments, unsigned int rings, bool support_readback )
		{
			hal::debug::profile_timer timer;

			hal::vertex_format fmt;
			fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3u, hal::stream_semantic::position, 0 ) );
			fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3u, hal::stream_semantic::normal, 0 ) );

			unsigned int verts_per_row = segments + 1u;
			unsigned int verts_per_col = rings + 1u;

			unsigned int num_verts = verts_per_row * verts_per_col;
			unsigned int vb_size = num_verts * fmt.vertex_size_in_bytes();
			char* vb = new char[vb_size];

			float theta = 0.f, phi = 0.f;
			float v_angular_stride = glm::two_pi<float>() / float( rings );
			float h_angular_stride = glm::two_pi<float>() / float( segments );

			unsigned int vert = 0u;
			for ( unsigned int v = 0; v < verts_per_col; ++v )
			{
				theta = v_angular_stride * v;

				for ( unsigned int h = 0; h < verts_per_row; ++h )
				{
					glm::vec3* pos = reinterpret_cast<glm::vec3*>( vb + vert * fmt.vertex_size_in_bytes() );
					glm::vec3* nml = pos + 1u;

					phi = h_angular_stride * h;

					// position
					float x = cosf( theta ) * ( radius + ring_radius * cosf( phi ) );
					float y = sinf( theta ) * ( radius + ring_radius * cosf( phi ) );
					float z = ring_radius * sinf( phi );
					*pos = glm::vec3( x, y, z );

					// normal
					glm::vec3 tan0( -sinf( theta ), cosf( theta ), 0.f );
					glm::vec3 tan1( cosf( theta ) * -sinf( phi ), sinf( theta ) * -sinf( phi ), cosf(phi) );
					*nml = glm::normalize( glm::cross( tan0, tan1 ) );

					++vert;
				}
			}

			unsigned int num_indices = segments * rings * 6u;
			unsigned int ib_size = sizeof( unsigned int ) * num_indices;
			char* ib = new char[ib_size];

			unsigned int idx = 0u;
			for ( unsigned int v = 0; v < rings; ++v )
			{
				for ( unsigned int h = 0; h < segments; ++h )
				{
					unsigned int* cur_idx = reinterpret_cast<unsigned int*>( ib + idx * sizeof( unsigned int ) );

					unsigned int lt = h + v * verts_per_row;
					unsigned int rt = ( h + 1 ) + v * verts_per_row;
					unsigned int lb = h + ( v + 1 ) * verts_per_row;
					unsigned int rb = ( h + 1 ) + ( v + 1 ) * verts_per_row;

					cur_idx[0] = lt;
					cur_idx[1] = lb;
					cur_idx[2] = rt;
					cur_idx[3] = rb;
					cur_idx[4] = rt;
					cur_idx[5] = lb;

					idx += 6;
				}
			}

			hal::mesh* mesh = hal::device::create_mesh( fmt, hal::primitive_type::triangle_list, num_verts, vb, num_indices / 3u, ib, support_readback );
			auto bound = sphere_bound::from_points( reinterpret_cast<glm::vec3*>( &vb[0] ), fmt.vertex_size_in_bytes(), num_verts );
			render::model_part* part = new render::model_part( mesh, glm::mat4(), bound, "torus" );
			render::model* model = new render::model;
			model->add_part( part );
			hal::device::release( mesh );

			delete[] vb;
			delete[] ib;

			timer.print_ms_to_debug( "created torus mesh with radius %f, ring radius %f, %i segments, and %i rings (readback=%s)", radius, ring_radius, segments, rings, support_readback ? "true" : "false" );

			return model;
		}
	}
}
