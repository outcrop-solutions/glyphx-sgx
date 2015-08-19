#include "sceneproperties.h"

namespace SynGlyphX {

	const std::wstring SceneProperties::s_propertyTreeName(L"SceneProperties");

	SceneProperties::SceneProperties() :
		m_backgroundColor(GlyphColor::s_black)
	{
	}

	SceneProperties::SceneProperties(const boost::property_tree::wptree& propertyTree) :
		m_backgroundColor(propertyTree.get_optional<GlyphColor>(L"BackgroundColor").get_value_or(GlyphColor::s_black)) {
		
	}

	SceneProperties::SceneProperties(const SceneProperties& properties) :
		m_backgroundColor(properties.m_backgroundColor) {

	}

	SceneProperties::~SceneProperties()
	{
	}

	SceneProperties& SceneProperties::operator=(const SceneProperties& properties) {

		m_backgroundColor = properties.m_backgroundColor;

		return *this;
	}

	bool SceneProperties::operator==(const SceneProperties& properties) const {

		return (m_backgroundColor == properties.m_backgroundColor);
	}

	bool SceneProperties::operator!=(const SceneProperties& properties) const {

		return !operator==(properties);
	}

	boost::property_tree::wptree& SceneProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& rootPropertyTree = propertyTree.add(s_propertyTreeName, L"");

		rootPropertyTree.put<GlyphColor>(L"BackgroundColor", m_backgroundColor);

		return rootPropertyTree;
	}

	const GlyphColor& SceneProperties::GetBackgroundColor() const {

		return m_backgroundColor;
	}

	void SceneProperties::SetBackgroundColor(const GlyphColor& color) {

		m_backgroundColor = color;
	}

} //namespace SynGlyphX