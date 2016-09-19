
#pragma once

#include <string>
#include "../platform.h"
#include "types.h"

namespace SynGlyphX
{
	namespace hal
	{
		class SGXGRAPHICS_API device
		{
		public:
			static bool init();
			static void shutdown();

			static void begin_frame();
			static void end_frame();

			static context* get_default_context();

			static mesh* create_mesh( const vertex_format& fmt, primitive_type prim, unsigned int vertex_count, void* vertices, unsigned int primitive_count, void* indices, bool support_readback = false );
			static effect* load_effect( const char* vs_file, const char* gs_file, const char* ps_file );
			static effect* create_effect( const char* vs, const char* gs, const char* ps );
			static cbuffer* create_cbuffer( unsigned int size );

			// Basic workaround for allowing shared shader code since unlike other shader languages GLSL does not support #include.
			// A more robust solution would be to write a GLSL preprocessor to handle #includes, but that's probably not necessary
			// unless our needs become much more complex.
			static void add_forced_effect_include( const char* filename );

			static void set_cbuffer_external( effect* e, const char* block_name );	// see context.h for explanation
			static void set_cbuffer_usage( effect* e, const char* block_name, cbuffer_usage usage );

			static texture* create_texture( unsigned int w, unsigned int h, texture_format fmt, uint8_t* data );
			static texture* load_texture( const char* file );
			static unsigned int get_texture_width( texture* t );
			static unsigned int get_texture_height( texture* t );

			static texture_array* create_texture_array( unsigned int w, unsigned int h, unsigned int layers, texture_format fmt );
			static void update_array_slice( texture_array* t, unsigned int layer, uint8_t* data );
			static void update_array_slice( texture_array* t, unsigned int layer, const pixel_rect& rect, uint8_t* data );

			static font* load_font( const char* file, unsigned int size );

			static void rebuild_effects();

			static void addref( mesh* m );
			static void addref( effect* m );
			static void addref( texture* t );

			static void release( mesh* m );
			static void release( effect* e );
			static void release( cbuffer* c );
			static void release( texture* t );
			static void release( texture_array* t );
			static void release( font* f );

			static unsigned int maximum_constant_buffer_size();

			static void set_break_on_error( bool val );
		};
	}
}
