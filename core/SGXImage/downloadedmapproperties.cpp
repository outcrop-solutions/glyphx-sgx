#include "downloadedmapproperties.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	const boost::bimap<DownloadedMapProperties::MapSource, std::wstring> DownloadedMapProperties::s_mapSourceStrings = boost::assign::list_of < boost::bimap<DownloadedMapProperties::MapSource, std::wstring>::relation >
		(DownloadedMapProperties::MapSource::MapQuestOpen, L"MapQuestOpen")
		(DownloadedMapProperties::MapSource::GoogleMaps, L"Google Maps");

	const boost::bimap<DownloadedMapProperties::MapType, std::wstring> DownloadedMapProperties::s_mapTypeStrings = boost::assign::list_of < boost::bimap<DownloadedMapProperties::MapType, std::wstring>::relation >
		(DownloadedMapProperties::MapType::Map, L"Map")
		(DownloadedMapProperties::MapType::Satellite, L"Satellite")
		(DownloadedMapProperties::MapType::Hybrid, L"Hybrid");

	DownloadedMapProperties::DownloadedMapProperties(MapSource source, MapType type, bool useBestFit, bool invert, bool grayscale, IntSize size, unsigned int margin) :
		BaseImageProperties(),
		m_source(source),
		m_type(type),
		m_size(size),
		m_invert(invert),
		m_grayscale(grayscale),
		m_useBestFit(useBestFit),
		m_margin(margin)
	{
	}

	DownloadedMapProperties::DownloadedMapProperties(const boost::property_tree::wptree& propertyTree) :
		BaseImageProperties(propertyTree),
		m_source(s_mapSourceStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.mapsource"))),
		m_type(s_mapTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.maptype"))),
		m_size({ { propertyTree.get<unsigned int>(L"<xmlattr>.width"), propertyTree.get<unsigned int>(L"<xmlattr>.height") } }),
		m_invert(propertyTree.get_optional<bool>(L"<xmlattr>.invert").get_value_or(false)),
		m_grayscale(propertyTree.get_optional<bool>(L"<xmlattr>.grayscale").get_value_or(false)),
		m_useBestFit(propertyTree.get_optional<bool>(L"<xmlattr>.bestfit").get_value_or(false)),
		m_margin(propertyTree.get_optional<unsigned int>(L"<xmlattr>.margin").get_value_or(20)) {

	}

	DownloadedMapProperties::DownloadedMapProperties(const DownloadedMapProperties& properties) :
		BaseImageProperties(properties),
		m_source(properties.m_source),
		m_type(properties.m_type),
		m_size(properties.m_size),
		m_invert(properties.m_invert), 
		m_grayscale(properties.m_grayscale),
		m_useBestFit(properties.m_useBestFit),
		m_margin(properties.m_margin) {

	}

	DownloadedMapProperties::~DownloadedMapProperties()
	{
	}

	bool DownloadedMapProperties::operator==(const DownloadedMapProperties& properties) const {

		if (m_source != properties.m_source) {

			return false;
		}
			
		if (m_type != properties.m_type) {

			return false;
		}

		if (m_size != properties.m_size) {

			return false;
		}

		if (m_invert != properties.m_invert) {

			return false;
		}
			
		if (m_useBestFit != properties.m_useBestFit) {

			return false;
		}

		if (m_grayscale != properties.m_grayscale) {

			return false;
		}

		if (m_margin != properties.m_margin) {

			return false;
		}

		return true;
	}

	bool DownloadedMapProperties::operator!=(const DownloadedMapProperties& properties) const {

		return !operator==(properties);
	}

	bool DownloadedMapProperties::IsGeographic() const {

		return true;
	}

	DownloadedMapProperties::MapSource DownloadedMapProperties::GetSource() const {

		return m_source;
	}

	void DownloadedMapProperties::SetSource(MapSource source) {

		m_source = source;
	}

	DownloadedMapProperties::MapType DownloadedMapProperties::GetType() const {

		return m_type;
	}

	void DownloadedMapProperties::SetType(MapType type) {

		m_type = type;
	}

	IntSize DownloadedMapProperties::GetSize() const {

		return m_size;
	}

	void DownloadedMapProperties::SetSize(const IntSize& size) {

		m_size = size;
	}

	bool DownloadedMapProperties::GetInvert() const {

		return m_invert;
	}

	void DownloadedMapProperties::SetInvert(bool invert) {

		m_invert = invert;
	}

	bool DownloadedMapProperties::GetGrayscale() const {

		return m_grayscale;
	}

	void DownloadedMapProperties::SetGrayscale(bool grayscale) {

		m_grayscale = grayscale;
	}

	bool DownloadedMapProperties::GetUseBestFit() const {

		return m_useBestFit;
	}

	void DownloadedMapProperties::SetUseBestFit(bool useBestFit) {

		m_useBestFit = useBestFit;
	}

	unsigned int DownloadedMapProperties::GetMargin() const {

		return m_margin;
	}

	void DownloadedMapProperties::SetMargin(unsigned int margin) {

		m_margin = margin;
	}

	void DownloadedMapProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		propertyTree.put(L"<xmlattr>.mapsource", s_mapSourceStrings.left.at(m_source));
		propertyTree.put(L"<xmlattr>.maptype", s_mapTypeStrings.left.at(m_type));
		propertyTree.put(L"<xmlattr>.width", m_size[0]);
		propertyTree.put(L"<xmlattr>.height", m_size[1]);
		propertyTree.put(L"<xmlattr>.invert", m_invert);
		propertyTree.put(L"<xmlattr>.grayscale", m_grayscale);
		propertyTree.put(L"<xmlattr>.bestfit", m_useBestFit);
		propertyTree.put(L"<xmlattr>.margin", m_margin);
	}

} //namespace SynGlyphX