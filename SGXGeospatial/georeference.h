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

        //Mapquest images are in spherical mercator so default to that geo reference
        void GeoReferenceImage(const std::string& inputFilename, const std::string& outputFilename, const GeographicBoundingBox& boundingBox, const std::string& spatialRef = "EPSG:4326") const;

	private:
		static GeoReference s_instance;

	};

} //namespace SynGlyphX

#endif //GEOREFERENCE_H