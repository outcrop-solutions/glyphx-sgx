
#pragma once

#include "../platform.h"
#include <vector>
#include <hal/types.h>
#include <render/renderer.h>

namespace SynGlyphX
{
	namespace render { class camera; }

	class BaseImageRenderer : protected render::renderer
	{
	public:
		BaseImageRenderer();
		~BaseImageRenderer() override;
		BaseImageRenderer( const BaseImageRenderer& ) = delete;

		void add( hal::texture* texture, const glm::mat4& transform, const glm::vec2& dimensions );
		void clear();

		void draw( hal::context* context, render::camera* camera );

		render::box_bound get_base_image_bound( unsigned int idx );

	private:
		struct base_image_info
		{
			hal::texture* texture;
			render::model* model;
			render::box_bound bound;
			bool enabled;
		};
		std::vector<base_image_info> base_images;

		hal::mesh* plane;
		hal::effect* effect;
	};
}
