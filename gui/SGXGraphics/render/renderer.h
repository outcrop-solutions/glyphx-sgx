
#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "../platform.h"
#include "color.h"

namespace SynGlyphX
{
	namespace hal { class context; class effect; }

	namespace render
	{
		class model;
		class camera;

		class SGXGRAPHICS_API renderer
		{
		public:
			renderer() : wireframe( false ) { }
			renderer( const renderer& ) = delete;
			virtual ~renderer();
			void add_batch( model* m, hal::effect* e, const glm::mat4& transform = glm::mat4(), const glm::vec4& tint_color = color::white() );
			void add_blended_batch( model* m, hal::effect* e, const glm::mat4& transform = glm::mat4(), const glm::vec4& tint_color = color::white() );
			void render( hal::context* context, camera* _camera );
			void set_wireframe( bool val ) { wireframe = val; }

		private:
			struct batch
			{
				glm::mat4 transform;
				hal::effect* e;
				model* m;
				glm::vec4 tint_color;
			};
			typedef std::vector<batch> batch_list;
			typedef std::unordered_map<hal::effect*, batch_list> batch_set;
			batch_set batches;
			batch_set blended_batches;

			bool wireframe;

			void render_batches( hal::context* context, camera* _camera, batch_set& b );
		};
	}
}
