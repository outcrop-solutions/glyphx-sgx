
#include <hal/hal.h>
#include "gadgetmanager.h"
#include <render/model.h>
#include <render/model_loader.h>
#include <render/color.h>
#include <render/perspective_camera.h>
#include <render/ortho_camera.h>
#include "glyphscene.h"

namespace SynGlyphX
{
	namespace
	{
		const float size = 1.f;
		const float switch_size = 0.2f;
		auto color = glm::vec3( 1.f, 1.f, 1.f );
		unsigned int switch_rt_size_x = 256u, switch_rt_size_y = 256u;
	}

	GadgetManager::GadgetManager( GlyphScene& _scene ) : scene( _scene ), gadget_model( nullptr ), switch_model( nullptr )
	{
		effect = hal::device::load_effect( "shaders/solid.vert", nullptr, "shaders/solid.frag" );
		switch_effect = hal::device::load_effect( "shaders/texture.vert", nullptr, "shaders/texture.frag" );
		font = hal::device::load_font( "fonts/OpenSans-Regular.ttf", 32 );

		float square[]
		{
			-0.5f, 0.f, -0.5f, 0.f, 1.f,
			0.5f, 0.f, -0.5f, 1.f, 1.f,
			0.5f, 0.f, 0.5f, 1.f, 0.f,
			-0.5f, 0.f, 0.5f, 0.f, 0.f,
		};
		unsigned int square_indices[]
		{
			0u, 2u, 1u, 0u, 3u, 2u,
		};

		hal::vertex_format fmt;
		fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3, hal::stream_semantic::position, 0 ) );
		fmt.add_stream( hal::stream_info( hal::stream_type::float32, 2, hal::stream_semantic::texcoord, 0 ) );

		billboard_mesh = hal::device::create_mesh( fmt, hal::primitive_type::triangle_list, 4, square, 2, square_indices, true );

		gadget_model = render::load_model( "meshes/sphere_2.dae", { true } );
		
		auto switch_part = new render::model_part( billboard_mesh, glm::mat4(), "group switch" );
		switch_model = new render::model;
		switch_model->add_part( switch_part );
		switch_model->set_transform( glm::scale( glm::mat4(), glm::vec3( 0.5f, 0.5f, 0.5f ) ) );

		switch_camera = new render::ortho_camera( switch_rt_size_x, switch_rt_size_y, -1024.f, 1024.f );
		switch_camera->update_viewport_size( switch_rt_size_x, switch_rt_size_y );
	}

	GadgetManager::~GadgetManager()
	{
		clear();
		hal::device::release( effect );
		hal::device::release( billboard_mesh );
		hal::device::release( font );
		delete gadget_model;
		delete switch_model;
		delete switch_camera;
	}

	void GadgetManager::clear()
	{
		gadgets.clear();
	}

	std::pair<unsigned int, float> GadgetManager::pick( const render::perspective_camera* camera, const glm::vec3& origin, const glm::vec3& dir, float max_distance )
	{
		float best_dist = FLT_MAX;
		gadget* best_gadget = nullptr;
		for ( auto i = 0u; i < gadgets.size(); ++i )
		{
			auto& g = gadgets[i];
			glm::vec3 pt;
			if ( switch_model->pick( origin, dir, compute_switch_transform( camera, g ), pt ) )
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
		g.exploded_offset = scale * 1.5f;
		g.scale = scale;
		g.position = position;
		g.texture = nullptr;
		g.texture_rt = nullptr;
		gadgets.push_back( g );
	}

	void GadgetManager::render( hal::context* context, render::perspective_camera* camera )
	{
		context->set_rasterizer_state( hal::rasterizer_state{ true, true, false, false } );
		context->bind( effect );

		for ( auto& g : gadgets )
		{
			if ( g.texture == nullptr )
				setup_texture( context, g );

			float gadget_alpha = 1.f;
			auto gadget_transform = compute_gadget_transform( g );
			if ( scene.getGroupStatus() != 0.f )
			{
				if ( unsigned int( scene.getActiveGroup() ) != g.group )
					gadget_alpha = 1.f - scene.getGroupStatus();
			}

			renderer.add_blended_batch( gadget_model, effect, gadget_transform, glm::vec4( color, 0.5f * gadget_alpha ) );
		}
		renderer.render( context, camera );

		// temp; optimize
		for ( auto& g : gadgets )
		{
			auto switch_transform = compute_switch_transform( camera, g );
			context->bind( 0u, g.texture );
			renderer.add_blended_batch( switch_model, switch_effect, switch_transform, render::color::white() );
			renderer.render( context, camera );
		}
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

	glm::mat4 GadgetManager::compute_switch_transform( const render::perspective_camera* camera, const gadget& g )
	{
		auto gadget_to_cam = camera->get_position() - g.position;
		float ax = atan2f( gadget_to_cam.x, gadget_to_cam.y );
		//float ay = atan2f( gadget_to_cam.z, gadget_to_cam.y );
		auto rotate = glm::mat4();
		//rotate = glm::rotate( rotate, -ay, glm::vec3( 1.f, 0.f, 0.f ) );
		rotate = glm::rotate( rotate, -ax, glm::vec3( 0.f, 0.f, 1.f ) );

		auto switch_pos = glm::vec3( 0.f, 1.f, 0.f ) * g.scale * 0.5f;
		auto switch_transform = rotate * glm::translate( glm::mat4(), switch_pos );

		auto transform = glm::translate( glm::mat4(), g.position ) * glm::scale( glm::mat4(), glm::vec3( g.scale ) ) * switch_transform;
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

	void GadgetManager::setup_texture( hal::context* context, gadget& g )
	{
		g.texture_rt = hal::device::create_render_target_set( switch_rt_size_x, switch_rt_size_y );
		hal::device::add_color_target( g.texture_rt, hal::texture_format::rgba8 );
		g.texture = hal::device::get_target_texture( g.texture_rt, 0u );
		context->bind( g.texture_rt );
		context->clear( hal::clear_type::color, render::color::black() );
		char buf[64];
		sprintf_s( buf, "TEST %i", scene.getGroupSize( g.group ) );
		auto text_pos = glm::vec2( 0.f );
		auto transform = switch_camera->get_proj() * switch_camera->get_view() * glm::translate( glm::mat4(), glm::vec3( glm::round( text_pos ), 0.f ) );
		context->set_depth_state( hal::depth_state::disabled );
		context->set_blend_state( hal::blend_state::alpha );
		context->draw( font, transform, render::color::white(), buf );
		context->bind( (hal::render_target_set*)nullptr );
	}
}
