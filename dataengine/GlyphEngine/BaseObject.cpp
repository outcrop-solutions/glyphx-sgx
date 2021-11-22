#include "BaseObject.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/property_tree/ptree.hpp>

namespace GlyphEngine
{

	BaseObject::BaseObject(const BaseObject::PropertyTree& propertyTree) :
		m_position({ { 0.0, 0.0, 0.0 } }),
		m_rotationAngles({ { 0.0, 0.0, 0.0 } }),
		m_type(BaseObject::Type::UserImage),
		m_worldSize({ { 360.0, 180.0 } }),
		m_gridLineCounts({ { 0, 0 } }),
		m_gridLinesColor(ObjectColor::ColorArray({ { 255, 255, 255 } })) {

		if (m_type == Type::DownloadedMap) {

			//m_properties = std::unique_ptr<DownloadedMapProperties>(new DownloadedMapProperties(propertyTree));
		}
		else if (m_type == Type::UserImage) {

			//m_properties = std::unique_ptr<UserDefinedBaseObjectProperties>(new UserDefinedBaseObjectProperties(propertyTree));
		}
		else {

			//m_properties = std::unique_ptr<DefaultBaseObjectProperties>(new DefaultBaseObjectProperties(propertyTree));
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

		boost::optional<const boost::property_tree::wptree&> gridLinesPropertyTree = propertyTree.get_child_optional(L"GridLines");
		if (gridLinesPropertyTree.is_initialized()) {

			m_gridLinesColor = gridLinesPropertyTree.get().get<ObjectColor>(L"Color");
			boost::optional<bool> showGridLines = gridLinesPropertyTree.get().get_optional<bool>(L"<xmlattr>.show");
			if (!showGridLines.is_initialized()) {

				m_gridLineCounts[0] = gridLinesPropertyTree.get().get<unsigned int>(L"<xmlattr>.horizontal");
				m_gridLineCounts[1] = gridLinesPropertyTree.get().get<unsigned int>(L"<xmlattr>.vertical");
			}
			else if (showGridLines.get()) {

				m_gridLineCounts = { { 5, 11 } };
			}
		}
		else {

			m_gridLineCounts = { { 5, 11 } };
		}
	}

	BaseObject::BaseObject(const BaseObject& baseImage) :
		m_position(baseImage.m_position),
		m_rotationAngles(baseImage.m_rotationAngles),
		m_type(baseImage.m_type),
		m_worldSize(baseImage.m_worldSize),
		m_gridLineCounts(baseImage.m_gridLineCounts),
		m_gridLinesColor(baseImage.m_gridLinesColor) {

	}

	BaseObject::~BaseObject()
	{

	}

	BaseObject& BaseObject::operator=(const BaseObject& baseImage) {

		m_position = baseImage.m_position;
		m_rotationAngles = baseImage.m_rotationAngles;
		m_type = baseImage.m_type;
		m_worldSize = baseImage.m_worldSize;
		m_gridLineCounts = baseImage.m_gridLineCounts;
		m_gridLinesColor = baseImage.m_gridLinesColor;

		return *this;
	}

	bool BaseObject::operator==(const BaseObject& baseImage) const {

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

		if (m_gridLineCounts != baseImage.m_gridLineCounts) {

			return false;
		}

		if (m_gridLinesColor != baseImage.m_gridLinesColor) {

			return false;
		}

		if (m_type == BaseObject::Type::DownloadedMap) {

			//auto downloadedMapProperties = std::dynamic_pointer_cast<const DownloadedMapProperties>(m_properties);
			//return downloadedMapProperties->operator==(*std::dynamic_pointer_cast<const DownloadedMapProperties>(baseImage.m_properties).get());
		}
		else if (m_type == BaseObject::Type::UserImage) {

			//auto userImageProperties = std::dynamic_pointer_cast<const UserDefinedBaseObjectProperties>(m_properties);
			//return userImageProperties->operator==(*std::dynamic_pointer_cast<const UserDefinedBaseObjectProperties>(baseImage.m_properties).get());
		}
		else {

			//auto defaultProperties = std::dynamic_pointer_cast<const DefaultBaseObjectProperties>(m_properties);
			//return defaultProperties->operator==(*std::dynamic_pointer_cast<const DefaultBaseObjectProperties>(baseImage.m_properties).get());
		}
	}

	bool BaseObject::operator!=(const BaseObject& baseImage) const {

		return !operator==(baseImage);
	}

	BaseObject::Type BaseObject::GetType() const {

		return m_type;
	}
	/*
	BaseObjectProperties::ConstSharedPtr BaseObject::GetProperties() const {

		return m_properties;
	}

	void BaseObject::SetProperties(BaseObjectProperties::ConstSharedPtr properties) {

		if (properties != nullptr) {

			auto downloadedMapProperties = std::dynamic_pointer_cast<const DownloadedMapProperties>(properties);
			if (downloadedMapProperties != nullptr) {

				m_properties = std::make_shared<DownloadedMapProperties>(*downloadedMapProperties.get());
				m_type = Type::DownloadedMap;
				return;
			}

			auto userDefinedProperties = std::dynamic_pointer_cast<const UserDefinedBaseObjectProperties>(properties);
			if (userDefinedProperties != nullptr) {

				m_properties = std::make_shared<UserDefinedBaseObjectProperties>(*userDefinedProperties.get());
				m_type = Type::UserImage;
				return;
			}

			auto defaultProperties = std::dynamic_pointer_cast<const DefaultBaseObjectProperties>(properties);
			if (defaultProperties != nullptr) {

				m_properties = std::make_shared<DefaultBaseObjectProperties>(*defaultProperties.get());
				m_type = Type::Default;
				return;
			}
		}
		else {

			m_properties = std::make_shared<DefaultBaseObjectProperties>();
			m_type = Type::Default;
		}
	}
	*/
	void BaseObject::ExportToPropertyTree(PropertyTree& parentPropertyTree) const {

		PropertyTree& propertyTree = parentPropertyTree.add(L"BaseObject", L"");
		propertyTree.put(L"<xmlattr>.type", m_type);
		//m_properties->ExportToPropertyTree(propertyTree);

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

		PropertyTree& gridLinesPropertyTree = propertyTree.add(L"GridLines", L"");
		gridLinesPropertyTree.put(L"<xmlattr>.horizontal", m_gridLineCounts[0]);
		gridLinesPropertyTree.put(L"<xmlattr>.vertical", m_gridLineCounts[1]);
		gridLinesPropertyTree.put(L"Color", m_gridLinesColor);
	}

	void BaseObject::SetPosition(const Vector3& position) {

		m_position = position;
	}

	const Vector3& BaseObject::GetPosition() const {

		return m_position;
	}

	void BaseObject::SetRotation(const Vector3& angles) {

		m_rotationAngles = angles;
	}

	const Vector3& BaseObject::GetRotationAngles() const {

		return m_rotationAngles;
	}

	void BaseObject::SetWorldSize(const DoubleSize& worldSize) {

		m_worldSize = worldSize;
	}

	const DoubleSize& BaseObject::GetWorldSize() const {

		return m_worldSize;
	}

	void BaseObject::SetGridLineCounts(const IntSize& gridLineCounts) {

		m_gridLineCounts = gridLineCounts;
	}

	IntSize BaseObject::GetGridLineCounts() const {

		return m_gridLineCounts;
	}

	void BaseObject::SetGridLinesColor(const ObjectColor& color) {

		m_gridLinesColor = color;
	}

	ObjectColor BaseObject::GetGridLinesColor() const {

		return m_gridLinesColor;
	}

}



