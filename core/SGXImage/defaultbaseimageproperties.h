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

#include "SGXImage.h"
#include <boost/property_tree/ptree_fwd.hpp>
SGX_PRAGMA_WARNING_PUSH
SGX_PRAGMA_WARNING_DISABLE(4512)
#include <boost/bimap.hpp>
SGX_PRAGMA_WARNING_POP
#include "baseimageproperties.h"

namespace SynGlyphX {

	class SGXIMAGE_API DefaultBaseImageProperties : public BaseImageProperties
	{
	public:
		enum Type {

			World = 0,
			WorldGrayscale,
			Black,
			Gray,
			White,
			Clear
		};

		typedef std::shared_ptr<DefaultBaseImageProperties> SharedPtr;
		typedef std::shared_ptr<const DefaultBaseImageProperties> ConstSharedPtr;

		DefaultBaseImageProperties(Type defaultBaseImage = Type::Clear);
		DefaultBaseImageProperties(const boost::property_tree::wptree& propertyTree);
		DefaultBaseImageProperties(const DefaultBaseImageProperties& properties);
		virtual ~DefaultBaseImageProperties();

		bool operator==(const DefaultBaseImageProperties& properties) const;
		bool operator!=(const DefaultBaseImageProperties& properties) const;

		virtual bool IsGeographic() const;
		virtual void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		Type GetDefaultBaseImageType() const;
		void SetDefaultBaseImageType(Type type);

		static std::wstring GetBasefilename(Type defaultBaseImage = Type::Clear);

		static const boost::bimap<Type, std::wstring> s_typeStrings;

	private:
		Type m_defaultBaseImage;
	};

} //namespace SynGlyphX
