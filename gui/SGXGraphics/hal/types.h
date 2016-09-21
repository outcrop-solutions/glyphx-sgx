
#pragma once

#include <glm/glm.hpp>
#include "../platform.h"
#include "vertex_format.h"

namespace SynGlyphX
{
	namespace hal
	{
		class device;
		class context;

		class render_target_set;

		class texture;
		class texture_array;
		class mesh;
		class effect;

		class vertex_format;

		class cbuffer;

		class font;

		enum clear_type : unsigned int
		{
			color = 1u,
			depth = 2u,
			color_depth = 3u,
			stencil = 4u,
		};

		enum class depth_state : unsigned int
		{
			disabled,
			read_only,
			read_write,
		};

		enum class blend_state : unsigned int
		{
			disabled,
			alpha,
			premultiplied_alpha,
			additive,
		};

		enum class texture_wrap : unsigned int
		{
			wrap,
			clamp,
		};

		enum class texture_filter : unsigned int
		{
			point,
			linear,
			aniso,
		};

		enum class texture_format : unsigned int
		{
			r8,
			rgb8,
			rgba8,
			d24,
		};

		enum class primitive_type : unsigned int
		{
			triangle_list,
			line_list,
		};

		struct sampler_state
		{
			sampler_state( texture_wrap _wrap = texture_wrap::clamp, texture_filter _filter = texture_filter::linear )
				: wrap( _wrap ), filter( _filter ) { }
			texture_wrap wrap;
			texture_filter filter;
		};

		struct rasterizer_state
		{
			rasterizer_state( bool _multisample = true, bool _cull_backfaces = true, bool _wireframe = false, bool _depth_bias = false )
				: multisample( _multisample ), cull_backfaces( _cull_backfaces ), wireframe( _wireframe ), depth_bias( _depth_bias ) { }
			bool multisample;
			bool cull_backfaces;
			bool wireframe;
			bool depth_bias;
		};

		struct mesh_readback
		{
			mesh_readback( vertex_format& _fmt, char* _vertex_data, char* _index_data,
				unsigned int _index_count, unsigned int _vertex_count )
				: fmt( _fmt ), vertex_data( _vertex_data ), index_data( _index_data ),
				index_count( _index_count ), vertex_count( _vertex_count ) { }
			mesh_readback& operator=( const mesh_readback& ) = delete;
			vertex_format& fmt;
			char* vertex_data;
			char* index_data;
			unsigned int vertex_count;
			unsigned int index_count;
		};

		enum class cbuffer_usage
		{
			static_draw,
			dynamic_draw,
		};

		struct pixel_rect
		{
			int x, y, w, h;
		};
	}
}
