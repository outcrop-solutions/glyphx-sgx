#include "sceneproperties.h"

namespace SynGlyphX {

	const std::wstring SceneProperties::s_propertyTreeName(L"SceneProperties");

	SceneProperties::SceneProperties() :
		m_backgroundColor(GlyphColor::s_black),
		m_legendImage(L"")
	{
	}

	SceneProperties::SceneProperties(const boost::property_tree::wptree& propertyTree) :
		m_backgroundColor(propertyTree.get_optional<GlyphColor>(L"BackgroundColor").get_value_or(GlyphColor::s_black)),
		m_legendImage(propertyTree.get_optional<std::wstring>(L"Legend").get_value_or(L"")) {
		
	}

	SceneProperties::SceneProperties(const SceneProperties& properties) :
		m_backgroundColor(properties.m_backgroundColor),
		m_legendImage(properties.m_legendImage){

	}

	SceneProperties::~SceneProperties()
	{
	}

	SceneProperties& SceneProperties::operator=(const SceneProperties& properties) {

		m_backgroundColor = properties.m_backgroundColor;
		m_legendImage = properties.m_legendImage;

		return *this;
	}

	bool SceneProperties::operator==(const SceneProperties& properties) const {

		if (m_legendImage != properties.m_legendImage) {

			return false;
		}

		return (m_backgroundColor == properties.m_backgroundColor);
	}

	bool SceneProperties::operator!=(const SceneProperties& properties) const {

		return !operator==(properties);
	}

	boost::property_tree::wptree& SceneProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& rootPropertyTree = propertyTree.add(s_propertyTreeName, L"");

		rootPropertyTree.put<GlyphColor>(L"BackgroundColor", m_backgroundColor);
		if (!m_legendImage.empty()) {

			rootPropertyTree.put<std::wstring>(L"Legend", m_legendImage);
		}

		return rootPropertyTree;
	}

	const GlyphColor& SceneProperties::GetBackgroundColor() const {

		return m_backgroundColor;
	}

	void SceneProperties::SetBackgroundColor(const GlyphColor& color) {

		m_backgroundColor = color;
	}

	const std::wstring& SceneProperties::GetLegend() const {

		return m_legendImage;
	}

	void SceneProperties::SetLegend(const std::wstring& legendImage) {

		m_legendImage = legendImage;
	}

} //namespace SynGlyphX