
#pragma once

#include "types_internal.h"
#include <string>

namespace SynGlyphX
{
	namespace hal_gl
	{
		class device_internal
		{
		public:
			static bool init();
			static void shutdown();

			static hal::context* get_default_context();

			static hal::mesh* create_mesh( const hal::vertex_format& fmt, hal::primitive_type prim, unsigned int vertex_count, void* vertices, unsigned int primitive_count, void* indices, bool support_readback );
			static hal::effect* load_effect( const char* vs_file, const char* gs_file, const char* ps_file );
			static hal::effect* create_effect( const char* vs, const char* gs, const char* ps );
			static hal::cbuffer* create_cbuffer( unsigned int size );

			static void add_forced_effect_include( const char* filename );
			static void set_cbuffer_external( hal::effect* e, const char* block_name );
			static void set_cbuffer_usage( hal::effect* e, const char* block_name, hal::cbuffer_usage usage );

			static void rebuild_effects();

			static hal::texture* create_texture( unsigned int w, unsigned int h, hal::texture_format fmt, uint8_t* data );
			static hal::texture* load_texture( const char* file );
			static unsigned int get_texture_width( hal::texture* tex ) { return tex->w; }
			static unsigned int get_texture_height( hal::texture* tex ) { return tex->h; }

			static hal::font* load_font( const char* file, unsigned int size );

			static void addref( hal::mesh* m );
			static void addref( hal::effect* e );
			static void addref( hal::texture* t );

			static void release( hal::mesh* m );
			static void release( hal::effect* e );
			static void release( hal::cbuffer* c );
			static void release( hal::texture* t );
			static void release( hal::font* f );

			static unsigned int maximum_constant_buffer_size();

			static void set_break_on_error( bool val );

			// INTERNAL ONLY (not exposed in hal::device)
			struct font_cache_entry
			{
				std::vector<glm::mat4> transforms;
				std::vector<hal::mesh*> meshes;
			};
			static hal::effect* get_text_effect();
			static const hal::font_glyph& get_glyph( hal::font* f, char c );
			static const glm::vec2 get_kerning( hal::font* f, char left, char right );
		};
	}
}

