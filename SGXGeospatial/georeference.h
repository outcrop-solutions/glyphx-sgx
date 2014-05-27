#ifndef GEOREFERENCE_H
#define GEOREFERENCE_H

#include "sgxggeospatial.h"
#include <string>
#include "geographicboundingbox.h"

namespace SynGlyphX {

	class SGXGGEOSPATIAL_API GeoReference
	{
	public:
		GeoReference();
		~GeoReference();

		static GeoReference& Instance();

		void GeoReferenceImage(const std::string& inputFilename, const std::string& outputFilename, const GeographicBoundingBox& boundingBox) const;

	private:
		static GeoReference s_instance;

	};

} //namespace SynGlyphX

#endif //GEOREFERENCE_H