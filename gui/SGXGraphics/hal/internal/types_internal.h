
#pragma once

#include "gl.h"
#include "../types.h"
#include "ref_counted.h"
#include "effect.h"
#include "../vertex_format.h"

namespace SynGlyphX
{
	namespace hal
	{
		class texture : public ref_counted
		{
		public:
			GLuint handle;
			unsigned int w, h;
			bool external;
		};

		class mesh : public ref_counted
		{
		public:
			GLuint vao, vbo, ebo;
			unsigned int vertex_count, primitive_count;
			char* vertex_data_copy;	// main memory copy of data for quick readback (optional)
			char* index_data_copy;
			vertex_format fmt;
			primitive_type prim;
		};

		class cbuffer : public ref_counted
		{
		public:
			GLuint buffer;
			unsigned int size;
		};

		inline unsigned int indices_per_primitive( primitive_type prim )
		{
			switch ( prim )
			{
				case primitive_type::triangle_list:
					return 3u;
				case primitive_type::line_list:
					return 2u;
			}
			assert( false );	// invalid prim type
			return 0u;
		}

		inline GLenum gl_prim_type( primitive_type prim )
		{
			switch ( prim )
			{
				case primitive_type::triangle_list:
					return GL_TRIANGLES;
				case primitive_type::line_list:
					return GL_LINES;
			}
			assert( false );	// invalid prim type
			return 0u;
		}
	}
}
