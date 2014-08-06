#ifndef SYNGLYPHX_DOWNLOADEDMAPPROPERTIES_H
#define SYNGLYPHX_DOWNLOADEDMAPPROPERTIES_H

#include "baseimageproperties.h"
#include <array>
#include "sgxdatatransform_global.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DownloadedMapProperties : public BaseImageProperties
	{
	public:
		typedef std::array<unsigned int, 2> Size;

		enum MapSource {
			MapQuestOpen, //MapQuestOpen gets data from OpenStreetMap
			GoogleMaps
		};

		enum MapType {
			Map,
			Satellite,
			Hybrid
		};

		DownloadedMapProperties(MapSource source, MapType type, Size size = { { 2048, 1024 } });
		DownloadedMapProperties(const DownloadedMapProperties& properties);
		virtual ~DownloadedMapProperties();

		//virtual bool IsGeographic() const;

		MapSource GetSource() const;
		MapType GetType() const;
		Size GetSize() const;

	private:
		MapSource m_source;
		MapType m_type;
		Size m_size;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DOWNLOADEDMAPPROPERTIES_H