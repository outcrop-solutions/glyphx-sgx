#include "sceneproperties.h"

namespace SynGlyphX {

	const std::wstring SceneProperties::s_propertyTreeName(L"SceneProperties");

	SceneProperties::SceneProperties() :
		m_backgroundColor(GlyphColor::s_black)
	{
	}

	SceneProperties::SceneProperties(const boost::property_tree::wptree& propertyTree) :
		m_backgroundColor(propertyTree.get_optional<GlyphColor>(L"BackgroundColor").get_value_or(GlyphColor::s_black)) {
		
		boost::optional<const boost::property_tree::wptree&> legendsTree = propertyTree.get_child_optional(L"Legends");
		if (legendsTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& legendPropertyTree : legendsTree.get()) {

				if (legendPropertyTree.first == Legend::s_propertyTreeName) {

					m_legends.emplace_back(legendPropertyTree.second);
				}
			}
		}
	}

	SceneProperties::SceneProperties(const SceneProperties& properties) :
		m_backgroundColor(properties.m_backgroundColor),
		m_legends(properties.m_legends){

	}

	SceneProperties::~SceneProperties()
	{
	}

	SceneProperties& SceneProperties::operator=(const SceneProperties& properties) {

		m_backgroundColor = properties.m_backgroundColor;
		m_legends = properties.m_legends;

		return *this;
	}

	bool SceneProperties::operator==(const SceneProperties& properties) const {

		if (m_legends != properties.m_legends) {

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
		if (!m_legends.empty()) {

			boost::property_tree::wptree& legendsPropertyTree = rootPropertyTree.add(L"Legends", L"");
			for (const auto& legend : m_legends) {

				legend.ExportToPropertyTree(legendsPropertyTree);
			}
		}

		return rootPropertyTree;
	}

	const GlyphColor& SceneProperties::GetBackgroundColor() const {

		return m_backgroundColor;
	}

	void SceneProperties::SetBackgroundColor(const GlyphColor& color) {

		m_backgroundColor = color;
	}

	const std::vector<Legend>& SceneProperties::GetLegends() const {

		return m_legends;
	}

	void SceneProperties::SetLegends(const std::vector<Legend>& legends) {

		m_legends = legends;
	}

} //namespace SynGlyphX