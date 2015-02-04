#include "uuid.h"

namespace SynGlyphX {

	boost::uuids::random_generator UUIDGenerator::s_randomUUIDGenerator;

	UUIDGenerator::UUIDGenerator() {

	}

	UUIDGenerator::~UUIDGenerator() {

	}

	boost::uuids::uuid UUIDGenerator::GetNewRandomUUID() {

		boost::uuids::uuid id = s_randomUUIDGenerator();
		return id;
	}

} //namespace SynGlyphX