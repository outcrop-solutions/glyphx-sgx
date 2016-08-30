
#include "linksrenderer.h"
#include <glm/gtx/rotate_vector.hpp>
#include <hal/hal.h>
#include <render/model_loader.h>
#include "glyphnode.h"
#include "glyphscene.h"

namespace SynGlyphX
{
	LinksRenderer::LinksRenderer()
	{
		effect = hal::device::create_effect( "shaders/solid.vert", nullptr, "shaders/solid.frag" );
		cylinder = render::load_model( "meshes/link_cylinder.dae" );
		cylinder->set_transform( glm::rotate( cylinder->get_transform(), glm::half_pi<float>(), glm::vec3( 1.f, 0.f, 0.f ) ) );
		cube = render::load_model( "meshes/link_cube.dae" );
	}

	LinksRenderer::~LinksRenderer()
	{
		hal::device::release( effect );
		delete cylinder;
		delete cube;
	}

	void LinksRenderer::add( const Glyph3DNode* glyph0, const Glyph3DNode* glyph1, const glm::vec4& color, float thickness, LinkProfile profile )
	{
		link_info info;
		info.glyph0 = glyph0;
		info.glyph1 = glyph1;
		info.color = color;
		info.thickness = thickness;
		info.profile = profile;
		links.push_back( info );
	}

	void LinksRenderer::clear()
	{
		links.clear();
	}

	render::model* LinksRenderer::get_model( LinkProfile profile )
	{
		switch ( profile )
		{
			case LinkProfile::Circle: return cylinder;
			case LinkProfile::Square: return cube;
		}
		return nullptr;
	}

	void LinksRenderer::draw( hal::context* context, render::camera* camera, const GlyphScene& scene, FilteredResultsDisplayMode filter_mode )
	{
		for ( const auto& info : links )
		{
			auto pt0 = info.glyph0->getCachedPosition();
			auto pt1 = info.glyph1->getCachedPosition();

			bool passed_filter = !scene.filterApplied() || ( scene.isFiltered( info.glyph0 ) || scene.isFiltered( info.glyph1 ) );

			if ( passed_filter || filter_mode != FilteredResultsDisplayMode::HideUnfiltered )
			{
				glm::vec3 origin = ( pt0 + pt1 ) * 0.5f;
				auto translate = glm::translate( glm::mat4(), origin );
				auto rotate = glm::orientation( glm::normalize( pt1 - pt0 ), glm::vec3( 0.f, 0.f, 1.f ) );
				float length = glm::length( pt0 - pt1 );
				auto scale = glm::scale( glm::mat4(), glm::vec3( info.thickness, info.thickness, length ) );

				glm::vec4 color = info.color;
				if ( !passed_filter && filter_mode == FilteredResultsDisplayMode::TranslucentUnfiltered )
					color.a *= 0.5f;

				if ( color.a == 1.f )
					add_batch( get_model( info.profile ), effect, translate * rotate * scale, color );
				else
					add_blended_batch( get_model( info.profile ), effect, translate * rotate * scale, color );
			}
		}

		render( context, camera );
	}
}
