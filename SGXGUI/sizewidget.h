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

#ifndef SIZEWIDGET_H
#define SIZEWIDGET_H

#include "sgxgui_global.h"
#include "horizontalformwidget.h"
#include <QtWidgets/QAbstractSpinBox>
#include <QtWidgets/QCheckBox>

namespace SynGlyphX {

    class SGXGUI_EXPORT SizeWidget : public HorizontalFormWidget
    {
        Q_OBJECT

    public:
        SizeWidget(bool showLockRatioCheckBox, QWidget *parent = 0);
        ~SizeWidget();

	protected slots:
		virtual void OnWidthChanged() = 0;
		virtual void OnHeightChanged() = 0;

	protected:
		virtual QMetaObject::Connection ConnectLockRatioToWidthSpinBox() = 0;
		virtual QMetaObject::Connection ConnectLockRatioToHeightSpinBox() = 0;
		virtual double GetRatio() = 0;

		void AddSpinBoxes(QAbstractSpinBox* widthSpinBox, QAbstractSpinBox* heightSpinBox, const QString& widthLabel = tr("Width:"), const QString& heightLabel = tr("Height:"));

		double m_ratio;

	private slots:
		void OnLockRatioChanged();

    private:
		QCheckBox* m_lockRatioCheckBox;
		QAbstractSpinBox* m_widthSpinBox;
		QAbstractSpinBox* m_heightSpinBox;

		QMetaObject::Connection m_widthSpinBoxConnection;
		QMetaObject::Connection m_heightSpinBoxConnection;
    };

} //namespace SynGlyphX

#endif // SIZEWIDGET_H
