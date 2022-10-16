
#include "baseimagerenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <hal/hal.h>
#include <render/model.h>
#include "AwsLogger.h"

namespace SynGlyphX
{
	namespace
	{
		float square_vertices[]
		{
			-0.5f, -0.5f, 0.f, 0.f, 1.f,
			 0.5f, -0.5f, 0.f, 1.f, 1.f,
			 0.5f,  0.5f, 0.f, 1.f, 0.f,
			-0.5f,  0.5f, 0.f, 0.f, 0.f,
		};
	}

	BaseImageRenderer::BaseImageRenderer()
	{
		effect = hal::device::load_effect( "shaders/texture.vert", nullptr, "shaders/texture.frag" );

		// Build a unit square mesh, which we'll transform as needed to form the base images.
		unsigned int square_indices[]
		{
			0u, 1u, 2u, 0u, 2u, 3u
		};

		hal::vertex_format fmt;
		fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3, hal::stream_semantic::position, 0 ) );
		fmt.add_stream( hal::stream_info( hal::stream_type::float32, 2, hal::stream_semantic::texcoord, 0 ) );

		plane = hal::device::create_mesh( fmt, hal::primitive_type::triangle_list, 4, square_vertices, 2, square_indices );
	}

	BaseImageRenderer::~BaseImageRenderer()
	{
		clear();
		hal::device::release( plane );
		hal::device::release( effect );
	}

	void BaseImageRenderer::add( hal::texture* _texture, const glm::mat4& transform, const glm::vec2& dimensions )
	{
		try {
			hal::device::addref(_texture);

			glm::mat4 t = transform * glm::scale(glm::mat4(), glm::vec3(dimensions, 1.f));

			render::model_part* part = new render::model_part(plane, t, "base image");

			base_image_info info;
			info.texture = _texture;
			info.model = new render::model();
			info.model->add_part(part);
			info.bound = render::box_bound::from_points(reinterpret_cast<glm::vec3*>(square_vertices), sizeof(square_vertices) / 4u, 4u, t);
			info.enabled = true;

			base_images.push_back(info);
		}
		catch (const std::exception& e) {
			AwsLogger::getInstance()->localLogger(QString(e.what()));
		}
	}

	void BaseImageRenderer::clear()
	{
		for ( auto i : base_images )
		{
			hal::device::release( i.texture );
			delete i.model;
		}
		base_images.clear();
	}

	void BaseImageRenderer::draw( hal::context* context, render::camera* camera )
	{
		context->set_depth_state( hal::depth_state::read_write );
		hal::rasterizer_state rast{ true, true, false, true };
		context->set_rasterizer_state( rast );
		context->set_blend_state( hal::blend_state::premultiplied_alpha );
		hal::sampler_state sampler( hal::texture_wrap::clamp, hal::texture_filter::aniso );
		for ( auto i : base_images )
		{
			if ( i.enabled )
			{
				context->bind( 0, i.texture, sampler );
				add_batch( i.model, effect );
				render( context, camera );
			}
		}
	}

	render::box_bound BaseImageRenderer::get_base_image_bound( unsigned int idx )
	{
		if ( idx < base_images.size() )
			return base_images[idx].bound;
		else
			return render::box_bound( glm::vec3(), glm::vec3() );
	}
}
