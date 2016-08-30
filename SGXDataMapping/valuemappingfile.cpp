#include "valuemappingfile.h"

namespace SynGlyphX {

	template<typename ValueMappingType>
	ValueMappingFile<ValueMappingType>::ValueMappingFile(std::shared_ptr<ValueMappingType> valueMapping) :
		m_valueMapping(valueMapping) {


	}

	template<typename ValueMappingType>
	ValueMappingFile<ValueMappingType>::~ValueMappingFile() {


	}

	template<typename ValueMappingType>
	std::shared_ptr<ValueMappingType> ValueMappingFile<ValueMappingType>::GetValueMapping() const {

		return m_valueMapping;
	}

	template<typename ValueMappingType>
	void ValueMappingFile<ValueMappingType>::ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) {

		boost::optional<const boost::property_tree::wptree&> valueMappingPropertyTree = filePropertyTree.get_child_optional(L"Function");
		if (!valueMappingPropertyTree.is_initialized()) {

			throw std::runtime_error("File does not have value mapping");
		}

		if (ValueMappingType::GetFunctionForThisType() != valueMappingPropertyTree.get().get<MappingFunctionData::Function>(L"<xmlattr>.type")) {

			throw std::runtime_error("File does not match current value mapping type");
		}

		if (ValueMappingType::GetSupportedInputForThisType() != valueMappingPropertyTree.get().get<MappingFunctionData::Input>(L"<xmlattr>.input")) {

			throw std::runtime_error("File does not match current value input type");
		}

		if (ValueMappingType::GetSupportedOutputForThisType() != valueMappingPropertyTree.get().get<MappingFunctionData::Output>(L"<xmlattr>.output")) {

			throw std::runtime_error("File does not match current value output type");
		}

		m_valueMapping = std::make_shared<ValueMappingType>(valueMappingPropertyTree.get());
	}

	template<typename ValueMappingType>
	void ValueMappingFile<ValueMappingType>::ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const {

		if (m_valueMapping) {

			boost::property_tree::wptree& valueMappingPropertyTree = m_valueMapping->ExportToPropertyTree(filePropertyTree);
			valueMappingPropertyTree.put<MappingFunctionData::Input>(L"<xmlattr>.input", ValueMappingType::GetSupportedInputForThisType());
			valueMappingPropertyTree.put<MappingFunctionData::Output>(L"<xmlattr>.output", ValueMappingType::GetSupportedOutputForThisType());
		}
		else {

			throw std::runtime_error("Can't save a null value mapping to a file");
		}
	}

	template class ValueMappingFile<Numeric2NumericMappingData>;
	template class ValueMappingFile<Text2NumericMappingData>;
	template class ValueMappingFile<Numeric2ColorMappingData>;
	template class ValueMappingFile<Text2ColorMappingData>;

	template class ValueMappingFile<Numeric2ShapeMappingData>;
	template class ValueMappingFile<Text2ShapeMappingData>;

	template class ValueMappingFile<Numeric2VirtualTopologyMappingData>;
	template class ValueMappingFile<Text2VirtualTopologyMappingData>;

	template class ValueMappingFile<Range2NumericMappingData>;
	template class ValueMappingFile<Range2ColorMappingData>;

	template class ValueMappingFile<Range2ShapeMappingData>;
	template class ValueMappingFile<Range2VirtualTopologyMappingData>;

} //namespace SynGlyphX
