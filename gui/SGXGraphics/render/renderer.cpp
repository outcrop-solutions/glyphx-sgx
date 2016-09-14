
#include "renderer.h"
#include <hal/context.h>
#include "model.h"
#include "camera.h"

namespace SynGlyphX
{
	namespace render
	{
		renderer::~renderer()
		{
			// Shouldn't have batches when the render destructs.
			assert( batches.size() == 0u );
		}

		void renderer::add_batch( model* m, hal::effect* e, const glm::mat4& transform, const glm::vec4& tint_color )
		{
			batch b;
			b.m = m;
			b.e = e;
			b.transform = transform;
			b.tint_color = tint_color;

			if ( batches.find( e ) == batches.end() )
				batches.insert( std::make_pair( e, batch_list() ) );

			batch_list& bl = batches[e];

			bl.push_back( b );
		}

		void renderer::add_blended_batch( model* m, hal::effect* e, const glm::mat4& transform, const glm::vec4& tint_color )
		{
			batch b;
			b.m = m;
			b.e = e;
			b.transform = transform;
			b.tint_color = tint_color;

			if ( blended_batches.find( e ) == blended_batches.end() )
				blended_batches.insert( std::make_pair( e, batch_list() ) );

			batch_list& bl = blended_batches[e];

			bl.push_back( b );
		}

		void renderer::render( hal::context* context, camera* _camera )
		{
			context->set_depth_state( hal::depth_state::read_write );
			context->set_blend_state( hal::blend_state::disabled );
			render_batches( context, _camera, batches );

			context->set_depth_state( hal::depth_state::read_only );
			context->set_blend_state( hal::blend_state::alpha );
			render_batches( context, _camera, blended_batches );
		}

		void renderer::render_batches( hal::context* context, camera* _camera, batch_set& b )
		{
			for ( auto& bl : b )
			{
				hal::effect* e = bl.first;
				context->bind( e );

				context->set_constant( e, "camera_data", "view", _camera->get_view() );
				context->set_constant( e, "camera_data", "proj", _camera->get_proj() );
				context->set_constant( e, "camera_data", "camera_pos", _camera->get_position() );

				for ( auto& b : bl.second )
				{
					const auto& parts = b.m->get_parts();
					auto transform = b.transform * b.m->get_transform();
					for ( auto part : parts )
					{
						context->set_constant( b.e, "material", "tint_color", b.tint_color );
						context->set_constant( b.e, "instance_data", "world", transform * part->get_transform() );
						context->draw( part->get_mesh() );
					}
				}
			}

			b.clear();
		}
	}
}
