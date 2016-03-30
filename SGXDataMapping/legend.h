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

#ifndef SYNGLYPHX_LEGEND_H
#define SYNGLYPHX_LEGEND_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/bimap.hpp>

namespace SynGlyphX {

	class Legend
	{
	public:
		enum Type {

			Image,
			Text
		};

		typedef boost::bimap<Type, std::wstring> TypeBimap;

		Legend();
		Legend(const boost::property_tree::wptree& propertyTree);
		Legend(const Legend& legend);
		~Legend();

		Legend& operator=(const Legend& legend);
		bool operator==(const Legend& legend) const;
		bool operator!=(const Legend& legend) const;

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		void SetTitle(const std::wstring& title);
		void SetFilename(const std::wstring& filename);

		const std::wstring& GetTitle() const;
		const std::wstring& GetFilename() const;
		Type GetType() const;

		bool IsValid() const;

		static const std::wstring s_propertyTreeName;
		static const TypeBimap s_typeStrings;

	private:
		std::wstring m_title;
		std::wstring m_filename;
		Type m_type;
	};

} //SynGlyphX

#endif //SYNGLYPHX_LEGEND_H