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

#ifndef SYNGLYPHX_MAPPINGFUNCTIONDATA_H
#define SYNGLYPHX_MAPPINGFUNCTIONDATA_H

#include "sgxdatatransform_global.h"
#include <vector>
#include <memory>
#include <boost/bimap.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT MappingFunctionData
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
			Enum = 0x04,
			All = 0xFF
		};

		typedef boost::bimap<Function, std::wstring> FunctionBimap;

		typedef std::shared_ptr<MappingFunctionData> SharedPtr;
		typedef std::shared_ptr<const MappingFunctionData> ConstSharedPtr;

		MappingFunctionData(Function function = Function::Identity);
		MappingFunctionData(const boost::property_tree::wptree& propertyTree);
		MappingFunctionData(const MappingFunctionData& data);
		virtual ~MappingFunctionData();

		MappingFunctionData& operator=(const MappingFunctionData& data);

		virtual boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		Function GetFunction() const;
		virtual Input GetSupportedInput() const;
		virtual bool NeedsDataOtherThanMinMax() const;

		virtual Output GetSupportedOutput() const;

		static const FunctionBimap s_functionNames;

	protected:
		Function m_function;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_MAPPINGFUNCTIONDATA_H
