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

#ifndef SYNGLYPHX_VALUEMAPPINGDATA_H
#define SYNGLYPHX_VALUEMAPPINGDATA_H

#include "sgxdatatransform_global.h"
#include "datamappingfunction.h"
#include <map>
#include "glyphcolor.h"
#include "range.h"

namespace SynGlyphX {

	template <typename OutputType, typename InputType, typename KeyType = InputType>
	class SGXDATATRANSFORM_EXPORT ValueMappingData : public MappingFunctionData
	{
	public:
		typedef std::shared_ptr<ValueMappingData<OutputType,InputType>> SharedPtr;
		typedef std::shared_ptr<const ValueMappingData<OutputType, InputType>> ConstSharedPtr;

		ValueMappingData();
		ValueMappingData(const boost::property_tree::wptree& propertyTree);
		ValueMappingData(const ValueMappingData& data);
		virtual ~ValueMappingData();

		ValueMappingData& operator=(const ValueMappingData& data);

		void SetDefaultValue(const OutputType& defaultValue);
		const OutputType& GetDefaultValue() const;

		void AddKeyValueIntoMap(const KeyType& key, const OutputType& output);
		void ClearKeyValueMap();
		const std::map<KeyType, OutputType>& GetKeyValueMap() const;

		virtual Input GetSupportedInput() const;
		virtual bool NeedsDataOtherThanMinMax() const;

		virtual boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		OutputType GetOutputValueFromInput(const InputType& input) const;

		virtual Output GetSupportedOutput() const;

	private:
		bool DoesInputMatch(const Range& range, const double& input) const;
		bool DoesInputMatch(const std::wstring& key, const std::wstring& input) const;
		bool DoesInputMatch(const double& key, const double& input) const;

		OutputType m_defaultValue;
		std::map<KeyType, OutputType> m_mappedValues;
	};

	typedef ValueMappingData<double, double> Numeric2NumericMappingData;
	typedef ValueMappingData<double, std::wstring> Text2NumericMappingData;
	typedef ValueMappingData<GlyphColor, double> Numeric2ColorMappingData;
	typedef ValueMappingData<GlyphColor, std::wstring> Text2ColorMappingData;

	typedef ValueMappingData<double, double, Range> Range2NumericMappingData;
	typedef ValueMappingData<GlyphColor, double, Range> Range2ColorMappingData;

} // namespace SynGlyphX

#endif //SYNGLYPHX_VALUEMAPPINGDATA_H