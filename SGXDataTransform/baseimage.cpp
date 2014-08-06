#include "baseimage.h"
#include "downloadedmapproperties.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const boost::bimap<BaseImage::Type, std::wstring> BaseImage::s_baseImageTypeStrings = boost::assign::list_of < boost::bimap<BaseImage::Type, std::wstring>::relation >
		(BaseImage::Type::Default, L"Default")
		(BaseImage::Type::DownloadedMap, L"Downloaded Map")
		(BaseImage::Type::UserImage, L"Local Image");

	BaseImage::BaseImage(const BaseImageProperties* const properties) :
		m_properties(nullptr),
		m_type(Type::Default)
	{
		ChangeProperties(properties);
	}

	BaseImage::BaseImage(const BaseImage& baseImage) :
		m_type(baseImage.m_type) {

		ChangeProperties(baseImage.GetPropertes());
	}

	BaseImage::~BaseImage()
	{
		if (m_properties != nullptr) {
			delete m_properties;
		}
	}

	BaseImage& BaseImage::operator=(const BaseImage& baseImage) {

		m_type = baseImage.m_type;
		ChangeProperties(baseImage.GetPropertes());

		return *this;
	}

	BaseImage::Type BaseImage::GetType() const {

		return m_type;
	}

	const BaseImageProperties* const BaseImage::GetPropertes() const {

		return m_properties;
	}

	void BaseImage::ChangeProperties(const BaseImageProperties* const properties) {

		if (m_properties != nullptr) {

			delete m_properties;
			m_properties = nullptr;
		}

		if (properties != nullptr) {

			const DownloadedMapProperties* const downloadedMapProperties = dynamic_cast<const DownloadedMapProperties* const>(properties);
			if (downloadedMapProperties != nullptr) {
				
				m_properties = new DownloadedMapProperties(*downloadedMapProperties);
				m_type = Type::DownloadedMap;
				return;
			}
		}

		m_properties = new BaseImageProperties(*properties);
		m_type = Type::Default;
	}

} //namespace SynGlyphX