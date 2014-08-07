#ifndef SYNGLYPHX_DOWNLOADEDMAPPROPERTIES_H
#define SYNGLYPHX_DOWNLOADEDMAPPROPERTIES_H

#include "baseimageproperties.h"
#include <array>
#include "sgxdatatransform_global.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/bimap.hpp>

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
		DownloadedMapProperties(const boost::property_tree::wptree& propertyTree);
		DownloadedMapProperties(const DownloadedMapProperties& properties);
		virtual ~DownloadedMapProperties();

		//virtual bool IsGeographic() const;

		MapSource GetSource() const;
		MapType GetType() const;
		Size GetSize() const;

		virtual void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		static const boost::bimap<MapSource, std::wstring> s_mapSourceStrings;
		static const boost::bimap<MapType, std::wstring> s_mapTypeStrings;

	private:
		MapSource m_source;
		MapType m_type;
		Size m_size;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DOWNLOADEDMAPPROPERTIES_H