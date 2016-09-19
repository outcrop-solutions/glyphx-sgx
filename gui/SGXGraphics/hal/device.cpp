
#include "device.h"
#include "internal/device_internal.h"

namespace SynGlyphX
{
	namespace hal
	{
		bool device::init()
		{
			return hal_gl::device_internal::init();
		}

		void device::shutdown()
		{
			hal_gl::device_internal::shutdown();
		}

		context* device::get_default_context()
		{
			return hal_gl::device_internal::get_default_context();
		}

		effect* device::load_effect( const char* vs_file, const char* gs_file, const char* ps_file )
		{
			return hal_gl::device_internal::load_effect( vs_file, gs_file, ps_file );
		}

		effect* device::create_effect( const char* vs, const char* gs, const char* ps )
		{
			return hal_gl::device_internal::create_effect( vs, gs, ps );
		}

		void device::addref( hal::mesh* m )
		{
			hal_gl::device_internal::addref( m );
		}

		void device::addref( hal::effect* e )
		{
			hal_gl::device_internal::addref( e );
		}

		void device::addref( hal::texture* t )
		{
			hal_gl::device_internal::addref( t );
		}

		void device::release( hal::effect* e )
		{
			return hal_gl::device_internal::release( e );
		}

		mesh* device::create_mesh( const vertex_format& fmt, const primitive_type prim, unsigned int vertex_count, void* vertices, unsigned int primitive_count, void* indices, bool support_readback )
		{
			return hal_gl::device_internal::create_mesh( fmt, prim, vertex_count, vertices, primitive_count, indices, support_readback );
		}

		cbuffer* device::create_cbuffer( unsigned int size )
		{
			return hal_gl::device_internal::create_cbuffer( size );
		}

		texture* device::create_texture( unsigned int w, unsigned int h, hal::texture_format fmt, uint8_t* data )
		{
			return hal_gl::device_internal::create_texture( w, h, fmt, data );
		}

		texture* device::load_texture( const char* file )
		{
			return hal_gl::device_internal::load_texture( file );
		}

		unsigned int device::get_texture_width( texture* t )
		{
			return hal_gl::device_internal::get_texture_width( t );
		}

		unsigned int device::get_texture_height( texture* t )
		{
			return hal_gl::device_internal::get_texture_height( t );
		}

		void device::release( mesh* m )
		{
			hal_gl::device_internal::release( m );
		}

		void device::release( cbuffer* c )
		{
			hal_gl::device_internal::release( c );
		}

		void device::release( texture* t )
		{
			hal_gl::device_internal::release( t );
		}

		void device::release( texture_array* t )
		{
			hal_gl::device_internal::release( t );
		}

		void device::release( font* f )
		{
			hal_gl::device_internal::release( f );
		}

		void device::set_break_on_error( bool val )
		{
			hal_gl::device_internal::set_break_on_error( val );
		}

		unsigned int device::maximum_constant_buffer_size()
		{
			return hal_gl::device_internal::maximum_constant_buffer_size();
		}

		void device::rebuild_effects()
		{
			hal_gl::device_internal::rebuild_effects();
		}

		void device::add_forced_effect_include( const char* filename )
		{
			hal_gl::device_internal::add_forced_effect_include( filename );
		}

		void device::set_cbuffer_external( effect* e, const char* block_name )
		{
			hal_gl::device_internal::set_cbuffer_external( e, block_name );
		}

		void device::set_cbuffer_usage( effect* e, const char* block_name, cbuffer_usage usage )
		{
			hal_gl::device_internal::set_cbuffer_usage( e, block_name, usage );
		}

		font* device::load_font( const char* file, unsigned size )
		{
			return hal_gl::device_internal::load_font( file, size );
		}

		texture_array* device::create_texture_array( unsigned int w, unsigned int h, unsigned int layers, texture_format fmt )
		{
			return hal_gl::device_internal::create_texture_array( w, h, layers, fmt );
		}

		void device::update_array_slice( texture_array* t, unsigned int layer, uint8_t* data )
		{
			pixel_rect rect{ 0u, 0u, t->w, t->h };
			return hal_gl::device_internal::update_array_slice( t, layer, rect, data );
		}

		void device::update_array_slice( texture_array* t, unsigned int layer, const pixel_rect& rect, uint8_t* data )
		{
			return hal_gl::device_internal::update_array_slice( t, layer, rect, data );
		}

		void device::begin_frame()
		{
		}

		void device::end_frame()
		{
			hal_gl::device_internal::end_frame();
		}
	}
}
