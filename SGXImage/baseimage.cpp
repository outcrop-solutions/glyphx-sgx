#include "baseimage.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"
#include "defaultbaseimageproperties.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const boost::bimap<BaseImage::Type, std::wstring> BaseImage::s_baseImageTypeStrings = boost::assign::list_of < boost::bimap<BaseImage::Type, std::wstring>::relation >
		(BaseImage::Type::Default, L"Default")
		(BaseImage::Type::DownloadedMap, L"Downloaded Map")
		(BaseImage::Type::UserImage, L"Local Image");

	BaseImage::BaseImage(BaseImageProperties::ConstSharedPtr properties) :
		m_position({ {0.0, 0.0, 0.0} }),
		m_rotationAngles({ { 0.0, 0.0, 0.0 } }),
		m_properties(nullptr),
		m_type(Type::Default),
		m_worldSize({ {360.0, 180.0} })
	{
		SetProperties(properties);
	}

	BaseImage::BaseImage(const BaseImage::PropertyTree& propertyTree) :
		m_type(s_baseImageTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))),
		m_properties(nullptr),
		m_position({ { 0.0, 0.0, 0.0 } }),
		m_rotationAngles({ { 0.0, 0.0, 0.0 } }),
		m_worldSize({ { 360.0, 180.0 } }) {

		if (m_type == Type::DownloadedMap) {

			m_properties = std::make_unique<DownloadedMapProperties>(propertyTree);
		}
		else if (m_type == Type::UserImage) {

			m_properties = std::make_unique<UserDefinedBaseImageProperties>(propertyTree);
		}
		else {

			m_properties = std::make_unique<DefaultBaseImageProperties>(propertyTree);
		}

		boost::optional<const boost::property_tree::wptree&> positionPropertyTree = propertyTree.get_child_optional(L"Position");
		if (positionPropertyTree.is_initialized()) {

			m_position[0] = positionPropertyTree.get().get<double>(L"X");
			m_position[1] = positionPropertyTree.get().get<double>(L"Y");
			m_position[2] = positionPropertyTree.get().get<double>(L"Z");
		}

		boost::optional<const boost::property_tree::wptree&> rotationPropertyTree = propertyTree.get_child_optional(L"Rotation");
		if (positionPropertyTree.is_initialized()) {

			m_rotationAngles[0] = rotationPropertyTree.get().get<double>(L"AngleX");
			m_rotationAngles[1] = rotationPropertyTree.get().get<double>(L"AngleY");
			m_rotationAngles[2] = rotationPropertyTree.get().get<double>(L"AngleZ");
		}

		boost::optional<const boost::property_tree::wptree&> worldSizePropertyTree = propertyTree.get_child_optional(L"WorldSize");
		if (worldSizePropertyTree.is_initialized()) {

			m_worldSize[0] = worldSizePropertyTree.get().get<double>(L"Width");
			m_worldSize[1] = worldSizePropertyTree.get().get<double>(L"Height");
		}
	}

	BaseImage::BaseImage(const BaseImage& baseImage) :
		m_position(baseImage.m_position),
		m_type(baseImage.m_type),
		m_properties(nullptr),
		m_rotationAngles(baseImage.m_rotationAngles),
		m_worldSize(baseImage.m_worldSize) {

		SetProperties(baseImage.GetProperties());
	}

	BaseImage::~BaseImage()
	{
		
	}

	BaseImage& BaseImage::operator=(const BaseImage& baseImage) {

		m_position = baseImage.m_position;
		m_rotationAngles = baseImage.m_rotationAngles;
		m_type = baseImage.m_type;
		SetProperties(baseImage.GetProperties());
		m_worldSize = baseImage.m_worldSize;

		return *this;
	}

	bool BaseImage::operator==(const BaseImage& baseImage) const {

		if (m_type != baseImage.m_type) {

			return false;
		}

		if (m_position != baseImage.m_position) {

			return false;
		}

		if (m_rotationAngles != baseImage.m_rotationAngles) {

			return false;
		}

		if (m_worldSize != baseImage.m_worldSize) {

			return false;
		}

		return true;
	}

	bool BaseImage::operator!=(const BaseImage& baseImage) const {

		return !operator==(baseImage);
	}

	BaseImage::Type BaseImage::GetType() const {

		return m_type;
	}

	BaseImageProperties::ConstSharedPtr BaseImage::GetProperties() const {

		return m_properties;
	}

	void BaseImage::SetProperties(BaseImageProperties::ConstSharedPtr properties) {

		if (properties != nullptr) {
			
			auto downloadedMapProperties = std::dynamic_pointer_cast<const DownloadedMapProperties>(properties);
			if (downloadedMapProperties != nullptr) {

				m_properties = std::make_shared<DownloadedMapProperties>(*downloadedMapProperties.get());
				m_type = Type::DownloadedMap;
				return;
			}

			auto userDefinedProperties = std::dynamic_pointer_cast<const UserDefinedBaseImageProperties>(properties);
			if (userDefinedProperties != nullptr) {

				m_properties = std::make_shared<UserDefinedBaseImageProperties>(*userDefinedProperties.get());
				m_type = Type::UserImage;
				return;
			}

			auto defaultProperties = std::dynamic_pointer_cast<const DefaultBaseImageProperties>(properties);
			if (defaultProperties != nullptr) {

				m_properties = std::make_shared<DefaultBaseImageProperties>(*defaultProperties.get());
				m_type = Type::Default;
				return;
			}
		}
		else {

			m_properties = std::make_shared<DefaultBaseImageProperties>();
			m_type = Type::Default;
		}
	}

	void BaseImage::ExportToPropertyTree(PropertyTree& parentPropertyTree) const {

		PropertyTree& propertyTree = parentPropertyTree.add(L"BaseObject", L"");
		propertyTree.put(L"<xmlattr>.type", s_baseImageTypeStrings.left.at(m_type));
		m_properties->ExportToPropertyTree(propertyTree);

		PropertyTree& positionPropertyTree = propertyTree.add(L"Position", L"");
		positionPropertyTree.put(L"X", m_position[0]);
		positionPropertyTree.put(L"Y", m_position[1]);
		positionPropertyTree.put(L"Z", m_position[2]);

		PropertyTree& rotationPropertyTree = propertyTree.add(L"Rotation", L"");
		rotationPropertyTree.put(L"AngleX", m_rotationAngles[0]);
		rotationPropertyTree.put(L"AngleY", m_rotationAngles[1]);
		rotationPropertyTree.put(L"AngleZ", m_rotationAngles[2]);

		PropertyTree& worldSizePropertyTree = propertyTree.add(L"WorldSize", L"");
		worldSizePropertyTree.put(L"Width", m_worldSize[0]);
		worldSizePropertyTree.put(L"Height", m_worldSize[1]);
	}

	void BaseImage::SetPosition(const Vector3& position) {

		m_position = position;
	}

	const Vector3& BaseImage::GetPosition() const {

		return m_position;
	}

	void BaseImage::SetRotation(const Vector3& angles) {

		m_rotationAngles = angles;
	}

	const Vector3& BaseImage::GetRotationAngles() const {

		return m_rotationAngles;
	}

	void BaseImage::SetWorldSize(const Size& worldSize) {

		m_worldSize = worldSize;
	}

	const BaseImage::Size& BaseImage::GetWorldSize() const {

		return m_worldSize;
	}

} //namespace SynGlyphX