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

#ifndef SYNGLYPHX_Link_H
#define SYNGLYPHX_Link_H

#include "sgxdatamapping.h"

#include "inputtable.h"
#include <unordered_set>
#include <boost/property_tree/ptree.hpp>
#include <boost/bimap.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/functional/hash.hpp>

namespace SynGlyphX {

	class LinkFunction {

		enum FunctionType
		{
			MatchValue = 0,
			//KeyToValue,
			//KeyToRange,
			//////////
			NFunctions
		};
	};
	//Using GlyphColor will be overkill. Ideally we need a generic color class
	class LinkColor {
		typedef std::array<short, 3> ColorArray;
		ColorArray m_color;
	};

	class SGXDATAMAPPING_API Link {
	public:
		struct Node { //tree ID and lable should uniquely identify glyph
			boost::uuids::uuid treeId;
			unsigned long label;
		};

	private:
		Node m_start;
		Node m_end;
		LinkFunction m_function;
		LinkColor m_color;
		unsigned char m_alpha; // transparancy was separare in the specification, also we could use RGBA color scheme.
	};

} //namespace SynGlyphX

#endif SYNGLYPHX_Link_H

