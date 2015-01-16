#include "datamappingproperty.h"

namespace SynGlyphX {

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty() :
		m_value(PropertyType()),
		m_binding(InputBinding()) {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty(const PropertyType& initialValue) :
		m_value(initalValue),
		m_binding(InputBinding()) {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty(const boost::property_tree::wptree& propertyTree) :
		m_value(propertyTree.get_optional<PropertyType>(L"Value").get_value_or(PropertyType())),
		m_binding(InputBinding()) {

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {

			m_binding = InputBinding(inputFieldTree.get());
		}
	}

	template<>
	DataMappingProperty<std::pair<double, double>>::DataMappingProperty(const boost::property_tree::wptree& propertyTree)  {

		m_value.first = propertyTree.get<double>(L"Min");
		m_value.second = propertyTree.get_optional<double>(L"Difference").get_value_or(0.0);

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {

			m_binding = InputBinding(inputFieldTree.get());
		}
		else {

			m_binding.Clear();
		}
	}

	template<>
	DataMappingProperty<std::pair<GlyphColor, GlyphColor>>::DataMappingProperty(const boost::property_tree::wptree& propertyTree) {

		const boost::property_tree::wptree& colorPropertyTree = propertyTree.get_child(L"Color");
		const boost::property_tree::wptree& rgbPropertyTree = colorPropertyTree.get_child(L"RGB");
		const boost::property_tree::wptree& rgbMinPropertyTree = rgbPropertyTree.get_child(L"Min");
		m_value.first.Set(0, rgbMinPropertyTree.get<short>(L"R"));
		m_value.first.Set(1, rgbMinPropertyTree.get<short>(L"G"));
		m_value.first.Set(2, rgbMinPropertyTree.get<short>(L"B"));

		boost::optional<const boost::property_tree::wptree&> rgbDiffPropertyTree = rgbPropertyTree.get_child_optional(L"Difference");
		if (rgbDiffPropertyTree.is_initialized()) {

			m_value.second.Set(0, rgbDiffPropertyTree.get().get<short>(L"R"));
			m_value.second.Set(1, rgbDiffPropertyTree.get().get<short>(L"G"));
			m_value.second.Set(2, rgbDiffPropertyTree.get().get<short>(L"B"));
		}
		else {
			m_value.second.FromHexString(L"000000");
		}

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {

			m_binding = InputBinding(inputFieldTree.get());
		}
		else {

			m_binding.Clear();
		}
	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty(const DataMappingProperty& prop) :
		m_value(prop.value),
		m_binding(prop.binding) {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::~DataMappingProperty() {

	}

	template <typename PropertyType>
	boost::property_tree::wptree& DataMappingProperty<PropertyType>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		propertyTree.put<PropertyType>(L"Value", value);

		if (m_binding.IsBoundToInputField()) {

			m_binding.ExportToPropertyTree(propertyTree);
		}
	}

	template <>
	boost::property_tree::wptree& DataMappingProperty<std::pair<double, double>>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		propertyTree.put<double>(L"Min", m_value.first);

		if (std::abs(m_value.second) > 0.01) {

			propertyTree.put<double>(L"Difference", m_value.second);
		}

		if (m_binding.IsBoundToInputField()) {

			m_binding.ExportToPropertyTree(propertyTree);
		}

		return propertyTree;
	}

	template <>
	boost::property_tree::wptree& DataMappingProperty<std::pair<GlyphColor, GlyphColor>>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		boost::property_tree::wptree& rgbMinPropertyTree = propertyTree.add(L"Min", L"");
		rgbMinPropertyTree.put<short>(L"R", m_value.first[0]);
		rgbMinPropertyTree.put<short>(L"G", m_value.first[1]);
		rgbMinPropertyTree.put<short>(L"B", m_value.first[2]);

		if ((std::abs(m_value.second[0]) > 0) || (std::abs(m_value.second[1]) > 0) || (std::abs(m_value.second[2]) > 0)) {

			boost::property_tree::wptree& rgbDiffPropertyTree = propertyTree.add(L"Difference", L"");
			rgbDiffPropertyTree.put<short>(L"R", m_value.second[0]);
			rgbDiffPropertyTree.put<short>(L"G", m_value.second[1]);
			rgbDiffPropertyTree.put<short>(L"B", m_value.second[2]);
		}

		if (m_binding.IsBoundToInputField()) {

			m_binding.ExportToPropertyTree(propertyTree);
		}

		return propertyTree;
	}

	template<typename PropertyType>
	PropertyType& DataMappingProperty<PropertyType>::GetValue() {

		return m_value;
	}

	template<typename PropertyType>
	const PropertyType& DataMappingProperty<PropertyType>::GetValue() const {

		return m_value;
	}

	template<typename PropertyType>
	InputBinding& DataMappingProperty<PropertyType>::GetBinding() {

		return m_binding;
	}

	template<typename PropertyType>
	const InputBinding& DataMappingProperty<PropertyType>::GetBinding() const {

		return m_binding;
	}

} //namespace SynGlyphX