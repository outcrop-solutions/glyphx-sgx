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

#ifndef SYNGLYPHX_SINGLENUMERICRANGEFILTERWIDGET_H
#define SYNGLYPHX_SINGLENUMERICRANGEFILTERWIDGET_H

#include "sgxdatatransformgui_global.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QDoubleSpinBox>
#include "rangeslider.h"
#include "interval.h"

namespace SynGlyphX {

	class SGXDATATRANSFORMGUI_EXPORT SingleNumericRangeFilterWidget : public QWidget
	{
		Q_OBJECT

	public:
		SingleNumericRangeFilterWidget(Qt::Orientation orientation, QWidget *parent);
		~SingleNumericRangeFilterWidget();

		void SetMaxRangeExtents(const DegenerateInterval& range);
		DegenerateInterval GetMaxRangeExtents() const;

		void SetRange(const DegenerateInterval& range);
		DegenerateInterval GetRange() const;

	signals:
		void RangeUpdated(DegenerateInterval range);

	private slots:
		void OnSliderLowerUpdated(int lower);
		void OnSliderUpperUpdated(int upper);
		void OnMinSpinBoxUpdated(double min);
		void OnMaxSpinBoxUpdated(double max);
		void EmitRangeUpdate();

	private:
		double ConvertSliderPositionToValueInRange(int value);
		int ConvertValueInRangeToSliderPosition(double value);

		void SetMinSpinBoxBlockSignals(double min);
		void SetMaxSpinBoxBlockSignals(double max);

		QDoubleSpinBox* m_minSpinBox;
		QDoubleSpinBox* m_maxSpinBox;
		RangeSlider* m_rangeSlider;

		double m_ratio;
	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_SINGLENUMERICRANGEFILTERWIDGET_H
