#include "baseimage.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const boost::bimap<BaseImage::Type, std::wstring> BaseImage::s_baseImageTypeStrings = boost::assign::list_of < boost::bimap<BaseImage::Type, std::wstring>::relation >
		(BaseImage::Type::Default, L"Default")
		(BaseImage::Type::DownloadedMap, L"Downloaded Map")
		(BaseImage::Type::UserImage, L"Local Image");

	BaseImage::BaseImage(const BaseImageProperties* const properties) :
		m_position({ {0.0, 0.0, 0.0} }),
		m_rotationAxis({ { 0.0, 0.0, 1.0 } }),
		m_rotationAngle(0.0),
		m_properties(nullptr),
		m_type(Type::Default)
	{
		ChangeProperties(properties);
	}

	BaseImage::BaseImage(const BaseImage::PropertyTree& propertyTree) :
		m_type(s_baseImageTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))),
		m_properties(nullptr) {

		if (m_type == Type::DownloadedMap) {

			m_properties = new DownloadedMapProperties(propertyTree);
		}
		else if (m_type == Type::UserImage) {

			m_properties = new UserDefinedBaseImageProperties(propertyTree);
		}
		else {

			m_properties = new BaseImageProperties();
		}

		boost::optional<const boost::property_tree::wptree&> positionPropertyTree = propertyTree.get_child_optional(L"Position");
		if (positionPropertyTree.is_initialized()) {

			m_position[0] = positionPropertyTree.get().get<double>(L"X");
			m_position[1] = positionPropertyTree.get().get<double>(L"Y");
			m_position[2] = positionPropertyTree.get().get<double>(L"Z");
		}

		boost::optional<const boost::property_tree::wptree&> rotationPropertyTree = propertyTree.get_child_optional(L"Rotation");
		if (positionPropertyTree.is_initialized()) {

			m_rotationAxis[0] = rotationPropertyTree.get().get<double>(L"AxisX");
			m_rotationAxis[1] = rotationPropertyTree.get().get<double>(L"AxisY");
			m_rotationAxis[2] = rotationPropertyTree.get().get<double>(L"AxisZ");
			m_rotationAngle = rotationPropertyTree.get().get<double>(L"Angle");
		}
	}

	BaseImage::BaseImage(const BaseImage& baseImage) :
		m_position(baseImage.m_position),
		m_type(baseImage.m_type),
		m_properties(nullptr) {

		ChangeProperties(baseImage.GetProperties());
	}

	BaseImage::~BaseImage()
	{
		if (m_properties != nullptr) {
			delete m_properties;
		}
	}

	BaseImage& BaseImage::operator=(const BaseImage& baseImage) {

		m_position = baseImage.m_position;
		m_type = baseImage.m_type;
		ChangeProperties(baseImage.GetProperties());

		return *this;
	}

	BaseImage::Type BaseImage::GetType() const {

		return m_type;
	}

	const BaseImageProperties* const BaseImage::GetProperties() const {

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

			const UserDefinedBaseImageProperties* const userDefinedProperties = dynamic_cast<const UserDefinedBaseImageProperties* const>(properties);
			if (userDefinedProperties != nullptr) {

				m_properties = new UserDefinedBaseImageProperties(*userDefinedProperties);
				m_type = Type::UserImage;
				return;
			}
		}

		m_properties = new BaseImageProperties(*properties);
		m_type = Type::Default;
	}

	void BaseImage::ExportToPropertyTree(PropertyTree& parentPropertyTree) const {

		PropertyTree& propertyTree = parentPropertyTree.add(L"BaseImage", L"");
		propertyTree.put(L"<xmlattr>.type", s_baseImageTypeStrings.left.at(m_type));
		m_properties->ExportToPropertyTree(propertyTree);

		PropertyTree& positionPropertyTree = parentPropertyTree.add(L"Position", L"");
		propertyTree.put(L"X", m_position[0]);
		propertyTree.put(L"Y", m_position[1]);
		propertyTree.put(L"Z", m_position[2]);

		PropertyTree& rotationPropertyTree = parentPropertyTree.add(L"Rotation", L"");
		rotationPropertyTree.put(L"AxisX", m_rotationAxis[0]);
		rotationPropertyTree.put(L"AxisY", m_rotationAxis[1]);
		rotationPropertyTree.put(L"AxisZ", m_rotationAxis[2]);
		rotationPropertyTree.put(L"Angle", m_rotationAngle);
	}

	void BaseImage::SetPosition(const Vector3& position) {

		m_position = position;
	}

	const Vector3& BaseImage::GetPosition() const {

		return m_position;
	}

	void BaseImage::SetRotation(double angle, const Vector3& axis) {

		m_rotationAngle = angle;
		m_rotationAxis = axis;
	}

	const Vector3& BaseImage::GetRotationAxis() const {

		return m_rotationAxis;
	}

	double BaseImage::GetRotationAngle() const {

		return m_rotationAngle;
	}

} //namespace SynGlyphX