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

#include "sgxutility.h"

#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/xml_parser.hpp>

#ifdef WIN32
// dllexport declarations for boost class specializations (to avoid warning 4251 on PC)
template class __declspec( dllexport ) std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>;
template class __declspec( dllexport ) boost::property_tree::basic_ptree<std::wstring, std::wstring, std::less<std::wstring>>;
//template class __declspec( dllexport ) boost::property_tree::xml_parser::xml_writer_settings<wchar_t>;
#endif

namespace SynGlyphX {

	class SGXUTILITY_API XMLPropertyTreeFile
	{
	public:
		XMLPropertyTreeFile(bool includeTimestamp = false);
		virtual ~XMLPropertyTreeFile();

		void ReadFromFile(const std::string& filename);
		void WriteToFile(const std::string& filename) const;

		static int GetReadFlags();
        static const boost::property_tree::xml_writer_settings<std::wstring>& GetWriteSettings();

	protected:
		virtual void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) = 0;
		virtual void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const = 0;
		virtual bool IsDifferentFromGivenPropertyTree(const boost::property_tree::wptree& originalPropertyTree) const;

	private:
		bool m_includeTimestamp;
		boost::property_tree::wptree m_originalPropertyTree;
		static boost::property_tree::xml_writer_settings<std::wstring> s_writeSettings;
		static const std::wstring s_timestampName;
	};

}
