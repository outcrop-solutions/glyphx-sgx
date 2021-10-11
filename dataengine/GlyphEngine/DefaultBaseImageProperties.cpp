#include "DefaultBaseImageProperties.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/property_tree/ptree.hpp>

namespace GlyphEngine {

	const boost::bimap<DefaultBaseImageProperties::Type, std::wstring> DefaultBaseImageProperties::s_typeStrings = boost::assign::list_of < boost::bimap<DefaultBaseImageProperties::Type, std::wstring>::relation >
		(DefaultBaseImageProperties::Type::World, L"World")
		(DefaultBaseImageProperties::Type::WorldGrayscale, L"WorldGrayscale")
		(DefaultBaseImageProperties::Type::Black, L"Black")
		(DefaultBaseImageProperties::Type::White, L"White")
		(DefaultBaseImageProperties::Type::Gray, L"Gray")
		(DefaultBaseImageProperties::Type::Clear, L"Clear");

	DefaultBaseImageProperties::DefaultBaseImageProperties(Type defaultBaseImage) :
		BaseImageProperties(),
		m_defaultBaseImage(defaultBaseImage)
	{
	}

	DefaultBaseImageProperties::DefaultBaseImageProperties(const boost::property_tree::wptree& propertyTree) :
		BaseImageProperties(propertyTree),
		m_defaultBaseImage(s_typeStrings.right.at(propertyTree.get_optional<std::wstring>(L"<xmlattr>.default").get_value_or(L"Clear"))) {


	}

	DefaultBaseImageProperties::DefaultBaseImageProperties(const DefaultBaseImageProperties& properties) :
		BaseImageProperties(properties),
		m_defaultBaseImage(properties.m_defaultBaseImage) {


	}

	DefaultBaseImageProperties::~DefaultBaseImageProperties()
	{
	}

	bool DefaultBaseImageProperties::operator==(const DefaultBaseImageProperties& properties) const {

		return m_defaultBaseImage == properties.m_defaultBaseImage;
	}

	bool DefaultBaseImageProperties::operator!=(const DefaultBaseImageProperties& properties) const {

		return !operator==(properties);
	}

	bool DefaultBaseImageProperties::IsGeographic() const {

		return ((m_defaultBaseImage == Type::World) || (m_defaultBaseImage == Type::WorldGrayscale));
	}
	 
	void DefaultBaseImageProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		propertyTree.put(L"<xmlattr>.default", s_typeStrings.left.at(m_defaultBaseImage));
	}

	DefaultBaseImageProperties::Type DefaultBaseImageProperties::GetDefaultBaseImageType() const {

		return m_defaultBaseImage;
	}

	void DefaultBaseImageProperties::SetDefaultBaseImageType(Type type) {

		m_defaultBaseImage = type;
	}

	std::wstring DefaultBaseImageProperties::GetBasefilename(Type defaultBaseImage) {

		return (s_typeStrings.left.at(defaultBaseImage) + L".png");
	}

}