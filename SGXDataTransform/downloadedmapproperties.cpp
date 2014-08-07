#include "downloadedmapproperties.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const boost::bimap<DownloadedMapProperties::MapSource, std::wstring> DownloadedMapProperties::s_mapSourceStrings = boost::assign::list_of < boost::bimap<DownloadedMapProperties::MapSource, std::wstring>::relation >
		(DownloadedMapProperties::MapSource::MapQuestOpen, L"MapQuestOpen")
		(DownloadedMapProperties::MapSource::GoogleMaps, L"Google Maps");

	const boost::bimap<DownloadedMapProperties::MapType, std::wstring> DownloadedMapProperties::s_mapTypeStrings = boost::assign::list_of < boost::bimap<DownloadedMapProperties::MapType, std::wstring>::relation >
		(DownloadedMapProperties::MapType::Map, L"Map")
		(DownloadedMapProperties::MapType::Satellite, L"Satellite")
		(DownloadedMapProperties::MapType::Hybrid, L"Hybrid");

	DownloadedMapProperties::DownloadedMapProperties(MapSource source, MapType type, Size size) :
		BaseImageProperties(),
		m_source(source),
		m_type(type),
		m_size(size)
	{
	}

	DownloadedMapProperties::DownloadedMapProperties(const boost::property_tree::wptree& propertyTree) :
		BaseImageProperties(),
		m_source(s_mapSourceStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.mapsource"))),
		m_type(s_mapTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.maptype"))),
		m_size({ { propertyTree.get<unsigned int>(L"<xmlattr>.width"), propertyTree.get<unsigned int>(L"<xmlattr>.height") } }) {

	}

	DownloadedMapProperties::DownloadedMapProperties(const DownloadedMapProperties& properties) :
		BaseImageProperties(properties),
		m_source(properties.m_source),
		m_type(properties.m_type),
		m_size(properties.m_size) {

	}

	DownloadedMapProperties::~DownloadedMapProperties()
	{
	}

	/*bool DownloadedMapProperties::IsGeographic() const {

		return true;
	}*/

	DownloadedMapProperties::MapSource DownloadedMapProperties::GetSource() const {

		return m_source;
	}

	DownloadedMapProperties::MapType DownloadedMapProperties::GetType() const {

		return m_type;
	}

	DownloadedMapProperties::Size DownloadedMapProperties::GetSize() const {

		return m_size;
	}

	void DownloadedMapProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		BaseImageProperties::ExportToPropertyTree(propertyTree);
		propertyTree.put(L"<xmlattr>.mapsource", s_mapSourceStrings.left.at(m_source));
		propertyTree.put(L"<xmlattr>.maptype", s_mapTypeStrings.left.at(m_type));
		propertyTree.put(L"<xmlattr>.width", m_size[0]);
		propertyTree.put(L"<xmlattr>.height", m_size[1]);
	}

} //namespace SynGlyphX