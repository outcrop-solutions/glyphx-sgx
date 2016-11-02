
#include "uuid.h"
#include <boost/uuid/uuid_generators.hpp>

namespace SynGlyphX {

	namespace
	{
		boost::uuids::random_generator s_randomUUIDGenerator;
	}

	UUIDGenerator::UUIDGenerator() {

	}

	UUIDGenerator::~UUIDGenerator() {

	}

	boost::uuids::uuid UUIDGenerator::GetNewRandomUUID() {

		boost::uuids::uuid id = s_randomUUIDGenerator();
		return id;
	}

} //namespace SynGlyphX
