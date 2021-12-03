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
#ifndef DATA_H
#define DATA_H

#include "GlyphEngine_Exports.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <boost/bimap.hpp>

namespace GlyphEngine {

	struct InputField {
		std::wstring name, field, type;
		InputField(std::wstring n, std::wstring f, std::wstring t) {
			name = n;
			field = f;
			type = t;
		}
	};

	class GLYPHENGINE Data
	{
	public:

		enum SourceType {
			File,
			DatabaseServer
		};

		typedef std::unordered_map<std::wstring, InputField> InputFieldMap;
		typedef boost::bimap<SourceType, std::wstring> SourceTypeBimap;

		Data(const boost::property_tree::wptree& datasourcePropertyTree);
        ~Data();

		void AddInputField(const boost::property_tree::wptree& datasourcePropertyTree);

		int GetInputFieldCount();

		const SourceType GetSourceType() const;

		const std::wstring& GetId() const;
        const std::wstring& GetHost() const;
        const std::wstring& GetUsername() const;
        const std::wstring& GetPassword() const;

    protected:
		std::wstring m_id;
        std::wstring m_host;
        std::wstring m_username;
        std::wstring m_password;
		std::wstring m_table;
		SourceType m_source;
		InputFieldMap m_inputFields;


	};

} 

#endif //DATA_H
