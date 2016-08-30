
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

		texture* device::register_external_texture( void* texture_data )
		{
			return hal_gl::device_internal::register_external_texture( texture_data );
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

		void device::set_uniform_block_external( effect* e, const char* block_name )
		{
			hal_gl::device_internal::set_uniform_block_external( e, block_name );
		}
	}
}
