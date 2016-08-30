
#pragma once

#include <render/renderer.h>

namespace SynGlyphX
{
	namespace render { class model; }

	enum class AxisDirection : unsigned int
	{
		X,
		Y,
		Z,
	};

	class AxisRenderer : protected render::renderer
	{
	public:
		AxisRenderer();
		~AxisRenderer() override;
		AxisRenderer( const AxisRenderer& ) = delete;

		void set_scales( float body_scale, const glm::vec3& head_scale );

		void draw_axis( hal::context* context, render::camera* camera, const glm::vec4& color, AxisDirection direction, const glm::vec3& origin, float length, const glm::mat4& base_rotation = glm::mat4(), bool bidirectional = false );

	private:
		void draw_axis( hal::context* context, render::camera* camera, const glm::vec4& color, const glm::mat4& rotate, const glm::vec3& origin, float length );

		render::model* axis_head;
		render::model* axis_body;
		hal::effect* effect;
		float body_scale;
		glm::vec3 head_scale;
	};
}
