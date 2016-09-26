
#include "gadgetmanager.h"
#include <hal/hal.h>
#include <render/model.h>
#include <render/model_loader.h>
#include <render/color.h>
#include <render/perspective_camera.h>

namespace SynGlyphX
{
	GadgetManager::GadgetManager()
	{
		effect = hal::device::load_effect( "shaders/gadget.vert", nullptr, "shaders/gadget.frag" );
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

	void GadgetManager::pick( const glm::vec3& origin, const glm::vec3& dir )
	{
	}

	void GadgetManager::create( std::function<void( void )> on_click, const glm::vec3& position )
	{
		gadget g;
		g.on_click = on_click;
		g.model = new render::model;
		const float size = 1.f;
		auto* part = new render::model_part( billboard_mesh, glm::scale( glm::mat4(), glm::vec3( size, size, size ) ), "gadget" );
		g.model->add_part( part );
		g.model->set_transform( glm::translate( glm::mat4(), position ) );
		gadgets.push_back( g );
	}

	void GadgetManager::render( hal::context* context, render::perspective_camera* camera )
	{
		context->set_rasterizer_state( hal::rasterizer_state{ true, false, false, false } );
		context->set_depth_state( hal::depth_state::read_only );
		context->set_blend_state( hal::blend_state::premultiplied_alpha );
		context->bind( effect );
		context->set_constant( effect, "camera_data", "cam_up", camera->get_up() );
		context->set_constant( effect, "camera_data", "cam_right", camera->get_right() );
		context->bind( 0u, texture );
		for ( auto& g : gadgets )
			renderer.add_batch( g.model, effect );
		renderer.render( context, camera );
	}
}
