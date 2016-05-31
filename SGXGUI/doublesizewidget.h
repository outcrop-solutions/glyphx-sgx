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
#ifndef DOUBLESIZEWIDGET_H
#define DOUBLESIZEWIDGET_H

#include "sizewidget.h"
#include <QtWidgets/QDoubleSpinBox>
#include "utilitytypes.h"

namespace SynGlyphX {

	class SGXGUI_EXPORT DoubleSizeWidget : public SizeWidget
	{
		Q_OBJECT

	public:
		DoubleSizeWidget(bool showLockRatioCheckBox, QWidget *parent);
		~DoubleSizeWidget();

		void SetSize(const QSizeF& size);
		QSizeF GetSize() const;

		void SetRange(double min, double max);

		static QSizeF ConvertToQSizeF(const SynGlyphX::DoubleSize& size);
		static SynGlyphX::DoubleSize ConvertFromQSizeF(const QSizeF& size);

	protected slots:
		virtual void OnWidthChanged();
		virtual void OnHeightChanged();

	protected:
		virtual QMetaObject::Connection ConnectLockRatioToWidthSpinBox();
		virtual QMetaObject::Connection ConnectLockRatioToHeightSpinBox();
		virtual double GetRatio();

	private:
		QDoubleSpinBox* m_doubleWidthSpinBox;
		QDoubleSpinBox* m_doubleHeightSpinBox;

	};

} //namespace SynGlyphX

#endif // DOUBLESIZEWIDGET_H
