#include "datamappingfunction.h"
#include <stdexcept>
#include <string>
#include "glyphcolor.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const MappingFunctionData::FunctionBimap MappingFunctionData::s_functionNames = boost::assign::list_of < MappingFunctionData::FunctionBimap::relation >
		(MappingFunctionData::Function::LinearInterpolation, L"Linear Interpolation")
		(MappingFunctionData::Function::LogarithmicInterpolation, L"Logarithmic Interpolation")
		(MappingFunctionData::Function::Numeric2Value, L"Numeric Field To Value")
		(MappingFunctionData::Function::Text2Value, L"Text Field To Value")
		(MappingFunctionData::Function::Range2Value, L"Range To Value");

	MappingFunctionData::MappingFunctionData(Function function) :
		m_function(function) {

	}

	MappingFunctionData::MappingFunctionData(const boost::property_tree::wptree& propertyTree) : 
		m_function(s_functionNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))) {

	}

	MappingFunctionData::MappingFunctionData(const MappingFunctionData& data) :
		m_function(data.m_function) {

	}

	MappingFunctionData::~MappingFunctionData() {

	}

	boost::property_tree::wptree& MappingFunctionData::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& functionDataPropertyTree = propertyTree.add(L"Function", L"");
		functionDataPropertyTree.put(L"<xmlattr>.type", s_functionNames.left.at(m_function));
		return functionDataPropertyTree;
	}

	MappingFunctionData& MappingFunctionData::operator=(const MappingFunctionData& data) {

		m_function = data.m_function;

		return *this;
	}

	MappingFunctionData::Function MappingFunctionData::GetFunction() const {

		return m_function;
	}

	bool MappingFunctionData::NeedsDataOtherThanMinMax() const {

		return false;
	}

	MappingFunctionData::Output MappingFunctionData::GetSupportedOutput() const {

		return Output::All;
	}

	/*
	template<typename OutputType, typename InputType>
	DataMappingFunction<OutputType, InputType>::DataMappingFunction(std::shared_ptr<const InputCombinationFunction<InputType>> inputCombinationFunction) :
		m_inputCombinationFunction(inputCombinationFunction ? inputCombinationFunction : std::make_shared<const InputCombinationFunction<InputType>>())
	{
		
	}

	template<typename OutputType, typename InputType>
	DataMappingFunction<OutputType, InputType>::~DataMappingFunction()
	{

	}

	template<typename OutputType, typename InputType>
	OutputType DataMappingFunction<OutputType, InputType>::MapInputToOuptut(const std::vector<InputType>& input) const {

		if (input.empty()) {

			throw std::invalid_argument("There must be at least one input value to map");
		}

		return MapCombinedInput(m_inputCombinationFunction->CombineInput(input));
	}

	template class DataMappingFunction < std::wstring >;
	template class DataMappingFunction < double >;
	template class DataMappingFunction < GlyphColor, double >;

	*/

} //namespace SynGlyphX