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

#ifndef SYNGLYPHX_INPUTFIELD_H
#define SYNGLYPHX_INPUTFIELD_H

#include "sgxdatamapping.h"
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/bimap.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/functional/hash.hpp>

namespace SynGlyphX {

	class SGXDATAMAPPING_API InputField
	{

	public:
		enum Type {
			Null = 0,
			Integer,
			Real,
			Text,
			Date
		};

		typedef boost::shared_ptr<InputField> SharedPtr;
		typedef boost::shared_ptr<const InputField> ConstSharedPtr;
		typedef size_t HashID;

		InputField();
		InputField(const boost::uuids::uuid& datasourceID, const std::wstring& table, const std::wstring field, Type type);
		InputField(const boost::property_tree::wptree& propertyTree);
		InputField(const InputField& inputField);
		~InputField();

		InputField& operator=(const InputField& inputField);
		bool operator==(const InputField& inputField) const;
		bool operator!=(const InputField& inputField) const;

		const boost::uuids::uuid& GetDatasourceID() const;
		const std::wstring& GetTable() const;
		const std::wstring& GetField() const;

		bool IsValid() const;

		void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		bool IsNumeric() const;

		static const boost::bimap<Type, std::wstring> s_fieldTypeStrings;

		HashID GetHashID() const;

	private:
		boost::uuids::uuid m_datasourceID;
		std::wstring m_table;
		std::wstring m_field;
		Type m_type;
	};

} //namespace SynGlyphX

#endif SYNGLYPHX_INPUTFIELD_H

