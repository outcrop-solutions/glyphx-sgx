
#include "glyphrenderer.h"
#include <hal/debug.h>
#include <render/perspective_camera.h>
#include "glyphnode.h"
#include "glyphgeometrydb.h"

namespace SynGlyphX
{
	namespace
	{
		bool updates_done = false;	// used to track if instance buffers have been rebuilt for profiling purposes
	}

	GlyphRenderer::glyph_bucket::~glyph_bucket()
	{
		for ( auto& entry : instances )
		{
			auto& instance_list = entry.second;
			for ( auto& id : instance_list )
			{
				hal::device::release( id.color_bound_buffer );
				hal::device::release( id.transform_buffer );
				hal::device::release( id.anim_buffer );
			}
		}
	}

	void GlyphRenderer::glyph_bucket::add_instance( hal::mesh* mesh, const glm::mat4& transform, const glm::vec4& color_or_bound, const glm::vec3& anim_axis, const float anim_rate )
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
		}
		instances[mesh].rbegin()->transforms.push_back( transform );
		instances[mesh].rbegin()->colors_or_bounds.push_back( color_or_bound );
		instances[mesh].rbegin()->animation.push_back( glm::vec4( anim_axis, anim_rate ) );
		instances[mesh].rbegin()->dirty = true;
	}

	void GlyphRenderer::glyph_bucket::clear()
	{
		instances.clear();
	}

	void GlyphRenderer::glyph_bucket::update_instances( hal::context* context )
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

					if ( id.transform_buffer ) hal::device::release( id.transform_buffer );
					if ( id.color_bound_buffer ) hal::device::release( id.color_bound_buffer );
					if ( id.anim_buffer ) hal::device::release( id.anim_buffer );

					id.transform_buffer = hal::device::create_cbuffer( transform_size );
					id.color_bound_buffer = hal::device::create_cbuffer( color_size );
					id.anim_buffer = hal::device::create_cbuffer( anim_size );
					context->update_constant_block( id.transform_buffer, &id.transforms[0], transform_size );
					context->update_constant_block( id.color_bound_buffer, &id.colors_or_bounds[0], color_size );
					context->update_constant_block( id.anim_buffer, &id.animation[0], anim_size );

					++update_count;
					id.dirty = false;
					updates_done = true;
				}
			}
		}
	}

	void GlyphRenderer::glyph_bucket::draw( hal::context* context, unsigned int transform_binding_point, unsigned int material_binding_point, unsigned int anim_binding_point )
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
					context->draw_instances( id.transforms.size() );
				}

				context->end_instancing();
			}
		}
	}

	GlyphRenderer::GlyphRenderer() : transform_binding_point( UINT_MAX ), material_binding_point( UINT_MAX ), selection_anim_max_scale( 64.f ),
		animation( true ), global_wireframe( false ), scene( nullptr ), filter_alpha( 0.5f ),
		bound_vis_enabled( false )
	{
		glyph_effect = hal::device::create_effect( "shaders/glyph.vert", nullptr, "shaders/glyph.frag" );
		selection_effect = hal::device::create_effect( "shaders/selection.vert", nullptr, "shaders/selection.frag" );
		hal::device::set_uniform_block_external( glyph_effect, "instance_data" );
		hal::device::set_uniform_block_external( glyph_effect, "material" );
		hal::device::set_uniform_block_external( glyph_effect, "animation" );
		hal::device::set_uniform_block_external( selection_effect, "instance_data" );
		hal::device::set_uniform_block_external( selection_effect, "bounds" );
		hal::device::set_uniform_block_external( selection_effect, "animation" );
	}

	GlyphRenderer::~GlyphRenderer()
	{
		hal::device::release( glyph_effect );
		hal::device::release( selection_effect );
	}

	void GlyphRenderer::add( const GlyphScene& scene )
	{
		scene.enumGlyphs( [this, &scene]( const Glyph3DNode& glyph )
		{
			int filter = scene.isFiltered( &glyph ) ? FILTERED : UNFILTERED;

			glyph_bucket* bucket = &solid[filter];
			if ( global_wireframe || glyph.getWireframe() )
			{
				if ( glyph.getColor().a < 1.0f )
					bucket = &wireframe_blended[filter];
				else
					bucket = &wireframe[filter];
			}
			else if ( glyph.getColor().a < 1.0f ) bucket = &blended[filter];

			render::model* model = GlyphGeometryDB::get( glyph.getGeometry(), glyph.getTorusRatio() );
			for ( auto part : model->get_parts() )
			{
				bucket->add_instance( part->get_mesh(), glyph.getCachedTransform() * glyph.getVisualTransform() * model->get_transform() * part->get_transform(), glyph.getColor(), glyph.getAnimationAxis(), glyph.getAnimationRate() );

				if ( bound_vis_enabled )
				{
					auto bound = glyph.getCachedCombinedBound();
					auto sphere = GlyphGeometryDB::get( GlyphShape::Sphere );
					auto spart = sphere->get_parts()[0];
					auto xform = glm::translate( glm::mat4(), bound.get_center() ) * glm::scale( glm::mat4(), glm::vec3( bound.get_radius() ) );
					wireframe[filter].add_instance( spart->get_mesh(), xform * sphere->get_transform() * spart->get_transform(), glyph.getColor(), glyph.getAnimationAxis(), 0.f );
				}
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
			solid[i].update_instances( context );
			blended[i].update_instances( context );
			wireframe[i].update_instances( context );
			wireframe_blended[i].update_instances( context );
		}

		if ( updates_done )
			timer.print_ms_to_debug( "rebuilt instance buffers" );
	}

	void GlyphRenderer::render_solid( hal::context* context, render::perspective_camera* camera, float elapsed_seconds )
	{
		transform_binding_point = context->get_uniform_block_index( glyph_effect, "instance_data" );
		material_binding_point = context->get_uniform_block_index( glyph_effect, "material" );
		anim_binding_point = context->get_uniform_block_index( glyph_effect, "animation" );

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

			hal::rasterizer_state filled{ true, true, false };
			hal::rasterizer_state wire{ true, false, true };

			context->set_depth_state( hal::depth_state::read_write );
			context->set_blend_state( hal::blend_state::disabled );
			context->bind( glyph_effect );
			context->set_rasterizer_state( filled );
			solid[FILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );
			auto filter_mode = scene->getFilterMode();
			if ( filter_mode == FilteredResultsDisplayMode::ShowUnfiltered ) solid[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );

			context->set_rasterizer_state( wire );
			wireframe[FILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );
			if ( filter_mode == FilteredResultsDisplayMode::ShowUnfiltered ) wireframe[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );
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

			hal::rasterizer_state filled{ true, true, false };
			
			context->set_depth_state( hal::depth_state::read_only );
			context->set_blend_state( hal::blend_state::alpha );
			context->set_constant( glyph_effect, "global_material_data", "base_alpha", 1.f );
			context->set_rasterizer_state( filled );
			context->bind( glyph_effect );
			blended[FILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );
			wireframe_blended[FILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );
			auto filter_mode = scene->getFilterMode();
			if ( filter_mode == FilteredResultsDisplayMode::ShowUnfiltered )
			{
				blended[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );
				wireframe_blended[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );
			}

			if ( filter_mode == FilteredResultsDisplayMode::TranslucentUnfiltered )
			{
				context->set_constant( glyph_effect, "global_material_data", "base_alpha", filter_alpha );
				blended[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );
				solid[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );

				context->set_rasterizer_state( hal::rasterizer_state{ true, false, true } );
				context->set_constant( glyph_effect, "global_material_data", "base_alpha", wire_filter_alpha );
				wireframe[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );
				wireframe_blended[UNFILTERED].draw( context, transform_binding_point, material_binding_point, anim_binding_point );
				context->set_rasterizer_state( filled );
			}

			renderSelection( context, camera, elapsed_seconds );
		}
	}

	void GlyphRenderer::renderSelection( hal::context* context, render::perspective_camera* camera, float elapsed_seconds )
	{
		if ( scene && !scene->selectionEmpty() )
		{
			sel_transform_binding_point = context->get_uniform_block_index( selection_effect, "instance_data" );
			sel_bound_binding_point = context->get_uniform_block_index( selection_effect, "bounds" );
			sel_anim_binding_point = context->get_uniform_block_index( selection_effect, "animation" );

			if ( scene->getSelectionChanged() )
			{
				hal::debug::profile_timer timer;
				selection.clear();
				scene->enumSelected( [&]( const Glyph3DNode& glyph ) {
					auto glyph_transform = glyph.getCachedTransform() * glyph.getVisualTransform();
					auto model = glyph.getModel( 0.f /* todo: correct LOD */ );
					for ( auto part : model->get_parts() )
					{
						// for the selection effect we don't care about instance color, so we can pack the bound into that
						// constant buffer instead.
						selection.add_instance( part->get_mesh(), glyph_transform * model->get_transform() * part->get_transform(), glm::vec4( glyph.getCachedBound().get_center(), glyph.getCachedBound().get_radius() ), glyph.getAnimationAxis(), glyph.getAnimationRate() );
					}
				} );
				selection.update_instances( context );
				scene->clearSelectionChangedFlag();
				timer.print_ms_to_debug( "built selection instance buffers" );
			}

			context->set_depth_state( hal::depth_state::read_only );
			context->bind( selection_effect );
			hal::rasterizer_state rast{ true, true, false };
			context->set_rasterizer_state( rast );
			context->set_blend_state( hal::blend_state::alpha );

			context->set_constant( selection_effect, "global_data", "tint_color", render::color::yellow() );
			context->set_constant( selection_effect, "global_data", "elapsed_seconds", elapsed_seconds );
			context->set_constant( selection_effect, "global_data", "selection_anim_max_scale", selection_anim_max_scale );
			context->set_constant( selection_effect, "camera_data", "viewport", glm::vec2( camera->get_viewport_w(), camera->get_viewport_h() ) );
			context->set_constant( selection_effect, "camera_data", "view", camera->get_view() );
			context->set_constant( selection_effect, "camera_data", "proj", camera->get_proj() );
			context->set_constant( selection_effect, "camera_data", "eyevec", camera->get_eye() );
			context->set_constant( selection_effect, "camera_data", "upvec", camera->get_world_up() );

			selection.draw( context, sel_transform_binding_point, sel_bound_binding_point, sel_anim_binding_point );
		}
		else
		{
			selection.clear();
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
}
