
#include "gadgetmanager.h"
#include <hal/hal.h>
#include <render/model.h>
#include <render/model_loader.h>
#include <render/color.h>
#include <render/perspective_camera.h>
#include "glyphscene.h"

namespace SynGlyphX
{
	namespace
	{
		const float size = 1.f;
		auto color = glm::vec3( 1.f, 1.f, 1.f );
	}

	GadgetManager::GadgetManager( GlyphScene& _scene ) : scene( _scene )
	{
		effect = hal::device::load_effect( "shaders/solid.vert", nullptr, "shaders/solid.frag" );
		texture = hal::device::load_texture( "textures/superimposed.png" );

		float square[]
		{
			-0.5f, -0.5f, 0.f, 0.f, 1.f,
			0.5f, -0.5f, 0.f, 1.f, 1.f,
			0.5f, 0.5f, 0.f, 1.f, 0.f,
			-0.5f, 0.5f, 0.f, 0.f, 0.f,
		};
		unsigned int square_indices[]
		{
			0u, 1u, 2u, 0u, 2u, 3u
		};

		hal::vertex_format fmt;
		fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3, hal::stream_semantic::position, 0 ) );
		fmt.add_stream( hal::stream_info( hal::stream_type::float32, 2, hal::stream_semantic::texcoord, 0 ) );

		billboard_mesh = hal::device::create_mesh( fmt, hal::primitive_type::triangle_list, 4, square, 2, square_indices, true );
	}

	GadgetManager::~GadgetManager()
	{
		clear();
		hal::device::release( effect );
		hal::device::release( texture );
		hal::device::release( billboard_mesh );
	}

	void GadgetManager::clear()
	{
		for ( auto& g : gadgets )
			delete g.model;
		gadgets.clear();
	}

	std::pair<unsigned int, float> GadgetManager::pick( const render::camera* camera, const glm::vec3& origin, const glm::vec3& dir, float max_distance )
	{
		float best_dist = FLT_MAX;
		gadget* best_gadget = nullptr;
		for ( auto i = 0u; i < gadgets.size(); ++i )
		{
			auto& g = gadgets[i];
			glm::vec3 pt;
			if ( g.model->pick( origin, dir, compute_gadget_transform( g ), pt ) )
			{
				float dist = glm::distance( camera->get_position(), pt );
				if ( dist < max_distance && dist < best_dist )
				{
					best_dist = dist;
					best_gadget = &g;
				}
			}
		}

		if ( best_gadget )
			return { best_gadget->group, best_dist };
		else
			return { 0, FLT_MAX };
	}

	void GadgetManager::create( std::function<void( void )> on_click, unsigned int group, const glm::vec3& position, float scale )
	{
		gadget g;
		g.group = group;
		g.on_click = on_click;
		g.model = render::load_model( "meshes/sphere_2.dae", { true } );
		g.exploded_offset = scale * 1.5f;
		g.scale = scale;
		g.position = position;
		gadgets.push_back( g );
	}

	void GadgetManager::render( hal::context* context, render::perspective_camera* camera )
	{
		context->set_rasterizer_state( hal::rasterizer_state{ true, true, false, false } );
		context->bind( effect );
		context->bind( 0u, texture );

		for ( auto& g : gadgets )
		{
			float gadget_alpha = 1.f;
			glm::mat4 gadget_transform = compute_gadget_transform( g );
			if ( scene.getGroupStatus() != 0.f )
			{
				if ( unsigned int( scene.getActiveGroup() ) != g.group )
					gadget_alpha = 1.f - scene.getGroupStatus();
			}

			renderer.add_blended_batch( g.model, effect, gadget_transform, glm::vec4( color, 0.5f * gadget_alpha ) );
		}
		renderer.render( context, camera );
	}

	glm::mat4 GadgetManager::compute_gadget_transform( const gadget& g )
	{
		auto transform = glm::translate( glm::mat4(), g.position ) * glm::scale( glm::mat4(), glm::vec3( g.scale ) );
		if ( scene.getGroupStatus() != 0.f )
		{
			if ( unsigned int( scene.getActiveGroup() ) == g.group )
			{
				transform = glm::scale( transform, glm::vec3( glm::mix( 1.f, 0.5f, scene.getGroupStatus() ) ) );
				transform = glm::translate( transform, glm::vec3( 0.f, 0.f, scene.getGroupStatus() * g.exploded_offset ) );
			}
		}
		return transform;
	}
}
