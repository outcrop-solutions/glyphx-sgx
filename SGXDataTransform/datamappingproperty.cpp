#include "datamappingproperty.h"

namespace SynGlyphX {

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty() :
		value(PropertyType()),
		binding(InputBinding()) {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty(const PropertyType& initialValue) :
		value(initalValue),
		binding(InputBinding()) {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty(const boost::property_tree::wptree& propertyTree) :
		value(propertyTree.get_optional<PropertyType>(L"Value").get_value_or(PropertyType())),
		binding(InputBinding()) {

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {

			binding = InputBinding(inputFieldTree.get());
		}
	}

	template<>
	DataMappingProperty<std::pair<double, double>>::DataMappingProperty(const boost::property_tree::wptree& propertyTree)  {

		value.first = propertyTree.get<double>(L"Min");
		value.second = propertyTree.get_optional<double>(L"Difference").get_value_or(0.0);

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {

			binding = InputBinding(inputFieldTree.get());
		}
		else {

			binding.Clear();
		}
	}

	template<>
	DataMappingProperty<std::pair<Color, Color>>::DataMappingProperty(const boost::property_tree::wptree& propertyTree) {

		const boost::property_tree::wptree& colorPropertyTree = propertyTree.get_child(L"Color");
		const boost::property_tree::wptree& rgbPropertyTree = colorPropertyTree.get_child(L"RGB");
		const boost::property_tree::wptree& rgbMinPropertyTree = rgbPropertyTree.get_child(L"Min");
		value.first.Set(0, rgbMinPropertyTree.get<short>(L"R"));
		value.first.Set(1, rgbMinPropertyTree.get<short>(L"G"));
		value.first.Set(2, rgbMinPropertyTree.get<short>(L"B"));

		boost::optional<const boost::property_tree::wptree&> rgbDiffPropertyTree = rgbPropertyTree.get_child_optional(L"Difference");
		if (rgbDiffPropertyTree.is_initialized()) {

			value.second.Set(0, rgbDiffPropertyTree.get().get<short>(L"R"));
			value.second.Set(1, rgbDiffPropertyTree.get().get<short>(L"G"));
			value.second.Set(2, rgbDiffPropertyTree.get().get<short>(L"B"));
		}
		else {
			value.second.FromHexString(L"000000");
		}

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {

			binding = InputBinding(inputFieldTree.get());
		}
		else {

			binding.Clear();
		}
	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty(const MinMaxProperty& prop) :
		value(prop.value),
		binding(prop.binding) {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::~DataMappingProperty() {

	}

	template <typename PropertyType>
	boost::property_tree::wptree& DataMappingProperty<PropertyType>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		propertyTree.put<PropertyType>(L"Value", value);

		if (binding.IsBoundToInputField()) {
			binding.ExportToPropertyTree(propertyTree);
		}
	}

	template <>
	boost::property_tree::wptree& DataMappingProperty<std::pair<double, double>>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		propertyTree.put<double>(L"Min", value.first);

		if (std::abs(value.second) > 0.01) {

			propertyTree.put<double>(L"Difference", value.second);
		}

		if (binding.IsBoundToInputField()) {
			binding.ExportToPropertyTree(propertyTree);
		}
	}

	template <>
	boost::property_tree::wptree& DataMappingProperty<std::pair<Color, Color>>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		boost::property_tree::wptree& rgbMinPropertyTree = propertyTree.add(L"Min", L"");
		rgbMinPropertyTree.put<short>(L"R", value.first[0]);
		rgbMinPropertyTree.put<short>(L"G", value.first[1]);
		rgbMinPropertyTree.put<short>(L"B", value.first[2]);

		if ((std::abs(value.second[0]) > 0) || (std::abs(value.second[1]) > 0) || (std::abs(value.second[2]) > 0)) {

			boost::property_tree::wptree& rgbDiffPropertyTree = propertyTree.add(L"Difference", L"");
			rgbDiffPropertyTree.put<short>(L"R", value.second[0]);
			rgbDiffPropertyTree.put<short>(L"G", value.second[1]);
			rgbDiffPropertyTree.put<short>(L"B", value.second[2]);
		}

		if (binding.IsBoundToInputField()) {
			binding.ExportToPropertyTree(propertyTree);
		}
	}

} //namespace SynGlyphX