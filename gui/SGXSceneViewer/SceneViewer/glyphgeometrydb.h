
#pragma once

#include "../platform.h"
#include "types.h"

namespace SynGlyphX
{
	class GlyphGeometryDB
	{
	public:
		static void init();
		static void reset();	// call when loading new vis
		static void clear();
		static render::model* get( GlyphShape type, float torus_ratio = 0.1f, float dist = FLT_MAX );
	private:
		struct model_entry
		{
			render::model* model;
			float lod;
		};
		static std::unordered_map<GlyphShape, std::vector<model_entry>> models;
		static std::unordered_map<float, std::vector<model_entry>> torus_models;	// indexed by ratio
		static void add_model( GlyphShape type, const char* filename, float lod, const glm::mat4& base_transform = glm::mat4() );
		static void add_model( GlyphShape type, render::model* model, float lod );
		static void add_torus_model( float ratio, render::model* model, float lod );
		static render::model* get_torus( float ratio, float dist );
		static render::model* create_pin();
	};
}
