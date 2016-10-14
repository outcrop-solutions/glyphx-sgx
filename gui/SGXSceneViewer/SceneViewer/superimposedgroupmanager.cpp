
#include <hal/hal.h>
#include "SuperimposedGroupManager.h"
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
		const float switch_size = 0.5f;
		auto color = glm::vec3( 1.f, 1.f, 1.f );
		const float gadget_base_alpha = 0.25f;
		glm::vec2 switch_fade_dist( 125.f, 150.f );
		glm::vec2 bound_fade_dist( 450.f, 500.f );
		//		unsigned int switch_rt_size_x = 256u, switch_rt_size_y = 256u;
	}

	SuperimposedGroupManager::SuperimposedGroupManager( GlyphScene& _scene ) : scene( _scene ), gadget_model( nullptr ), switch_model( nullptr ),
		mode( SuperimposedGadgetMode::OnSelection )
	{
		effect = hal::device::load_effect( "shaders/gadget_bound.vert", nullptr, "shaders/gadget_bound.frag" );
		switch_effect = hal::device::load_effect( "shaders/texture.vert", nullptr, "shaders/texture.frag" );
		font = hal::device::load_font( "fonts/OpenSans-Regular.ttf", 32 );
		explode_icon = hal::device::load_texture( "textures/expand-64.tga" );
		collapse_icon = hal::device::load_texture( "textures/compress-64.tga" );

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

		// switch_camera = new render::ortho_camera( switch_rt_size_x, switch_rt_size_y, -1024.f, 1024.f );
		// switch_camera->update_viewport_size( switch_rt_size_x, switch_rt_size_y );
	}

	SuperimposedGroupManager::~SuperimposedGroupManager()
	{
		clear();
		hal::device::release( effect );
		hal::device::release( billboard_mesh );
		hal::device::release( font );
		delete gadget_model;
		delete switch_model;
		// delete switch_camera;
	}

	void SuperimposedGroupManager::clear()
	{
		gadgets.clear();
	}

	std::pair<unsigned int, float> SuperimposedGroupManager::pick( const render::perspective_camera* camera, const glm::vec3& origin, const glm::vec3& dir, float max_distance )
	{
		float best_dist = FLT_MAX;
		gadget* best_gadget = nullptr;
		for ( auto i = 0u; i < gadgets.size(); ++i )
		{
			auto& g = gadgets[i];
			if ( scene.getGroupStatus() == 0.f || ( g.group == scene.getActiveGroup() ) )
			{
				if ( g.switch_alpha > 0.f )
				{
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
			}
		}

		if ( best_gadget )
			return{ best_gadget->group, best_dist };
		else
			return{ 0, FLT_MAX };
	}

	void SuperimposedGroupManager::create( unsigned int group, const glm::vec3& position, float scale )
	{
		gadget g;
		g.group = group;
		g.exploded_offset = scale * 2.f;
		g.scale = scale;
		g.position = position;
		//		g.texture = nullptr;
		//		g.texture_rt = nullptr;
		gadgets.push_back( g );
	}

	bool SuperimposedGroupManager::groupInSelection( unsigned int group )
	{
		auto root = scene.getSingleRoot();
		return root ? ( root->getExplodedPositionGroup() == group ) : false;

		// This will show gadgets for *ALL* selected objects (not just if a single one is selected).
		// Superimposed group gadget rendering will need some optimization before this is practical.
/*		bool in_selection = false;
		scene.enumSelected( [&in_selection, group]( const Glyph3DNode& g ) {
			if ( g.getExplodedPositionGroup() == group ) in_selection = true;
		} );
		return in_selection;*/
	}

	void SuperimposedGroupManager::render( hal::context* context, render::perspective_camera* camera )
	{
		update( camera );

		context->set_rasterizer_state( hal::rasterizer_state{ true, true, false, false } );
		context->bind( effect );

		for ( auto& g : gadgets )
		{
#if RENDERED_GADGET_TEXTURE
			if ( g.texture == nullptr )
				setup_texture( context, g );
#endif

			if ( g.bound_alpha > 0.f )
			{
				auto gadget_transform = compute_gadget_transform( g );
				renderer.add_blended_batch( gadget_model, effect, gadget_transform, glm::vec4( color, gadget_base_alpha * g.bound_alpha ) );
			}
		}
		renderer.render( context, camera );

		render::renderer switch_expand, switch_collapse;
		for ( auto& g : gadgets )
		{
			if ( g.switch_alpha > 0.f )
			{
				auto switch_transform = compute_switch_transform( camera, g );
				if ( scene.isExploded( g.group ) )
					switch_collapse.add_blended_batch( switch_model, switch_effect, switch_transform, glm::vec4( 1.f, 1.f, 1.f, g.switch_alpha ) );
				else
					switch_expand.add_blended_batch( switch_model, switch_effect, switch_transform, glm::vec4( 1.f, 1.f, 1.f, g.switch_alpha ) );
			}
		}
		context->bind( 0u, explode_icon );
		switch_expand.render( context, camera );
		context->bind( 0u, collapse_icon );
		switch_collapse.render( context, camera );

		context->set_depth_state( hal::depth_state::read_write );
		context->set_blend_state( hal::blend_state::disabled );
	}

	glm::mat4 SuperimposedGroupManager::compute_gadget_transform( const gadget& g )
	{
		auto scale = glm::scale( glm::mat4(), glm::vec3( g.scale ) );
		auto translate = glm::translate( glm::mat4(), g.position );
		if ( scene.getGroupStatus() != 0.f && scene.getActiveGroup() == g.group )
		{
			scale = glm::scale( scale, glm::vec3( glm::mix( 1.f, 0.5f, scene.getGroupStatus() ) ) );
			translate = glm::translate( translate, glm::vec3( 0.f, 0.f, scene.getGroupStatus() * g.exploded_offset ) );
		}
		return translate * scale;
	}

	float SuperimposedGroupManager::compute_gadget_alpha( const gadget& g, const glm::vec3& cam_pos, const glm::vec2& fade_dist )
	{
		float gadget_alpha = 1.f;
		if ( scene.getGroupStatus() != 0.f )
		{
			if ( scene.getActiveGroup() != g.group )
				gadget_alpha = 1.f - scene.getGroupStatus();
		}

		if ( !groupInSelection( g.group ) )
		{
			const float start_fade_dist = 100.f, end_fade_dist = 120.f;
			float dist = glm::distance( cam_pos, g.position );
			float a = 1.f - glm::clamp( ( dist - fade_dist.x ) / ( fade_dist.y - fade_dist.x ), 0.f, 1.f );
			gadget_alpha *= a;
		}

		return gadget_alpha;
	}

	glm::mat4 SuperimposedGroupManager::compute_switch_transform( const render::perspective_camera* camera, const gadget& g )
	{
		auto gadget_to_cam = camera->get_position() - g.position;
		float ax = atan2f( gadget_to_cam.x, gadget_to_cam.y );
		auto rotate = glm::rotate( glm::mat4(), -ax, glm::vec3( 0.f, 0.f, 1.f ) );
		auto switch_pos = glm::vec3( 0.f, 1.f, 0.f );
		auto offset = glm::translate( glm::mat4(), switch_pos );

		return compute_gadget_transform( g ) * rotate * offset;
	}

	void SuperimposedGroupManager::setup_texture( hal::context* context, gadget& g )
	{
#if RENDERED_GADGET_TEXTURE
		// WIP - render glyph count
		g.texture_rt = hal::device::create_render_target_set( switch_rt_size_x, switch_rt_size_y );
		hal::device::add_color_target( g.texture_rt, hal::texture_format::rgba8 );
		g.texture = hal::device::get_target_texture( g.texture_rt, 0u );
		context->bind( g.texture_rt );
		context->clear( hal::clear_type::color, render::color::black() );
		char buf[64];
		sprintf_s( buf, "%i", scene.getGroupSize( g.group ) );
		auto text_pos = glm::vec2( 0.f );
		auto transform = switch_camera->get_proj() * switch_camera->get_view() * glm::translate( glm::mat4(), glm::vec3( glm::round( text_pos ), 0.f ) );
		context->set_depth_state( hal::depth_state::disabled );
		context->set_blend_state( hal::blend_state::alpha );
		context->draw( font, transform, render::color::white(), buf );
		context->bind( ( hal::render_target_set* )nullptr );
#endif
	}

	void SuperimposedGroupManager::update( render::perspective_camera* camera )
	{
		for ( auto& g : gadgets )
		{
			if ( scene.isExploded( g.group ) || groupInSelection( g.group ) )
			{
				g.switch_alpha = 1.f;
				g.bound_alpha = 1.f;
			}
			else if ( mode == SuperimposedGadgetMode::Always )
			{
				g.switch_alpha = compute_gadget_alpha( g, camera->get_position(), switch_fade_dist );
				g.bound_alpha = compute_gadget_alpha( g, camera->get_position(), bound_fade_dist );
			}
			else
			{
				g.switch_alpha = g.bound_alpha = 0.f;
			}
		}
	}
}
