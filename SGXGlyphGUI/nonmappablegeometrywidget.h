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

#ifndef SYNGLYPHX_NONMAPPABLEGEOMETRYWIDGET_H
#define SYNGLYPHX_NONMAPPABLEGEOMETRYWIDGET_H

#include "sgxglyphgui_global.h"
#include "nonmappablegeometryproperties.h"
#include <QtWidgets/QWidget>
#include "surfaceradiobuttonwidget.h"
#include <QtWidgets/QDoubleSpinBox>
#include "groupboxsinglewidget.h"

Q_DECLARE_METATYPE(SynGlyphX::NonMappableGeometryProperties)

namespace SynGlyphX {

	class SGXGLYPHGUI_EXPORT NonMappableGeometryWidget : public QWidget
	{
		Q_OBJECT
		Q_PROPERTY(NonMappableGeometryProperties properties READ GetProperties WRITE SetProperties USER true)

	public:
		NonMappableGeometryWidget(QWidget *parent);
		~NonMappableGeometryWidget();

		void SetProperties(const NonMappableGeometryProperties& properties);
		NonMappableGeometryProperties GetProperties() const;

		void ShowTorusRatioWidget(bool show);

	signals:
		void PropertiesChanged();

	private:
		SurfaceRadioButtonWidget* m_surfaceRadioButtonGroup;
		QDoubleSpinBox* m_ratioSpinBox;
		GroupBoxSingleWidget* m_ratioGroupBox;
	};

} //namespace SynGlyphX

#endif // NONMAPPABLEGEOMETRYWIDGET_H
