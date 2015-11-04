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

#ifndef SYNGLYPHX_GLYPHPROPERTIES_WIDGET
#define SYNGLYPHX_GLYPHPROPERTIES_WIDGET

#include "sgxglyphgui_global.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <boost/shared_ptr.hpp>
#include "coloralphawidget.h"
#include "xyzwidget.h"
#include "glyph.h"
#include "groupboxsinglewidget.h"
#include "glyphstructurewidget.h"

namespace SynGlyphX {

	class SGXGLYPHGUI_EXPORT GlyphPropertiesWidget : public QWidget
	{
		Q_OBJECT

	public:
		enum ChildOption {
			Invisible = 0x00000000,
			ShowOnBottom = 0x00000001,
			ShowOnTop = 0x00000002,
			EnabledSpinBox = 0x00000004,
			AddChildrenButton = 0x00000008
		};

		Q_DECLARE_FLAGS(ChildOptions, ChildOption);

		GlyphPropertiesWidget(bool addLockToScaleWidget, ChildOptions childOptions = Invisible, QWidget *parent = 0);
		~GlyphPropertiesWidget();

		void SetNumberOfChildren(unsigned int numChildren);
		unsigned int GetNumberOfChildren() const;

		void SetGlyphFromWidget(Glyph& glyph);
		void SetWidgetFromGlyph(const Glyph& glyph, bool isNotRootNode);

	signals:
		void AddChildrenButtonClicked();

	protected:
		void CreateWidgets(bool addLockToScaleWidget, ChildOptions childOptions);
		QWidget* CreateChildrenWidget(ChildOptions childOptions);

		QSpinBox* m_childrenSpinBox;

		ColorAlphaWidget* m_colorWidget;

		XYZWidget* m_translateWidget;
		XYZWidget* m_rotateWidget;
		XYZWidget* m_scaleWidget;

		XYZWidget* m_rotateRateWidget;

		GlyphStructureWidget* m_glyphStructureWidget;
	};

	Q_DECLARE_OPERATORS_FOR_FLAGS(GlyphPropertiesWidget::ChildOptions)

} //namespace SynGlyphX

#endif // SYNGLYPHX_GLYPHPROPERTIES_WIDGET
