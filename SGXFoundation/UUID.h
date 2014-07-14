#ifndef SYNGLYPHX_UUID_H
#define SYNGLYPHX_UUID_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>

namespace SynGlyphX {

	//Hash function for boost uuid so that it can be used in STL classes like unordered_map
	struct UUIDHash {
		std::size_t operator()(const boost::uuids::uuid& uuid) const
		{
			return boost::uuids::hash_value(uuid);
		}
	};

} //namespace SynGlyphX

#endif SYNGLYPHX_UUID_H