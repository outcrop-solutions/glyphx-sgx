
#include "glyphrenderer.h"
#include <hal/debug.h>
#include <render/perspective_camera.h>
#include <glm/gtx/compatibility.hpp>
#include "glyphnode.h"
#include "glyphgeometrydb.h"

namespace SynGlyphX
{
	GlyphRenderer::glyph_bucket::~glyph_bucket()
	{
		clear();
	}

	void GlyphRenderer::glyph_bucket::add_instance( hal::mesh* mesh, const glm::mat4& transform, const glm::vec4& color_or_bound, const glm::vec3& anim_axis, const float anim_rate, const glm::vec3& anim_center, const glm::vec3& alt_pos, float alt_pos_group )
	{
		const unsigned int max_instances_per_batch = 512u;
		if ( instances.find( mesh ) == instances.end() )
			instances.insert( std::make_pair( mesh, instance_data_list() ) );
		if ( instances[mesh].size() == 0u || instances[mesh].rbegin()->transforms.size() == max_instances_per_batch )
		{
			instances[mesh].push_back( instance_data() );
			instances[mesh].rbegin()->color_bound_buffer = nullptr;
			instances[mesh].rbegin()->anim_buffer = nullptr;
			instances[mesh].rbegin()->transform_buffer = nullptr;
			instances[mesh].rbegin()->alt_pos_buffer = nullptr;
		}

		auto axis = glm::vec3( anim_axis.x, anim_axis.z, anim_axis.y );

		instances[mesh].rbegin()->transforms.push_back( transform );
		instances[mesh].rbegin()->colors_or_bounds.push_back( color_or_bound );
		instances[mesh].rbegin()->animation.push_back( glm::vec4( axis, anim_rate ) );
		instances[mesh].rbegin()->animation.push_back( glm::vec4( anim_center, 0.f ) );
		instances[mesh].rbegin()->alternate_positions.push_back( glm::vec4( alt_pos, alt_pos_group ) );
		instances[mesh].rbegin()->dirty = true;
	}

	void GlyphRenderer::glyph_bucket::clear()
	{
		for ( auto& entry : instances )
		{
			auto& instance_list = entry.second;
			for ( auto& id : instance_list )
			{
				hal::device::release( id.color_bound_buffer );
				hal::device::release( id.transform_buffer );
				hal::device::release( id.anim_buffer );
				hal::device::release( id.alt_pos_buffer );
			}
		}
		instances.clear();
	}

	void GlyphRenderer::glyph_bucket::update_instances( hal::context* context, bool& updates_done )
	{
		unsigned int update_count = 0u;
		for ( auto& entry : instances )
		{
			auto& instance_list = entry.second;
			for ( auto& id : instance_list )
			{
				if ( id.dirty )
				{
					assert( id.colors_or_bounds.size() == id.transforms.size() );

					size_t transform_size = id.transforms.size() * sizeof( glm::mat4 );
					size_t color_size = id.colors_or_bounds.size() * sizeof( glm::vec4 );
					size_t anim_size = id.animation.size() * sizeof( glm::vec4 );
					size_t alt_pos_size = id.alternate_positions.size() * sizeof( glm::vec4 );

					if ( id.transform_buffer ) hal::device::release( id.transform_buffer );
					if ( id.color_bound_buffer ) hal::device::release( id.color_bound_buffer );
					if ( id.anim_buffer ) hal::device::release( id.anim_buffer );
					if ( id.alt_pos_buffer ) hal::device::release( id.alt_pos_buffer );

					id.transform_buffer = hal::device::create_cbuffer( transform_size );
					id.color_bound_buffer = hal::device::create_cbuffer( color_size );
					id.anim_buffer = hal::device::create_cbuffer( anim_size );
					id.alt_pos_buffer = hal::device::create_cbuffer( alt_pos_size );
					context->update_constant_block( id.transform_buffer, &id.transforms[0], transform_size, hal::cbuffer_usage::static_draw );
					context->update_constant_block( id.color_bound_buffer, &id.colors_or_bounds[0], color_size, hal::cbuffer_usage::static_draw );
					context->update_constant_block( id.anim_buffer, &id.animation[0], anim_size, hal::cbuffer_usage::static_draw );
					context->update_constant_block( id.alt_pos_buffer, &id.alternate_positions[0], alt_pos_size, hal::cbuffer_usage::static_draw );

					++update_count;
					id.dirty = false;
					updates_done = true;
				}
			}
		}
	}

	void GlyphRenderer::glyph_bucket::draw( hal::context* context, unsigned int transform_binding_point, unsigned int material_binding_point, unsigned int anim_binding_point, unsigned int alt_pos_binding_point )
	{
		for ( auto& entry : instances )
		{
			auto mesh = entry.first;
			auto& instance_list = entry.second;

			if ( instance_list.size() > 0u )
			{
				context->begin_instancing( mesh );

				for ( auto& id : instance_list )
				{
					context->bind( transform_binding_point, id.transform_buffer );
					context->bind( material_binding_point, id.color_bound_buffer );
					context->bind( anim_binding_point, id.anim_buffer );
					context->bind( alt_pos_binding_point, id.alt_pos_buffer );
					context->draw_instances( id.transforms.size() );
				}

				context->end_instancing();
			}
		}
	}

	GlyphRenderer::GlyphRenderer( GlyphGeometryDB& _db ) : transform_binding_point( UINT_MAX ), material_binding_point( UINT_MAX ), selection_anim_max_scale( 64.f ),
		animation( true ), global_wireframe( false ), scene( nullptr ), filter_alpha( 0.5f ), selection_animation_time( 0.f ), selection_animation_state( 0.f ),
		bound_vis_enabled( false ), bound_vis_mode( GlyphRenderer::BoundVisMode::Individual ), sel_effect_enabled( true ), db( _db)
	{
		glyph_effect = hal::device::load_effect( "shaders/glyph.vert", nullptr, "shaders/glyph.frag" );
		selection_effect = hal::device::load_effect( "shaders/selection.vert", nullptr, "shaders/selection.frag" );
		hal::device::set_cbuffer_external( glyph_effect, "instance_data" );
		hal::device::set_cbuffer_external( glyph_effect, "material" );
		hal::device::set_cbuffer_external( glyph_effect, "animation" );
		hal::device::set_cbuffer_external( glyph_effect, "alternate_positions" );
		hal::device::set_cbuffer_external( selection_effect, "instance_data" );
		hal::device::set_cbuffer_external( selection_effect, "bounds" );
		hal::device::set_cbuffer_external( selection_effect, "animation" );
		hal::device::set_cbuffer_external( selection_effect, "alternate_positions" );
		db.init();
	}

	GlyphRenderer::~GlyphRenderer()
	{
		hal::device::release( glyph_effect );
		hal::device::release( selection_effect );
		db.clear();
	}

	void GlyphRenderer::add_bound_to_bucket( const Glyph3DNode& glyph, GlyphRenderer::glyph_bucket& bucket )
	{
		auto bound = bound_vis_mode == BoundVisMode::Combined ? glyph.getCachedCombinedBound() : glyph.getCachedBound();
		auto sphere = db.get( GlyphShape::Sphere );
		auto spart = sphere->get_parts()[0];
		auto xform = glm::translate( glm::mat4(), bound.get_center() ) * glm::scale( glm::mat4(), glm::vec3( bound.get_radius() ) );
		bucket.add_instance( spart->get_mesh(), xform * sphere->get_transform() * spart->get_transform(), glyph.getColor(), glyph.getAnimationAxis(), 0.f, glyph.getAnimationCenter() );
	}

	void GlyphRenderer::add( const GlyphScene& scene )
	{
		scene.enumGlyphs( [this, &scene]( const Glyph3DNode& glyph )
		{
			int filter = scene.passedFilter( &glyph ) ? FILTERED : UNFILTERED;

			glyph_bucket* bucket = &solid[filter];
			if ( global_wireframe || glyph.getWireframe() )
			{
				if ( glyph.getColor().a < 1.0f )
					bucket = &wireframe_blended[filter];
				else
					bucket = &wireframe[filter];
			}
			else if ( glyph.getColor().a < 1.0f ) bucket = &blended[filter];

			render::model* model = db.get( glyph.getGeometry(), glyph.getTorusRatio() );
			for ( auto part : model->get_parts() )
			{
				bucket->add_instance( part->get_mesh(), glyph.getCachedTransform() * glyph.getVisualTransform() * model->get_transform() * part->get_transform(), glyph.getColor(), glyph.getAnimationAxis(), glyph.getAnimationRate(), glyph.getAnimationCenter(), glyph.getAlternatePosition(), glyph.getAlternatePositionGroup() );

				if ( bound_vis_enabled )
					add_bound_to_bucket( glyph, wireframe[filter] );
			}
			return true;
		}, false );
	}

	void GlyphRenderer::clear()
	{
		for ( int i = 0; i < 2; ++i )
		{
			solid[i].clear();
			blended[i].clear();
			wireframe[i].clear();
			wireframe_blended[i].clear();
		}
	}

	void GlyphRenderer::update_instances( hal::context* context )
	{
		updates_done = false;
		hal::debug::profile_timer timer;
		for ( int i = 0; i < 2; ++i )
		{
			solid[i].update_instances( context, updates_done );
			blended[i].update_instances( context, updates_done );
			wireframe[i].update_instances( context, updates_done );
			wireframe_blended[i].update_instances( context, updates_done );
		}

		if ( updates_done )
			timer.print_ms_to_debug( "rebuilt instance buffers" );
	}

	void GlyphRenderer::render_solid( hal::context* context, render::perspective_camera* camera, float elapsed_seconds )
	{
		transform_binding_point = context->get_uniform_block_index( glyph_effect, "instance_data" );
		material_binding_point = context->get_uniform_block_index( glyph_effect, "material" );
		anim_binding_point = context->get_uniform_block_index( glyph_effect, "animation" );
		alt_pos_binding_point = context->get_uniform_block_index( glyph_effect, "alternate_positions" );

		if ( scene )
		{
			if ( scene->getChanged() )
			{
				clear();
				add( *scene );
				scene->clearChangedFlag();
			}

			update_instances( context );

			const float zero = 0.f;

			context->set_constant( glyph_effect, "scene_data", "view", camera->get_view() );
			context->set_constant( glyph_effect, "scene_data", "proj", camera->get_proj() );
			context->set_constant( glyph_effect, "scene_data", "elapsed_seconds", animation ? elapsed_seconds : zero );
			context->set_constant( glyph_effect, "scene_data", "alternate_position_state", scene->getGroupStatus() );
			context->set_constant( glyph_effect, "scene_data", "active_alternate_position_group", scene->getActiveGroup() );

			hal::rasterizer_state filled{ true, true, false };
			hal::rasterizer_state wire{ true, false, true };

			context->set_depth_state( hal::depth_state::read_write );
			context->set_blend_state( hal::blend_state::disabled );
			context->bind( glyph_effect );
			context->set_rasterizer_state( filled );
			solid[FILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			auto filter_mode = scene->getFilterMode();
			if ( filter_mode == FilteredResultsDisplayMode::ShowUnfiltered ) solid[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );

			context->set_rasterizer_state( wire );
			wireframe[FILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			if ( filter_mode == FilteredResultsDisplayMode::ShowUnfiltered ) wireframe[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			context->set_rasterizer_state( filled );
		}
	}

	void GlyphRenderer::render_blended( hal::context* context, render::perspective_camera* camera, float elapsed_seconds )
	{
		if ( scene )
		{
			const float zero = 0.f;

			context->set_constant( glyph_effect, "scene_data", "view", camera->get_view() );
			context->set_constant( glyph_effect, "scene_data", "proj", camera->get_proj() );
			context->set_constant( glyph_effect, "scene_data", "elapsed_seconds", animation ? elapsed_seconds : zero );
			context->set_constant( glyph_effect, "scene_data", "camera_pos", camera->get_position() );

			hal::rasterizer_state filled{ true, true, false };
			
			context->set_depth_state( hal::depth_state::read_only );
			context->set_blend_state( hal::blend_state::alpha );
			context->set_constant( glyph_effect, "global_material_data", "base_alpha", 1.f );
			context->set_rasterizer_state( filled );
			context->bind( glyph_effect );
			blended[FILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			wireframe_blended[FILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			auto filter_mode = scene->getFilterMode();
			if ( filter_mode == FilteredResultsDisplayMode::ShowUnfiltered )
			{
				blended[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
				wireframe_blended[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			}

			if ( filter_mode == FilteredResultsDisplayMode::TranslucentUnfiltered )
			{
				context->set_constant( glyph_effect, "global_material_data", "base_alpha", filter_alpha );
				blended[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
				solid[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );

				context->set_rasterizer_state( hal::rasterizer_state{ true, false, true } );
				context->set_constant( glyph_effect, "global_material_data", "base_alpha", wire_filter_alpha );
				wireframe[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
				wireframe_blended[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
				context->set_rasterizer_state( filled );
			}

			renderSelection( context, camera, elapsed_seconds );
		}

		previous_frame_time = elapsed_seconds;
	}

	void GlyphRenderer::renderSelection( hal::context* context, render::perspective_camera* camera, float elapsed_seconds )
	{
		if ( scene->getSelectionChanged() )
			selection_animation_state = 0.f;

		if ( !sel_effect_enabled ) return;

		if ( scene && !scene->selectionEmpty() )
		{
			// Pulse moves through the speeds array after the number of pulses in speed_thresholds.
			const int speed_thresholds[] = { 0, 3, 6 };
			const float speeds[] = { 0.5f, 0.25f, 0.15f };
			float count;
			float anim_state = modf( selection_animation_state, &count );
			float speed = 0.f;
			for ( unsigned int i = 0; i < 3; ++i )
				if ( count >= float( speed_thresholds[i] ) )
					speed = speeds[i];
			selection_animation_state += speed * ( elapsed_seconds - previous_frame_time );
			anim_state = sinf( anim_state * glm::pi<float>() * 0.5f );

			const unsigned int maximum_count = UINT_MAX;	// change to limit number of pulses
			if ( float( count ) < maximum_count )
			{
				sel_transform_binding_point = context->get_uniform_block_index( selection_effect, "instance_data" );
				sel_bound_binding_point = context->get_uniform_block_index( selection_effect, "bounds" );
				sel_anim_binding_point = context->get_uniform_block_index( selection_effect, "animation" );
				sel_alt_pos_binding_point = context->get_uniform_block_index( selection_effect, "alternate_positions" );

				selection_animation_time += elapsed_seconds - previous_frame_time;

				if ( scene->getSelectionChanged() )
				{
					selection_animation_time = 0.f;
					hal::debug::profile_timer timer;
					selection.clear();
					selection_wireframe.clear();
					scene->enumSelected( [&]( const Glyph3DNode& glyph ) {
						auto glyph_transform = glyph.getCachedTransform() * glyph.getVisualTransform();
						auto model = db.get( glyph.getGeometry(), glyph.getTorusRatio(), 0.f );
						for ( auto part : model->get_parts() )
						{
							// for the selection effect we don't care about instance color, so we can pack the bound into that
							// constant buffer instead.
							selection.add_instance( part->get_mesh(), glyph_transform * model->get_transform() * part->get_transform(), glm::vec4( glyph.getCachedBound().get_center(), glyph.getCachedBound().get_radius() ), glyph.getAnimationAxis(), glyph.getAnimationRate(), glyph.getAnimationCenter(), glyph.getAlternatePosition(), glyph.getAlternatePositionGroup() );

							if ( bound_vis_enabled )
								add_bound_to_bucket( glyph, selection_wireframe );
						}
					} );
					selection.update_instances( context, updates_done );
					selection_wireframe.update_instances( context, updates_done );
					scene->clearSelectionChangedFlag();
					timer.print_ms_to_debug( "built selection instance buffers" );
				}

				context->set_depth_state( hal::depth_state::read_only );
				context->bind( selection_effect );
				context->set_blend_state( hal::blend_state::alpha );

				context->set_constant( selection_effect, "global_data", "tint_color", render::color::yellow() );
				context->set_constant( selection_effect, "global_data", "elapsed_seconds", elapsed_seconds );
				context->set_constant( selection_effect, "global_data", "selection_anim_max_scale", selection_anim_max_scale );
				context->set_constant( selection_effect, "global_data", "selection_anim_state", anim_state );
				context->set_constant( selection_effect, "global_data", "alternate_position_state", scene->getGroupStatus() );
				context->set_constant( selection_effect, "global_data", "active_alternate_position_group", scene->getActiveGroup() );
				context->set_constant( selection_effect, "camera_data", "viewport", glm::vec2( camera->get_viewport_w(), camera->get_viewport_h() ) );
				context->set_constant( selection_effect, "camera_data", "view", camera->get_view() );
				context->set_constant( selection_effect, "camera_data", "proj", camera->get_proj() );
				context->set_constant( selection_effect, "camera_data", "camera_pos", camera->get_position() );
				context->set_constant( selection_effect, "camera_data", "upvec", camera->get_world_up() );

				hal::rasterizer_state filled{ true, true, false };
				context->set_rasterizer_state( filled );
				selection.draw( context, sel_transform_binding_point, sel_bound_binding_point, sel_anim_binding_point, sel_alt_pos_binding_point );
				hal::rasterizer_state wire{ true, true, true };
				context->set_rasterizer_state( wire );
				selection_wireframe.draw( context, sel_transform_binding_point, sel_bound_binding_point, sel_anim_binding_point, sel_alt_pos_binding_point );
				context->set_rasterizer_state( filled );
			}
			else
			{
				selection.clear();
				selection_wireframe.clear();
			}
		}
		else
		{
			selection.clear();
			selection_wireframe.clear();
		}
	}

	void GlyphRenderer::enableBoundVis( bool enable )
	{
		if ( enable != bound_vis_enabled )
		{
			bound_vis_enabled = enable;
			clear();
			add( *scene );
		}
	}

	void GlyphRenderer::setBoundVisMode( BoundVisMode mode )
	{
		if ( mode != bound_vis_mode )
		{
			bound_vis_mode = mode;
			if ( bound_vis_enabled )
			{
				clear();
				add( *scene );
			}
		}
	}
}
