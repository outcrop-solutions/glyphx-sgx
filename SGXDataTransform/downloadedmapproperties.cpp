#include "downloadedmapproperties.h"

namespace SynGlyphX {

	DownloadedMapProperties::DownloadedMapProperties(MapSource source, MapType type, Size size) :
		BaseImageProperties(),
		m_source(source),
		m_type(type),
		m_size(size)
	{
	}

	DownloadedMapProperties::DownloadedMapProperties(const DownloadedMapProperties& properties) :
		BaseImageProperties(properties),
		m_source(properties.m_source),
		m_type(properties.m_type) {

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

} //namespace SynGlyphX