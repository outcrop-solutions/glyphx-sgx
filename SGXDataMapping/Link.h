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
#ifndef SYNGLYPHX_Link_H
#define SYNGLYPHX_Link_H

#include "sgxdatamapping.h"

#include "inputfield.h"
#include <unordered_set>
#include <boost/property_tree/ptree.hpp>
#include <boost/bimap.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/functional/hash.hpp>

class LinksDialog;

namespace SynGlyphX {

	struct LinkNode { //tree ID and lable should uniquely identify glyph
		LinkNode(boost::uuids::uuid treeId, unsigned long label, const std::wstring& inputFieldId) : m_treeId(treeId), m_label(label), m_inputFieldId(inputFieldId) {}
		LinkNode(const boost::property_tree::wptree& propertyTree);
		LinkNode() {}
		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) const;
		boost::uuids::uuid m_treeId;
		unsigned long m_label;
		std::wstring m_inputFieldId;
	};

	class SGXDATAMAPPING_API Link {
        friend class ::LinksDialog;
	public:
		//Using GlyphColor will be overkill. Ideally we need a generic color class
		struct Color {
			unsigned short m_r, m_g, m_b, m_alpha;
			bool m_inheritfromParent;
			void SetRGB(unsigned short r, unsigned short g, unsigned short b) {
				m_r = r; m_g = g; m_b = b;
			}
			Color() {}
			Color(const boost::property_tree::wptree& propertyTree);
			boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) const;
		};

		class Function { //currently implemented as dumb tree-holder, so LinksDialog can deal with it directly
		public:
			//enum class Type
			//{
			//	MatchValue = 0,
			//	KeyToValue = 1,
			//	KeyToRange = 2,
			//	//////////
			//	NFunctions
			//};
			Function() {}
			//virtual ~Function() {}
			Function(const boost::property_tree::wptree& propertyTree);
			virtual boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;
			//virtual Type GetType() { return m_type;  }
			//static std::shared_ptr<Function> CreateFunction(const boost::property_tree::wptree& propertyTree);
			//Type m_type;
			boost::property_tree::wptree m_propertyTree;
		};
		Link();
		~Link();
		Link(const boost::property_tree::wptree& propertyTree);
		const std::wstring& GetName() const { return m_name; } 
		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) const;
	private:
		std::wstring m_name;
		LinkNode m_start;
		LinkNode m_end;
		Function m_function;
		Color m_color;
	};

} //namespace SynGlyphX

#endif SYNGLYPHX_Link_H

