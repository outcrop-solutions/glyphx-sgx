#ifndef SYNGLYPHX_UUIDHASH_H
#define SYNGLYPHX_UUIDHASH_H

//This is necessary because including boost::uuid causes this warning to be elevated to an error
#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "sgxfoundation.h"
#include <boost/uuid/uuid.hpp>
//#include <boost/uuid/uuid_io.hpp>
//#include <string>
#include <boost/uuid/uuid_generators.hpp>

namespace SynGlyphX {

	//Hash function for boost uuid so that it can be used in STL classes like unordered_map
	struct UUIDHash {
		std::size_t operator()(const boost::uuids::uuid& uuid) const
		{
			return boost::uuids::hash_value(uuid);
		}
	};

	class SGXFOUNDATION_API UUIDGenerator {

	public:
		UUIDGenerator();
		~UUIDGenerator();

		static boost::uuids::uuid GetNewRandomUUID();

	private:
		static boost::uuids::random_generator s_randomUUIDGenerator;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_UUID_H