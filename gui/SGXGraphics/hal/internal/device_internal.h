
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

			static void set_external_default_render_target( unsigned int rt );

			static void end_frame();

			static hal::context* get_default_context();
			static hal::context* get_new_context();
			static void release_context( hal::context* c );

			static hal::render_target_set* create_render_target_set( unsigned int w, unsigned int h );
			static unsigned int add_color_target( hal::render_target_set* set, hal::texture_format fmt );
			static void add_depth_target( hal::render_target_set* set, hal::texture_format fmt );
			static hal::texture* get_target_texture( hal::render_target_set* set, unsigned int index );
			static hal::texture* get_target_depth_texture( hal::render_target_set* set );

			static hal::mesh* create_mesh( const hal::vertex_format& fmt, hal::primitive_type prim, unsigned int vertex_count, void* vertices, unsigned int primitive_count, void* indices, bool support_readback );
			static hal::effect* load_effect( const char* vs_file, const char* gs_file, const char* ps_file );
			static hal::effect* create_effect( const char* vs, const char* gs, const char* ps );
			static hal::cbuffer* create_cbuffer( unsigned int size );

			static void add_forced_effect_include( const char* filename );
			static void set_cbuffer_external( hal::effect* e, const char* block_name );
			static void set_cbuffer_usage( hal::effect* e, const char* block_name, hal::cbuffer_usage usage );

			static void rebuild_effects();

			static hal::texture* create_texture( unsigned int w, unsigned int h, hal::texture_format fmt, uint8_t* data );
			static hal::texture* load_texture( const char* file, bool generate_mips );
			static unsigned int get_texture_width( hal::texture* tex ) { return tex->w; }
			static unsigned int get_texture_height( hal::texture* tex ) { return tex->h; }

			static hal::texture_array* create_texture_array( unsigned int w, unsigned int h, unsigned int layers, hal::texture_format fmt );
			static void update_array_slice( hal::texture_array* t, unsigned int layer, const hal::pixel_rect& rect, uint8_t* data );
            static void clear_array_slice( hal::texture_array* t, unsigned int layer );

			static hal::font* load_font( const char* file, unsigned int size );

			static void addref( hal::render_target_set* r );
			static void addref( hal::mesh* m );
			static void addref( hal::effect* e );
			static void addref( hal::texture* t );

			static void release( hal::render_target_set* r );
			static void release( hal::mesh* m );
			static void release( hal::effect* e );
			static void release( hal::cbuffer* c );
			static void release( hal::texture* t );
			static void release( hal::texture_array* t );
			static void release( hal::font* f );

			static unsigned int maximum_constant_buffer_size();

			static void set_break_on_error( bool val );

			static hal::effect* get_text_effect();
			static uint64_t current_frame();
			static GLuint get_default_render_target();
		};
	}
}

