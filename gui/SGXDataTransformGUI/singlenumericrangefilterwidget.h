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
#ifndef SYNGLYPHX_SINGLENUMERICRANGEFILTERWIDGET_H
#define SYNGLYPHX_SINGLENUMERICRANGEFILTERWIDGET_H

#include "sgxdatatransformgui_global.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtGui/QDoubleValidator>
#include "rangeslider.h"
#include "interval.h"
#include <set>
#include "FieldProperties.h"

namespace SynGlyphX {

	class SGXDATATRANSFORMGUI_EXPORT SingleNumericRangeFilterWidget : public QWidget
	{
		Q_OBJECT

	public:
		typedef std::set<double> SliderPositionValues;

		SingleNumericRangeFilterWidget(Qt::Orientation orientation, QWidget *parent);
		~SingleNumericRangeFilterWidget();

		void SetSliderPositionValuesAndMaxExtents(const SliderPositionValues& valuesAtSliderPosition);
		void SetMaxRangeExtents(const DegenerateInterval& range);
		DegenerateInterval GetMaxRangeExtents() const;

		void SetRange(const DegenerateInterval& range);
		DegenerateInterval GetRange() const;

		const SliderPositionValues& GetSliderPositionValues() const;

		void SetFieldProperties(FieldProperties fp) { m_fieldProperties = fp; }

	signals:
		void RangeUpdated(DegenerateInterval range);

	private slots:
		void OnSliderLowerUpdated(int lower);
		void OnSliderUpperUpdated(int upper);
		void OnMinLineEditUpdated();
		void OnMaxLineEditUpdated();
		void EmitRangeUpdate();

	private:
		double ConvertSliderPositionToValueInRange(int value);
		int ConvertValueInRangeToSliderPosition(double value);

		void SetMinLineEditBlockSignals(double min);
		void SetMaxLineEditBlockSignals(double max);

		void SetMinLineEdit(double val);
		double GetMinLineEdit() const;
		void SetMaxLineEdit(double val);
		double GetMaxLineEdit() const;

		QString ConvertDoubleToString(double val, int decimals) const;

		void SetMaxRangeExtents(double min, double max);

		//QDoubleSpinBox* m_minSpinBox;
		//QDoubleSpinBox* m_maxSpinBox;
		QLineEdit* m_minLineEdit;
		QDoubleValidator* m_minValidator;
		QLineEdit* m_maxLineEdit;
		QDoubleValidator* m_maxValidator;
		RangeSlider* m_rangeSlider;
		FieldProperties m_fieldProperties;

		double m_ratio;
		SliderPositionValues m_valuesAtSliderPosition;
	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_SINGLENUMERICRANGEFILTERWIDGET_H
