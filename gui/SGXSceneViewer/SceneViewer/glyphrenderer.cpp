
#include "glyphrenderer.h"
#include <hal/debug.h>
#include <render/perspective_camera.h>
#include <glm/gtx/compatibility.hpp>
#include "glyphnode.h"
#include "glyphgeometrydb.h"

namespace SynGlyphX
{
	namespace
	{
		bool updates_done = false;	// used to track if instance buffers have been rebuilt for profiling purposes

		enum bucket_flags
		{
			WIREFRAME = 1u,
			BLENDED = 2u,
			FILTER_FAIL = 4u,
			SELECTION = 8u,

			// bits 16-31 reserved for group id
		};
		uint16_t ALL_GROUPS = USHRT_MAX;
	}

	GlyphRenderer::glyph_bucket& GlyphRenderer::get_bucket( uint16_t bucket_id, uint16_t group_id )
	{
		uint32_t bid = bucket_id, gid = group_id;
		uint32_t bucket = bid | ( gid << 16 );
		auto& it = buckets.find( bucket );
		if ( it == buckets.end() )
			it = buckets.insert( std::make_pair( bucket, glyph_bucket() ) ).first;
		return it->second;
	}

	void GlyphRenderer::process_buckets( uint16_t flags_on, uint16_t flags_off, uint16_t group_id, std::function<void( glyph_bucket& )> fn )
	{
		for ( auto& b : buckets )
		{
			uint32_t bucket_flags = b.first & 0x0000ffff;	// most significant 16 bits store group index
			uint32_t bucket_group = ( b.first & 0xffff0000 ) >> 16;
			if ( group_id == ALL_GROUPS || group_id == bucket_group )
				if ( ( bucket_flags & flags_on ) == flags_on && ( ~bucket_flags & flags_off ) == flags_off )
					fn( b.second );
		}
	}

	void GlyphRenderer::draw_buckets( hal::context* context, uint16_t flags, uint16_t group_id, unsigned int transform_binding_point, unsigned int material_binding_point, unsigned int anim_binding_point, unsigned int alt_pos_binding_point )
	{
		process_buckets( flags, ~flags, group_id, [=]( glyph_bucket& b )
		{
			b.draw( context, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
		} );
	}

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
			bool passed_filter = scene.passedFilter( &glyph );

			uint32_t bucket_id = 0u;
			if ( !passed_filter )
				bucket_id |= FILTER_FAIL;
			if ( global_wireframe || glyph.getWireframe() )
				bucket_id |= WIREFRAME;
			if ( glyph.getColor().a < 1.f )
				bucket_id |= BLENDED;

			glyph_bucket& bucket = get_bucket( bucket_id, static_cast<uint16_t>( glyph.getAlternatePositionGroup() ) );

			render::model* model = db.get( glyph.getGeometry(), glyph.getTorusRatio() );
			for ( auto part : model->get_parts() )
			{
				bucket.add_instance( part->get_mesh(), glyph.getCachedTransform() * glyph.getVisualTransform() * model->get_transform() * part->get_transform(), glyph.getColor(), glyph.getAnimationAxis(), glyph.getAnimationRate(), glyph.getAnimationCenter(), glyph.getAlternatePosition(), glyph.getAlternatePositionGroup() );
				if ( bound_vis_enabled )
					add_bound_to_bucket( glyph, get_bucket( WIREFRAME | passed_filter ? 0u : FILTER_FAIL, 0u ) );
			}
			return true;
		}, false );
	}

	void GlyphRenderer::clear()
	{
		for ( auto& b : buckets )
			b.second.clear();
		buckets.clear();
	}

	void GlyphRenderer::update_instances( hal::context* context )
	{
		updates_done = false;
		hal::debug::profile_timer timer;
		for ( auto& b : buckets )
			b.second.update_instances( context, updates_done );

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
			context->set_constant( glyph_effect, "global_material_data", "base_alpha", 1.f );

			hal::rasterizer_state filled{ true, true, false };
			hal::rasterizer_state wire{ true, false, true };

			if ( scene->getGroupStatus() > 0.f )
			{
				context->set_depth_state( hal::depth_state::read_only );
				context->set_blend_state( hal::blend_state::alpha );
			}
			else
			{
				context->set_depth_state( hal::depth_state::read_write );
				context->set_blend_state( hal::blend_state::disabled );
			}
			context->bind( glyph_effect );
			context->set_rasterizer_state( filled );
			draw_buckets( context, 0u, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			auto filter_mode = scene->getFilterMode();
			if ( filter_mode == FilteredResultsDisplayMode::ShowUnfiltered ) 
				draw_buckets( context, FILTER_FAIL, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );

			context->set_rasterizer_state( wire );
			draw_buckets( context, WIREFRAME, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			if ( filter_mode == FilteredResultsDisplayMode::ShowUnfiltered )
				draw_buckets( context, WIREFRAME | FILTER_FAIL, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
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
			draw_buckets( context, BLENDED, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			draw_buckets( context, WIREFRAME | BLENDED, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			auto filter_mode = scene->getFilterMode();
			if ( filter_mode == FilteredResultsDisplayMode::ShowUnfiltered )
			{
				draw_buckets( context, BLENDED | FILTER_FAIL, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
				draw_buckets( context, BLENDED | FILTER_FAIL | WIREFRAME, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
			}

			if ( filter_mode == FilteredResultsDisplayMode::TranslucentUnfiltered )
			{
				context->set_constant( glyph_effect, "global_material_data", "base_alpha", filter_alpha );
				draw_buckets( context, BLENDED | FILTER_FAIL, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
				draw_buckets( context, FILTER_FAIL, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );

				context->set_rasterizer_state( hal::rasterizer_state{ true, false, true } );
				context->set_constant( glyph_effect, "global_material_data", "base_alpha", wire_filter_alpha );
				draw_buckets( context, WIREFRAME | FILTER_FAIL, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
				draw_buckets( context, WIREFRAME | BLENDED | FILTER_FAIL, ALL_GROUPS, transform_binding_point, material_binding_point, anim_binding_point, alt_pos_binding_point );
				context->set_rasterizer_state( filled );
			}

			renderSelection( context, camera, elapsed_seconds );
		}

		previous_frame_time = elapsed_seconds;
	}

	void GlyphRenderer::renderSelection( hal::context* context, render::perspective_camera* camera, float elapsed_seconds )
	{
		auto& selection = get_bucket( SELECTION, 0u );
		auto& selection_wireframe = get_bucket( SELECTION | WIREFRAME, 0u );

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
