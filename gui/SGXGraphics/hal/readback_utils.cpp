
#include "readback_utils.h"

namespace SynGlyphX
{
	namespace hal
	{
		void for_each_triangle( const mesh_readback& m, std::function<void( const glm::vec3&, const glm::vec3&, const glm::vec3& )> fn )
		{
			unsigned int* indices = reinterpret_cast<unsigned int*>( m.index_data );
			unsigned int triangle_count = m.index_count / 3u;
			for ( unsigned int face = 0; face < triangle_count; ++face )
			{
				unsigned int i0 = indices[face * 3 + 0];
				unsigned int i1 = indices[face * 3 + 1];
				unsigned int i2 = indices[face * 3 + 2];

				auto get_pos = [=]( unsigned int i ) {
					return reinterpret_cast<glm::vec3*>( m.vertex_data + ( i * m.fmt.vertex_size_in_bytes() ) + m.fmt.element_offset_in_bytes( hal::stream_semantic::position, 0u ) );
				};

				auto pos0 = get_pos( i0 ), pos1 = get_pos( i1 ), pos2 = get_pos( i2 );
				fn( *pos0, *pos1, *pos2 );
			}
		}
	}
}
