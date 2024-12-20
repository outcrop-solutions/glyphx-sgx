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
#ifndef SYNGLYPHX_GLYPHENUMCOMBOBOX_H
#define SYNGLYPHX_GLYPHENUMCOMBOBOX_H

#include "sgxglyphgui_global.h"
#include <QtWidgets/QComboBox>
#include "glyphgeometryinfo.h"
#include "virtualtopologyinfo.h"

Q_DECLARE_METATYPE(SynGlyphX::GlyphGeometryInfo::Shape)
Q_DECLARE_METATYPE(SynGlyphX::VirtualTopologyInfo::Type)

namespace SynGlyphX {

	class SGXGLYPHGUI_EXPORT GlyphShapeComboBox : public QComboBox
	{
		Q_OBJECT
		Q_PROPERTY(SynGlyphX::GlyphGeometryInfo::Shape value READ GetCurrentValue WRITE SetCurrentValue USER true)

	public:
		GlyphShapeComboBox(QWidget *parent);
		~GlyphShapeComboBox();

		GlyphGeometryInfo::Shape GetCurrentValue() const;
		void SetCurrentValue(GlyphGeometryInfo::Shape value);

	private:
		
	};

	class SGXGLYPHGUI_EXPORT VirtualTopologyComboBox : public QComboBox
	{
		Q_OBJECT
		Q_PROPERTY(SynGlyphX::VirtualTopologyInfo::Type value READ GetCurrentValue WRITE SetCurrentValue USER true)

	public:
		VirtualTopologyComboBox(QWidget *parent);
		~VirtualTopologyComboBox();

		VirtualTopologyInfo::Type GetCurrentValue() const;
		void SetCurrentValue(VirtualTopologyInfo::Type value);

	private:

	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_GLYPHENUMCOMBOBOX_H
