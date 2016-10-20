
#pragma once

#include "../platform.h"
#include <vector>
#include <unordered_map>
#include <cfloat>
#include <glm/glm.hpp>
#include "types.h"

namespace SynGlyphX
{
    namespace render { class model; }
    
	class GlyphGeometryDB
	{
	public:
		GlyphGeometryDB() { };
		GlyphGeometryDB( const GlyphGeometryDB& ) = delete;

		void init();
		void reset();	// call when loading new vis
		void clear();
		render::model* get( GlyphShape type, float torus_ratio = 0.1f, float dist = FLT_MAX ) const;
	private:
		struct model_entry
		{
			render::model* model;
			float lod;
		};
		void add_model( GlyphShape type, const char* filename, float lod, const glm::mat4& base_transform = glm::mat4() );
		void add_model( GlyphShape type, render::model* model, float lod );
		void add_torus_model( float ratio, render::model* model, float lod ) const;
		render::model* get_torus( float ratio, float dist ) const;
		render::model* create_pin();

		mutable std::unordered_map<GlyphShape, std::vector<GlyphGeometryDB::model_entry>> models;
		mutable std::unordered_map<float, std::vector<GlyphGeometryDB::model_entry>> torus_models;
	};
}

