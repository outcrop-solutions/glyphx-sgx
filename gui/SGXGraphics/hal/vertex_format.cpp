
#include "vertex_format.h"

namespace SynGlyphX
{
	namespace hal
	{
		stream_info::stream_info( const stream_type& _data_type, const unsigned int& _count,
			const stream_semantic& _semantic, const unsigned int& _index )
			: data_type( _data_type ), count( _count ), semantic( _semantic ), index( _index )
		{
		}

		void vertex_format::add_stream( const stream_info& info )
		{
			streams.push_back( info );
		}

		const stream_info& vertex_format::get_stream_info( const unsigned int& stream_index ) const 
		{
			return streams[stream_index];
		}

		unsigned int vertex_format::stream_count() const
		{
			return static_cast<unsigned int>( streams.size() );
		}

		unsigned int vertex_format::vertex_size_in_bytes() const
		{
			unsigned int size = 0;
			for ( unsigned int i = 0; i < streams.size(); ++i )
			{
				const stream_info& stream = streams[i];
				unsigned int entrysize = element_size_in_bytes( stream.data_type );
				size += ( entrysize * stream.count );
			}

			return size;
		}

		unsigned int vertex_format::element_size_in_bytes( const stream_type& Type )
		{
			switch ( Type )
			{
				case stream_type::float32:
					return 4;
				default:
					assert( false );
					return  0;
			}
		}

		unsigned int vertex_format::element_offset_in_bytes( const stream_semantic& semantic, const unsigned int index ) const
		{
			unsigned int offset = 0;
			bool bfound = false;
			for ( unsigned int i = 0; i < static_cast<unsigned int>( streams.size() ); ++i )
			{
				const stream_info& stream = streams[i];
				if ( stream.semantic == semantic && stream.index == index )
				{
					bfound = true;
					break;
				}
				else
				{
					offset += element_size_in_bytes( stream.data_type ) * stream.count;
				}
			}
			assert( bfound );
			return offset;
		}

		bool vertex_format::operator==( const vertex_format& vf ) const
		{
			if ( streams.size() != vf.streams.size() )
				return false;

			for ( unsigned int i = 0; i < streams.size(); ++i )
			{
				if ( streams[i].data_type != vf.streams[i].data_type )
					return false;
				if ( streams[i].count != vf.streams[i].count )
					return false;
				if ( streams[i].semantic != vf.streams[i].semantic )
					return false;
				if ( streams[i].index != vf.streams[i].index )
					return false;
			}

			return true;
		}
	}
};