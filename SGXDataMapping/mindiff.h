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
#ifndef SYNGLYPHX_MINDIFF_H
#define SYNGLYPHX_MINDIFF_H

#include "sgxdatamapping.h"
#include "glyphcolor.h"

namespace SynGlyphX {

	template<typename DataType>
	class SGXDATAMAPPING_API MinDiff
	{
	public:
		MinDiff();
		MinDiff(DataType min, DataType diff);
		MinDiff(const MinDiff& minDiff);
		~MinDiff();

		MinDiff& operator=(const MinDiff& minDiff);
		bool operator==(const MinDiff& minDiff) const;
		bool operator!=(const MinDiff& minDiff) const;

		void SetMinDiff(DataType min, DataType diff);
		void SetMinMax(DataType min, DataType max);
		void SetMinWithCurrentMax(DataType min);
		void SetMaxWithCurrentMin(DataType max);

		DataType GetMin() const;
		DataType GetDiff() const;
		DataType GetMax() const;

	private:
		DataType m_min;
		DataType m_diff;
	};

	typedef MinDiff<int> IntMinDiff;
	typedef MinDiff<double> DoubleMinDiff;
	typedef MinDiff<GlyphColor> ColorMinDiff;

} //namespace SynGlyphX

#endif //SYNGLYPHX_MINDIFF_H