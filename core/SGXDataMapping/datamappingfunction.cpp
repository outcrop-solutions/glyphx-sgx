#include "datamappingfunction.h"
#include <stdexcept>
#include <string>
#include "glyphcolor.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	const MappingFunctionData::FunctionBimap MappingFunctionData::s_functionNames = boost::assign::list_of < MappingFunctionData::FunctionBimap::relation >
		(MappingFunctionData::Function::None, L"None")
		(MappingFunctionData::Function::LinearInterpolation, L"Linear Interpolation")
		(MappingFunctionData::Function::LogarithmicInterpolation, L"Logarithmic Interpolation")
		(MappingFunctionData::Function::TextInterpolation, L"Text Interpolation")
		(MappingFunctionData::Function::PercentRank, L"Percent Rank")
		(MappingFunctionData::Function::Numeric2Value, L"Numeric Field To Value")
		(MappingFunctionData::Function::Text2Value, L"Text Field To Value")
		(MappingFunctionData::Function::Range2Value, L"Range To Value");

	MappingFunctionData::MappingFunctionData(Function function) :
		m_function(function) {

	}

	MappingFunctionData::MappingFunctionData(const boost::property_tree::wptree& propertyTree) : 
		m_function(propertyTree.get<Function>(L"<xmlattr>.type")) {

	}

	MappingFunctionData::MappingFunctionData(const MappingFunctionData& data) :
		m_function(data.m_function) {

	}

	MappingFunctionData::~MappingFunctionData() {

	}

	boost::property_tree::wptree& MappingFunctionData::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& functionDataPropertyTree = propertyTree.add(L"Function", L"");
		functionDataPropertyTree.put<Function>(L"<xmlattr>.type", m_function);
		return functionDataPropertyTree;
	}

	MappingFunctionData& MappingFunctionData::operator=(const MappingFunctionData& data) {

		m_function = data.m_function;

		return *this;
	}

	bool MappingFunctionData::IsSame(const MappingFunctionData& data) const {

		return (m_function == data.m_function);
	}

	MappingFunctionData::Function MappingFunctionData::GetFunction() const {

		return m_function;
	}

	bool MappingFunctionData::NeedsDataOtherThanMinMax() const {

		return false;
	}

	MappingFunctionData::Input MappingFunctionData::GetSupportedInput() const {

		return Input::All;
	}

	MappingFunctionData::Output MappingFunctionData::GetSupportedOutput() const {

		return Output::All;
	}

	MappingFunctionTranslator::MappingFunctionTranslator() {

	}

	boost::optional<MappingFunctionData::Function> MappingFunctionTranslator::get_value(std::wstring const &v) {

		MappingFunctionData::FunctionBimap::right_map::const_iterator iT = MappingFunctionData::s_functionNames.right.find(v);

		if (iT == MappingFunctionData::s_functionNames.right.end()) {

			return boost::none;
		}
		else {

			return iT->second;
		}
	}

	boost::optional<std::wstring> MappingFunctionTranslator::put_value(MappingFunctionData::Function const& v) {

		return MappingFunctionData::s_functionNames.left.at(v);
	}

	MappingInputTranslator::MappingInputTranslator() {

	}

	boost::optional<MappingFunctionData::Input> MappingInputTranslator::get_value(std::wstring const &v) {

		if (v == L"Numeric") {

			return MappingFunctionData::Input::Numeric;
		}
		else if (v == L"Text") {

			return MappingFunctionData::Input::Text;
		}
		else {

			return boost::none;
		}
	}

	boost::optional<std::wstring> MappingInputTranslator::put_value(MappingFunctionData::Input const& v) {

		if (v == MappingFunctionData::Input::Numeric) {

			return boost::optional<std::wstring>(L"Numeric");
		}
		else if (v == MappingFunctionData::Input::Text) {

			return boost::optional<std::wstring>(L"Text");
		}
		else {

			return boost::none;
		}
	}

	MappingOutputTranslator::MappingOutputTranslator() {

	}

	boost::optional<MappingFunctionData::Output> MappingOutputTranslator::get_value(std::wstring const &v) {

		if (v == L"Numeric") {

			return MappingFunctionData::Output::Numeric;
		}
		else if (v == L"Color") {

			return MappingFunctionData::Output::Color;
		}
		else if (v == L"Shape") {

			return MappingFunctionData::Output::Shape;
		}
		else if (v == L"VirtualTopology") {

			return MappingFunctionData::Output::Topology;
		}
		else {

			return boost::none;
		}
	}

	boost::optional<std::wstring> MappingOutputTranslator::put_value(MappingFunctionData::Output const& v) {

		if (v == MappingFunctionData::Output::Numeric) {

			return boost::optional<std::wstring>(L"Numeric");
		}
		else if (v == MappingFunctionData::Output::Color) {

			return boost::optional<std::wstring>(L"Color");
		}
		else if (v == MappingFunctionData::Output::Shape) {

			return boost::optional<std::wstring>(L"Shape");
		}
		else if (v == MappingFunctionData::Output::Topology) {

			return boost::optional<std::wstring>(L"VirtualTopology");
		}
		else {

			return boost::none;
		}
	}

} //namespace SynGlyphX
