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

#ifndef SYNGLYPHX_BASEIMAGE_H
#define SYNGLYPHX_BASEIMAGE_H

#include "SGXImage.h"
#include "utilitytypes.h"
#include "baseimageproperties.h"
#include "glyphcolor.h"
#include <boost/bimap.hpp>
#include <boost/property_tree/ptree.hpp>
#include <memory>

namespace SynGlyphX {

	class SGXIMAGE_API BaseImage
	{
	public:
		typedef std::array<double, 2> Size;
		typedef boost::property_tree::wptree PropertyTree;

		enum Type {
			Default = 0,
			DownloadedMap,
			UserImage
		};

		BaseImage(BaseImageProperties::ConstSharedPtr properties = nullptr);
		BaseImage(const PropertyTree& propertyTree);
		BaseImage(const BaseImage& baseImage);
		~BaseImage();

		BaseImage& operator=(const BaseImage& baseImage);
		bool operator==(const BaseImage& baseImage) const;
		bool operator!=(const BaseImage& baseImage) const;

		Type GetType() const;

		BaseImageProperties::ConstSharedPtr GetProperties() const;
		void SetProperties(BaseImageProperties::ConstSharedPtr properties);

		void SetPosition(const Vector3& position);
		const Vector3& GetPosition() const;

		void SetRotation(const Vector3& angles);
		const Vector3& GetRotationAngles() const;

		void SetWorldSize(const Size& worldSize);
		const Size& GetWorldSize() const;

		void SetShowGridLines(bool show);
		bool GetShowGridLines() const;

		void SetGridLinesColor(const GlyphColor& color);
		GlyphColor GetGridLinesColor() const;

		void ExportToPropertyTree(PropertyTree& parentPropertyTree) const;

		static const boost::bimap<Type, std::wstring> s_baseImageTypeStrings;

	private:
		Vector3 m_position;
		Vector3 m_rotationAngles;
		Type m_type;
		std::shared_ptr<BaseImageProperties> m_properties;
		Size m_worldSize;
		bool m_showGridLines;
		GlyphColor m_gridLinesColor;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_BASEIMAGE_H