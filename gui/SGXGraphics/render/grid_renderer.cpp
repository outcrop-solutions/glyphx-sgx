
#include "grid_renderer.h"
#include <hal/device.h>
#include <hal/context.h>
#include <hal/vertex_format.h>
#include "camera.h"

namespace SynGlyphX
{
	namespace render
	{
		grid_renderer::grid_renderer()
		{
			effect = hal::device::create_effect( "shaders/line.vert", nullptr, "shaders/line.frag" );
		}

		grid_renderer::~grid_renderer()
		{
			if ( effect ) hal::device::release( effect );
			clear();
		}

		void grid_renderer::clear()
		{
			for ( auto& g : grids )
				if ( g.mesh ) hal::device::release( g.mesh );
			grids.clear();
		}

		void grid_renderer::add( const glm::mat4& _transform, const glm::vec2& dimensions, unsigned int divisions_x, unsigned int divisions_y, const glm::vec4& _base_color )
		{
			float div_size_x = dimensions.x / static_cast<float>( divisions_x );
			float div_size_y = dimensions.y / static_cast<float>( divisions_y );
			std::vector<glm::vec3> vertices;
			std::vector<unsigned int> indices;
			unsigned int idx = 0u;

			for ( unsigned int x = 0; x < divisions_x + 1; ++x )
			{
				vertices.push_back( glm::vec3( div_size_x * static_cast<float>( x ), 0.f, 0.f ) - glm::vec3( dimensions * 0.5f, 0.f ) );
				vertices.push_back( glm::vec3( div_size_x * static_cast<float>( x ), dimensions.y, 0.f ) - glm::vec3( dimensions * 0.5f, 0.f ) );
				indices.push_back( idx++ ); indices.push_back( idx++ );
			}

			for ( unsigned int y = 0; y < divisions_y + 1; ++y )
			{
				vertices.push_back( glm::vec3( 0.f, div_size_y * static_cast<float>( y ), 0.f ) - glm::vec3( dimensions * 0.5f, 0.f ) );
				vertices.push_back( glm::vec3( dimensions.x, div_size_y * static_cast<float>( y ), 0.f ) - glm::vec3( dimensions * 0.5f, 0.f ) );
				indices.push_back( idx++ ); indices.push_back( idx++ );
			}

			hal::vertex_format fmt;
			fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3u, hal::stream_semantic::position, 0u ) );

			grid_info g;
			g.mesh = hal::device::create_mesh( fmt, hal::primitive_type::line_list, vertices.size(), &vertices[0], indices.size() / 2u, &indices[0] );
			g.base_color = _base_color;
			g.transform = _transform;
			grids.push_back( g );
		}

		void grid_renderer::draw( hal::context* context, render::camera* camera, const glm::vec4& color )
		{
			context->bind( effect );
			context->set_depth_state( hal::depth_state::read_only );
			hal::rasterizer_state rast{ true, false, false, true };
			context->set_rasterizer_state( rast );
			context->set_constant( effect, "camera_data", "view", camera->get_view() );
			context->set_constant( effect, "camera_data", "proj", camera->get_proj() );
			for ( auto& g : grids )
			{
				glm::vec4 combined_color = color * g.base_color;
				context->set_blend_state( combined_color.a != 1.f ? hal::blend_state::alpha : hal::blend_state::disabled );

				context->set_constant( effect, "instance_data", "world", g.transform );
				context->set_constant( effect, "material", "tint_color", combined_color );

				context->draw( g.mesh );
			}
		}
	}
}
