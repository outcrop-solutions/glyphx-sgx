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
#ifndef INPUTFIELDMANAGER_H
#define INPUTFIELDMANAGER_H

#include "GlyphEngine_Exports.h"
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <unordered_map>
#include "hashid.h"
#include "InputField.h"

namespace GlyphEngine {

	class GLYPHENGINE InputFieldManager
	{
	public:
		InputFieldManager(){}
		const InputField& GetInputField(const std::wstring& fieldID) const;
		void SetInputField(const std::wstring& fieldID, const InputField& field);
		void RemoveInputFieldAndBindings(const std::wstring& fieldID);
		void OnRemoveDataSource(const boost::uuids::uuid& id);
		std::wstring GenerateInputFieldID(const InputField& field); //may not be const with future implementation

		void Clear();

		void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;
		void ImportFromPropertyTree(const boost::property_tree::wptree& propertyTree);

		const std::unordered_map<std::wstring, InputField>& GetFieldMap() const;
	private:
		std::unordered_map<std::wstring, InputField> m_inputFields;
	};
}

#endif //INPUTFIELDMANAGER_H