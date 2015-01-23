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

#ifndef SYNGLYPHX_RANGEVALUEMAPPINGFUNCTION_H
#define SYNGLYPHX_RANGEVALUEMAPPINGFUNCTION_H

#include "valuemappingfunction.h"
#include "range.h"
#include <map>
#include "glyphcolor.h"

namespace SynGlyphX {

	template <typename OutputType>
	class RangeMappingData : public ValueMappingData<OutputType, double>
	{
	public:
		typedef std::shared_ptr<RangeMappingData<OutputType>> SharedPtr;
		typedef std::shared_ptr<const RangeMappingData<OutputType>> ConstSharedPtr;

		RangeMappingData();
		virtual ~RangeMappingData();

		void SetMappedValue(const Range& input, const OutputType& output);
		void RemoveMappedValue(const Range& input);
		void ClearMappedValues();
		const std::map<Range, OutputType>& GetMappedValues() const;

		virtual OutputType GetOutputValueFromInput(const double& input) const;

	protected:
		std::map<Range, OutputType> m_mappedValues;
	};

	typedef RangeMappingData<double> Range2DoubleMappingData;
	typedef RangeMappingData<GlyphColor> Range2ColorMappingData;

} // namespace SynGlyphX

#endif //SYNGLYPHX_RANGEVALUEMAPPINGFUNCTION_H