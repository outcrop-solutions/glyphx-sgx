
#pragma once
#ifdef __linux__
#include <stdint.h>
#include <unistd.h>
#endif
namespace SynGlyphX
{
#ifdef __linux__
	extern "C++" {
#endif
	template<typename T> inline T endian32( T v )
	{
		static_assert( sizeof( v ) == 4, "endian32 template requires 32-bit parameter type" );
		uint32_t val = *reinterpret_cast<uint32_t*>( &v );
		uint32_t result;
		result = ( val & 0x000000FF ) << 24;
		result |= ( val & 0x0000FF00 ) << 8;
		result |= ( val & 0x00FF0000 ) >> 8;
		result |= ( val & 0xFF000000 ) >> 24;
		return *reinterpret_cast<T*>( &result );
	}

	template<typename T> inline T endian16( T v )
	{
		static_assert( sizeof( v ) == 2, "endian16 template requires 32-bit parameter type" );
		char* in = reinterpret_cast<char*>( &v );
		char out[2];
		swab( in, out, 2 );
		return *reinterpret_cast<T*>( out );
	}
#ifdef __linux__
	}
#endif
}
