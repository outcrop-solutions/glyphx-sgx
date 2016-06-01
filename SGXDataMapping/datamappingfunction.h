///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///
#pragma once
#ifndef SYNGLYPHX_MAPPINGFUNCTIONDATA_H
#define SYNGLYPHX_MAPPINGFUNCTIONDATA_H

#include "sgxdatamapping.h"
#include <vector>
#include <memory>
#include <boost/bimap.hpp>
#include <boost/property_tree/ptree_fwd.hpp>

namespace SynGlyphX {

	class SGXDATAMAPPING_API MappingFunctionData
	{
	public:
		enum Function {
			None,
			LinearInterpolation,
			LogarithmicInterpolation,
			Numeric2Value,
			Text2Value,
			Range2Value
		};

		enum class Input {

			Numeric = 0x01,
			Text = 0x02,
			All = 0xFF
		};

		enum class Output {

			Numeric = 0x01,
			Color = 0x02,
			NumericAndColor = 0x03,
			Shape = 0x04,
			Topology = 0x05,
			All = 0xFF
		};

		typedef boost::bimap<Function, std::wstring> FunctionBimap;
		typedef boost::bimap<Input, std::wstring> InputBimap;
		typedef boost::bimap<Output, std::wstring> OutputBimap;

		typedef std::shared_ptr<MappingFunctionData> SharedPtr;
		typedef std::shared_ptr<const MappingFunctionData> ConstSharedPtr;

		MappingFunctionData(Function function = Function::None);
		MappingFunctionData(const boost::property_tree::wptree& propertyTree);
		MappingFunctionData(const MappingFunctionData& data);
		virtual ~MappingFunctionData();

		MappingFunctionData& operator=(const MappingFunctionData& data);
		virtual bool IsSame(const MappingFunctionData& data) const;

		virtual boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		Function GetFunction() const;
		virtual Input GetSupportedInput() const;
		virtual bool NeedsDataOtherThanMinMax() const;

		virtual Output GetSupportedOutput() const;

		static const FunctionBimap s_functionNames;
		static const InputBimap s_inputNames;
		static const OutputBimap s_outputNames;

	protected:
		Function m_function;
	};

	//This translator is so that MappingFunctionData::Function can be automatically used by boost::property_tree
	class SGXDATAMAPPING_API MappingFunctionTranslator
	{
	public:
		typedef std::wstring internal_type;
		typedef MappingFunctionData::Function external_type;

		MappingFunctionTranslator();

		boost::optional<MappingFunctionData::Function> get_value(std::wstring const &v);
		boost::optional<std::wstring> put_value(MappingFunctionData::Function const& v);

	private:

	};

	//This translator is so that MappingFunctionData::Input can be automatically used by boost::property_tree
	class SGXDATAMAPPING_API MappingInputTranslator
	{
	public:
		typedef std::wstring internal_type;
		typedef MappingFunctionData::Input external_type;

		MappingInputTranslator();

		boost::optional<MappingFunctionData::Input> get_value(std::wstring const &v);
		boost::optional<std::wstring> put_value(MappingFunctionData::Input const& v);

	private:

	};

	//This translator is so that MappingFunctionData::Output can be automatically used by boost::property_tree
	class SGXDATAMAPPING_API MappingOutputTranslator
	{
	public:
		typedef std::wstring internal_type;
		typedef MappingFunctionData::Output external_type;

		MappingOutputTranslator();

		boost::optional<MappingFunctionData::Output> get_value(std::wstring const &v);
		boost::optional<std::wstring> put_value(MappingFunctionData::Output const& v);

	private:

	};

} //namespace SynGlyphX

namespace boost {

	namespace property_tree {

		template<>
		struct translator_between < std::wstring, SynGlyphX::MappingFunctionData::Function >
		{
			typedef SynGlyphX::MappingFunctionTranslator type;
		};

		template<>
		struct translator_between < std::wstring, SynGlyphX::MappingFunctionData::Input >
		{
			typedef SynGlyphX::MappingInputTranslator type;
		};

		template<>
		struct translator_between < std::wstring, SynGlyphX::MappingFunctionData::Output >
		{
			typedef SynGlyphX::MappingOutputTranslator type;
		};
	}
}  //namespace boost

#endif //SYNGLYPHX_MAPPINGFUNCTIONDATA_H
