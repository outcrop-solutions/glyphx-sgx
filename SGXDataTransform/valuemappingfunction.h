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

#include "datamappingfunction.h"

namespace SynGlyphX {

	template <typename OutputType, typename InputType>
	class ValueMappingData : public MappingFunctionData
	{
	public:
		typedef std::shared_ptr<ValueMappingData<OutputType,InputType>> SharedPtr;
		typedef std::shared_ptr<const ValueMappingData<OutputType, InputType>> ConstSharedPtr;

		ValueMappingData(Function function);
		ValueMappingData(const boost::property_tree::wptree& propertyTree);
		ValueMappingData(const ValueMappingData& data);
		virtual ~ValueMappingData();

		ValueMappingData& operator=(const ValueMappingData& data);

		void SetDefaultValue(const OutputType& defaultValue);
		const OutputType& GetDefaultValue() const;

		virtual Input GetSupportedInput() const;

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree);

		virtual OutputType GetOutputValueFromInput(const InputType& input) const = 0;

	protected:
		OutputType m_defaultValue;
	};

} // namespace SynGlyphX

#endif //SYNGLYPHX_VALUEMAPPINGDATA_H