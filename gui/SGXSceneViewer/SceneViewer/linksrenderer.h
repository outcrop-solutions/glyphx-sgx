
#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <render/renderer.h>
#include "types.h"

namespace SynGlyphX
{
	class Glyph3DNode;
	class GlyphScene;

	enum class LinkProfile : unsigned int
	{
		Circle,
		Square,
	};

	class LinksRenderer : protected render::renderer
	{
	public:
		LinksRenderer();
		~LinksRenderer() override;
		LinksRenderer( const LinksRenderer& ) = delete;

		void add( const Glyph3DNode* glyph0, const Glyph3DNode* glyph1, const glm::vec4& color, float thickness, LinkProfile profile );
		void clear();

		void draw( hal::context* context, render::camera* camera, const GlyphScene& scene, FilteredResultsDisplayMode filter_mode );

	private:
		struct link_info
		{
			const Glyph3DNode* glyph0;
			const Glyph3DNode* glyph1;
			glm::vec4 color;
			float thickness;
			LinkProfile profile;
		};
		std::vector<link_info> links;

		render::model* cube;
		render::model* cylinder;
		hal::effect* effect;

		render::model* get_model( LinkProfile profile );
	};
}
