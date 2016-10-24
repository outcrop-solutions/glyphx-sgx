
#include "axisrenderer.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <hal/device.h>
#include <render/model.h>
#include <render/model_loader.h>

namespace SynGlyphX
{
	AxisRenderer::AxisRenderer() : body_scale( 1.f ), head_scale( glm::vec3( 2.f, 2.f, 2.f ) )
	{
		effect = hal::device::load_effect( "shaders/solid.vert", nullptr, "shaders/solid.frag" );
		axis_body = render::load_model( "meshes/axis_cylinder.dae" );
		axis_head = render::load_model( "meshes/axis_cone.dae" );
	}

	AxisRenderer::~AxisRenderer()
	{
		delete axis_head;
		delete axis_body;
		hal::device::release( effect );
	}

	void AxisRenderer::set_scales( float _body_scale, const glm::vec3& _head_scale )
	{
		body_scale = _body_scale;

		// swizzle because it feels more intuitive to have scale.z = the length of the arrowhead
		head_scale = glm::vec3( _head_scale.x, _head_scale.z, _head_scale.y );
	}

	void AxisRenderer::draw_axis( hal::context* context, render::camera* camera, const glm::vec4& color, AxisDirection direction, const glm::vec3& origin, float length, const glm::mat4& base_rotation, bool bidirectional )
	{
		glm::mat4 rotate;
		switch ( direction )
		{
			case AxisDirection::X: rotate = base_rotation * glm::rotate( glm::mat4(), -glm::half_pi<float>(), glm::vec3( 0.f, 0.f, 1.f ) ); break;
			case AxisDirection::Y: rotate = base_rotation; break;
			case AxisDirection::Z: rotate = base_rotation * glm::rotate( glm::mat4(), glm::half_pi<float>(), glm::vec3( 1.f, 0.f, 0.f ) ); break;
		}

		draw_axis( context, camera, color, rotate, origin, length );

		if ( bidirectional )
		{
			switch ( direction )
			{
				case AxisDirection::X: rotate = base_rotation * glm::rotate( glm::mat4(), glm::half_pi<float>(), glm::vec3( 0.f, 0.f, 1.f ) ); break;
				case AxisDirection::Y: rotate = base_rotation * glm::rotate( glm::mat4(), -glm::pi<float>(), glm::vec3( 1.f, 0.f, 0.f ) ); break;
				case AxisDirection::Z: rotate = base_rotation * glm::rotate( glm::mat4(), -glm::half_pi<float>(), glm::vec3( 1.f, 0.f, 0.f ) ); break;
			}
			draw_axis( context, camera, color, rotate, origin, length );
		}
	}

	void AxisRenderer::draw_axis( hal::context* context, render::camera* camera, const glm::vec4& color, const glm::mat4& rotate, const glm::vec3& origin, float length )
	{
		auto axes_translate = glm::translate( glm::mat4(), origin );
		add_batch( axis_body, effect, axes_translate * rotate * glm::scale( glm::mat4(), glm::vec3( body_scale, length, body_scale ) ), color );
		add_batch( axis_head, effect, axes_translate * rotate * glm::translate( glm::mat4(), glm::vec3( 0.f, length, 0.f ) )* glm::scale( glm::mat4(), head_scale ), color );
		render( context, camera );
	}
}
