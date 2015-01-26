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

#ifndef SYNGLYPHX_SINGLEVALUEMAPPINGDATA_H
#define SYNGLYPHX_SINGLEVALUEMAPPINGDATA_H

#include "valuemappingfunction.h"
#include <unordered_map>
#include "glyphcolor.h"

namespace SynGlyphX {
	/*
	template <typename OutputType, typename InputType>
	class SingleValueMappingData : public ValueMappingData<OutputType, InputType>
	{
	public:
		typedef std::shared_ptr<SingleValueMappingData<OutputType, InputType>> SharedPtr;
		typedef std::shared_ptr<const SingleValueMappingData<OutputType, InputType>> ConstSharedPtr;

		SingleValueMappingData();
		SingleValueMappingData(const boost::property_tree::wptree& propertyTree);
		SingleValueMappingData(const SingleValueMappingData& data);
		virtual ~SingleValueMappingData();

		void SetMappedValue(const InputType& input, const OutputType& output);
		void RemoveMappedValue(const InputType& input);
		void ClearMappedValues();
		const std::unordered_map<InputType, OutputType>& GetMappedValues() const;
		bool IsInputInMappedValues(const InputType& input) const;

		virtual OutputType GetOutputValueFromInput(const InputType& input) const;

		virtual boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

	protected:
		std::unordered_map<InputType, OutputType> m_mappedValues;
	};

	typedef SingleValueMappingData<double, double> Numeric2NumericMappingData;
	typedef SingleValueMappingData<double, std::wstring> Text2NumericMappingData;
	typedef SingleValueMappingData<GlyphColor, double> Numeric2ColorMappingData;
	typedef SingleValueMappingData<GlyphColor, std::wstring> Text2ColorMappingData;*/

} // namespace SynGlyphX

#endif //SYNGLYPHX_SINGLEVALUEMAPPINGDATA_H