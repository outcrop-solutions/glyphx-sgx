
#pragma once

#include <vector>
#include "../platform.h"

namespace SynGlyphX
{
	namespace hal
	{
		enum class stream_type
		{
			float32,
		};

		enum class stream_semantic
		{
			position,
			normal,
			tangent,
			bitangent,
			texcoord,
			color,
		};

		struct SGXGRAPHICS_API stream_info
		{
			stream_info() { }
			stream_info( const stream_type& data_type, const unsigned int& count, const stream_semantic& semantic, const unsigned int& index );
			stream_type data_type;
			unsigned int count;
			stream_semantic semantic;
			unsigned int index;
		};

		enum class index_format
		{
			uint16,
			uint32,
		};

		class SGXGRAPHICS_API vertex_format
		{
		public:
			bool operator==( const vertex_format& vf ) const;

			void add_stream( const stream_info& info );

			unsigned int vertex_size_in_bytes() const;
			unsigned int stream_count() const;
			const stream_info& get_stream_info( const unsigned int& stream_index ) const;
			unsigned int element_offset_in_bytes( const stream_semantic& semantic, unsigned int index ) const;
			static unsigned int element_size_in_bytes( const stream_type& st );

		private:
			std::vector< stream_info > streams;
		};
	}
}
