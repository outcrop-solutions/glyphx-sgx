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
#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include "sgxgui_global.h"
#include <QtWidgets/QToolButton>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>
#include "utilitytypes.h"

namespace SynGlyphX {

	class SGXGUI_EXPORT ColorButton : public QToolButton
	{
		Q_OBJECT
		Q_PROPERTY(QColor color READ GetColor WRITE SetColor USER true)

	public:
		ColorButton(bool showAlphaInColorDialog = false, QWidget *parent = 0);
		virtual ~ColorButton();

		const QColor& GetColor() const;

		virtual QSize minimumSizeHint() const;

	public slots:
		void SetColor(const QColor& color);

	signals:
		void ColorChanged(const QColor& color);

	private slots:
		void OnCustomColor();
		void SetColorFromMenu(const QColor& color);

	private:
		void UpdateButtonColor();
		QIcon CreateColorIcon(const QColor& color, const QSize& size);

		QColor m_color;
		QMenu* m_menu;
		QWidget* m_standardColorsWidget;
		bool m_showAlphaInColorDialog;
	};

} //namespace SynGlyphX

#endif // COLORBUTTON_H
