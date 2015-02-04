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

#ifndef SYNGLYPHX_RANGE_H
#define SYNGLYPHX_RANGE_H

#include "sgxutility.h"
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXUTILITY_API Range
	{
	public:
		Range(double min, double max);
		Range(const Range& range);
		~Range();

		Range& operator=(const Range& range);
		bool operator==(const Range& range) const;
		bool operator!=(const Range& range) const;
		bool operator<(const Range& range) const;
		//bool operator<(double value) const;

		double GetMin() const;
		double GetMax() const;

		bool IsValueInRange(double value) const;

	private:
		double m_min;
		double m_max;
	};

	//This translator is so that Range can be automatically used by boost::property_tree
	class SGXUTILITY_API RangeTranslator
	{
	public:
		typedef std::wstring internal_type;
		typedef Range external_type;

		RangeTranslator();

		boost::optional<Range> get_value(std::wstring const &v);
		boost::optional<std::wstring> put_value(Range const& v);

	};

} //namespace SynGlyphX

namespace boost{

	namespace property_tree{

		template<>
		struct translator_between<std::wstring, SynGlyphX::Range>
		{
			typedef SynGlyphX::RangeTranslator type;
		};
	}
}
#endif //SYNGLYPHX_RANGE_H