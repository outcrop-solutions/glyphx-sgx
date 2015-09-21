#include "datamappingproperty.h"
#include "interpolationmappingfunction.h"
#include "valuemappingfunction.h"

namespace SynGlyphX {

	template<>
	DataMappingProperty<VirtualTopologyInfo::Type>::DataMappingProperty() :
		m_value(VirtualTopologyInfo::Type::Circle),
		m_binding(InputBinding()),
		m_mappingFunctionData(CreateDefaultMappingData()) {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty() :
		m_value(PropertyType()),
		m_binding(InputBinding()),
		m_mappingFunctionData(CreateDefaultMappingData()) {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty(const PropertyType& initialValue) :
		m_value(initialValue),
		m_binding(InputBinding()),
		m_mappingFunctionData(CreateDefaultMappingData()) {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty(const boost::property_tree::wptree& propertyTree) :
		m_binding(InputBinding()),
		m_mappingFunctionData(CreateDefaultMappingData()) {

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {

			m_binding = InputBinding(inputFieldTree.get());
		}

		ImportValueToPropertyTree(propertyTree);

		boost::optional<const boost::property_tree::wptree&> mappingFunctionTree = propertyTree.get_child_optional(L"Function");
		if (mappingFunctionTree.is_initialized()) {

			ChangeMappingFunction(MappingFunctionData::s_functionNames.right.at(mappingFunctionTree.get().get<std::wstring>(L"<xmlattr>.type")), mappingFunctionTree.get());
		}
	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::DataMappingProperty(const DataMappingProperty& prop) :
		m_value(prop.m_value),
		m_binding(prop.m_binding),
		m_mappingFunctionData(prop.m_mappingFunctionData) {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>::~DataMappingProperty() {

	}

	template<typename PropertyType>
	DataMappingProperty<PropertyType>& DataMappingProperty<PropertyType>::operator=(const DataMappingProperty<PropertyType>& prop) {

		m_value = prop.m_value;
		m_binding = prop.m_binding;
		m_mappingFunctionData = prop.m_mappingFunctionData;

		return *this;
	}

	template<typename PropertyType>
	bool DataMappingProperty<PropertyType>::operator==(const DataMappingProperty<PropertyType>& prop) const {

		if (m_value != prop.m_value) {

			return false;
		}

		if (m_binding != prop.m_binding) {

			return false;
		}

		//Need to check here if mapping function data is the same
		/*if (m_mappingFunctionData-> != prop.m_binding) {

			return false;
		}*/

		return true;
	}

	template<typename PropertyType>
	bool DataMappingProperty<PropertyType>::operator!=(const DataMappingProperty<PropertyType>& prop) const {

		return !operator==(prop);
	}

	template <typename PropertyType>
	boost::property_tree::wptree& DataMappingProperty<PropertyType>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		ExportValueToPropertyTree(propertyTree);

		if (m_binding.IsBoundToInputField()) {

			m_binding.ExportToPropertyTree(propertyTree);
		}

		m_mappingFunctionData->ExportToPropertyTree(propertyTree);

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

	template<typename PropertyType>
	MappingFunctionData::ConstSharedPtr DataMappingProperty<PropertyType>::GetMappingFunctionData() const {

		return m_mappingFunctionData;
	}

	template<typename PropertyType>
	void DataMappingProperty<PropertyType>::SetMappingFunctionData(MappingFunctionData::SharedPtr mappingFunctionData) {

		if (IsMappingFunctionDataCompatible(mappingFunctionData)) {

			m_mappingFunctionData = mappingFunctionData;
		}
		else {

			throw std::invalid_argument("Mapping function data is not compatible with DataMappingProperty");
		}
	}
	
	template<>
	void DataMappingProperty<DoubleMinDiff>::ChangeMappingFunction(MappingFunctionData::Function function, const boost::property_tree::wptree& propertyTree) {

		if (function == MappingFunctionData::Function::LinearInterpolation) {

			m_mappingFunctionData = std::make_shared<InterpolationMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::LogarithmicInterpolation) {

			m_mappingFunctionData = std::make_shared<InterpolationMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::Numeric2Value) {

			m_mappingFunctionData = std::make_shared<Numeric2NumericMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::Text2Value) {

			m_mappingFunctionData = std::make_shared<Text2NumericMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::Range2Value) {

			m_mappingFunctionData = std::make_shared<Range2NumericMappingData>(propertyTree);
		}
	}

	template<>
	void DataMappingProperty<ColorMinDiff>::ChangeMappingFunction(MappingFunctionData::Function function, const boost::property_tree::wptree& propertyTree) {

		if (function == MappingFunctionData::Function::LinearInterpolation) {

			m_mappingFunctionData = std::make_shared<InterpolationMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::LogarithmicInterpolation) {

			m_mappingFunctionData = std::make_shared<InterpolationMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::Numeric2Value) {

			m_mappingFunctionData = std::make_shared<Numeric2ColorMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::Text2Value) {

			m_mappingFunctionData = std::make_shared<Text2ColorMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::Range2Value) {

			m_mappingFunctionData = std::make_shared<Range2ColorMappingData>(propertyTree);
		}
	}

	template<>
	void DataMappingProperty<GlyphGeometryInfo::Shape>::ChangeMappingFunction(MappingFunctionData::Function function, const boost::property_tree::wptree& propertyTree) {

		if (function == MappingFunctionData::Function::Numeric2Value) {

			m_mappingFunctionData = std::make_shared<Numeric2ShapeMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::Text2Value) {

			m_mappingFunctionData = std::make_shared<Text2ShapeMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::Range2Value) {

			m_mappingFunctionData = std::make_shared<Range2ShapeMappingData>(propertyTree);
		}
	}

	template<>
	void DataMappingProperty<VirtualTopologyInfo::Type>::ChangeMappingFunction(MappingFunctionData::Function function, const boost::property_tree::wptree& propertyTree) {

		if (function == MappingFunctionData::Function::Numeric2Value) {

			m_mappingFunctionData = std::make_shared<Numeric2VirtualTopologyMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::Text2Value) {

			m_mappingFunctionData = std::make_shared<Text2VirtualTopologyMappingData>(propertyTree);
		}
		else if (function == MappingFunctionData::Function::Range2Value) {

			m_mappingFunctionData = std::make_shared<Range2VirtualTopologyMappingData>(propertyTree);
		}
	}

	template<typename PropertyType>
	void DataMappingProperty<PropertyType>::ChangeMappingFunction(MappingFunctionData::Function function, const boost::property_tree::wptree& propertyTree) {

		m_mappingFunctionData = std::make_shared<MappingFunctionData>();
	}

	template<typename PropertyType>
	bool DataMappingProperty<PropertyType>::IsMappingFunctionDataCompatible(MappingFunctionData::SharedPtr mappingFunctionData) const {

		MappingFunctionData::Function function = mappingFunctionData->GetFunction();
		if ((std::is_same<GlyphGeometryInfo::Shape, PropertyType>::value) || (std::is_same<VirtualTopologyInfo::Type, PropertyType>::value)) {

			return ((function != MappingFunctionData::Function::LinearInterpolation) && (function != MappingFunctionData::Function::LogarithmicInterpolation));
		}
		else if (std::is_same<std::wstring, PropertyType>::value) {

			return (function == MappingFunctionData::Function::None);
		}
		else {

			return true;
		}
	}

	template<>
	void DataMappingProperty<DoubleMinDiff>::ExportValueToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		propertyTree.put<double>(L"Min", m_value.GetMin());

		if (std::abs(m_value.GetDiff()) > 0.01) {

			propertyTree.put<double>(L"Difference", m_value.GetDiff());
		}
	}

	template<>
	void DataMappingProperty<ColorMinDiff>::ExportValueToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//m_value.first.ExportToPropertyTree(propertyTree.add(L"Min", L""));
		propertyTree.put<GlyphColor>(L"Min", m_value.GetMin());

		if ((std::abs(m_value.GetDiff()[0]) > 0) || (std::abs(m_value.GetDiff()[1]) > 0) || (std::abs(m_value.GetDiff()[2]) > 0)) {

			//m_value.second.ExportToPropertyTree(propertyTree.add(L"Difference", L""));
			propertyTree.put<GlyphColor>(L"Difference", m_value.GetDiff());
		}
	}

	template<typename PropertyType>
	void DataMappingProperty<PropertyType>::ExportValueToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		propertyTree.put<PropertyType>(L"Value", m_value);
	}

	template<>
	void DataMappingProperty<DoubleMinDiff>::ImportValueToPropertyTree(const boost::property_tree::wptree& propertyTree) {

		m_value.SetMinDiff(propertyTree.get<double>(L"Min"), propertyTree.get_optional<double>(L"Difference").get_value_or(0.0));
	}

	template<>
	void DataMappingProperty<ColorMinDiff>::ImportValueToPropertyTree(const boost::property_tree::wptree& propertyTree) {

		GlyphColor min, diff;
		//For color check if property tree has it in the old way.  If not read in using the new way
		boost::optional<short> rgbMinRed = propertyTree.get_optional<short>(L"Min.R");
		if (rgbMinRed.is_initialized()) {

			min.Set(0, rgbMinRed.get());
			min.Set(1, propertyTree.get<short>(L"Min.G"));
			min.Set(2, propertyTree.get<short>(L"Min.B"));
		}
		else {

			min = propertyTree.get<GlyphColor>(L"Min");
		}

		boost::optional<short> rgbDiffRed = propertyTree.get_optional<short>(L"Difference.R");
		if (rgbDiffRed.is_initialized()) {

			diff.Set(0, rgbDiffRed.get());
			diff.Set(1, propertyTree.get<short>(L"Difference.G"));
			diff.Set(2, propertyTree.get<short>(L"Difference.B"));
		}
		else {

			diff = propertyTree.get_optional<GlyphColor>(L"Difference").get_value_or(GlyphColor({ { 0, 0, 0 } }));
		}

		m_value.SetMinDiff(min, diff);
	}

	template<typename PropertyType>
	void DataMappingProperty<PropertyType>::ImportValueToPropertyTree(const boost::property_tree::wptree& propertyTree) {

		m_value = propertyTree.get_optional<PropertyType>(L"Value").get_value_or(PropertyType());
	}

	template<typename PropertyType>
	MappingFunctionData::SharedPtr DataMappingProperty<PropertyType>::CreateDefaultMappingData() {

		MappingFunctionData::SharedPtr mappingFunctionData = nullptr;
		if ((std::is_same<ColorMinDiff, PropertyType>::value) || (std::is_same<DoubleMinDiff, PropertyType>::value)) {

			mappingFunctionData = std::make_shared<InterpolationMappingData>();
		}
		else {

			mappingFunctionData = std::make_shared<MappingFunctionData>();
		}

		return mappingFunctionData;
	}

	template class DataMappingProperty < std::wstring >;
	template class DataMappingProperty<DoubleMinDiff>;
	template class DataMappingProperty<ColorMinDiff>;
	template class DataMappingProperty <GlyphGeometryInfo::Shape>;
	template class DataMappingProperty <VirtualTopologyInfo::Type>;

} //namespace SynGlyphX