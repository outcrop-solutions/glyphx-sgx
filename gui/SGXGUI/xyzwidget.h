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
#ifndef XYZWIDGET_H
#define XYZWIDGET_H

#include "sgxgui_global.h"
#include <QtWidgets/QWidget>
#include "utilitytypes.h"
class QDoubleSpinBox;
class QCheckBox;
namespace SynGlyphX {

	class SGXGUI_EXPORT XYZWidget : public QWidget
	{
		Q_OBJECT

	public:
		XYZWidget(bool includeLockCheckbox = false, QWidget *parent = 0);
		~XYZWidget();

		void SetRange(double min, double max);

		void SetX(double value);
		double GetX() const;
		void SetY(double value);
		double GetY() const;
		void SetZ(double value);
		double GetZ() const;

		void Set(const SynGlyphX::Vector3& vec);
		SynGlyphX::Vector3 Get() const;

		void SetWrapping(bool wrapping);
		void SetDecimal(int decimals);
		void SetSuffix(const QString& suffix);

		void SetEnabled(bool enableX, bool enableY, bool enableZ, bool enableLock);

	public slots:
		void SetSpinBoxesLocked(bool lock);

	signals:
		void ValuesChanged(double x, double y, double z);

		private slots:
		void OnXSpinBoxChanged();
		void OnYSpinBoxChanged();
		void OnZSpinBoxChanged();
		void UpdateSpinBoxLock(bool lock);

	private:
		void SetSpinBoxWithoutSignals(QDoubleSpinBox* spinBox, double value);

		QDoubleSpinBox* m_xSpinBox;
		QDoubleSpinBox* m_ySpinBox;
		QDoubleSpinBox* m_zSpinBox;
		QCheckBox* m_lockCheckBox;
		bool m_spinBoxesLocked;
	};

} //namespace SynGlyphX

#endif // XYZWIDGET_H
