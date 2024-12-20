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
#ifndef SYNGLYPHX_SINGLEDATETIMERANGEFILTERWIDGET_H
#define SYNGLYPHX_SINGLEDATETIMERANGEFILTERWIDGET_H

#include "sgxdatatransformgui_global.h"
#include <QtWidgets/QWidget>
#include "singlerangefilterwidget.h"
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QLabel>
#include <QtGui/QDoubleValidator>
#include "rangeslider.h"
#include "interval.h"
#include <set>
#include "FieldProperties.h"

class QBoxLayout;

namespace SynGlyphX {

	class SGXDATATRANSFORMGUI_EXPORT SingleDatetimeRangeFilterWidget : public SingleRangeFilterWidget
	{
		Q_OBJECT

	public:
		typedef std::set<double> SliderPositionValues;

		SingleDatetimeRangeFilterWidget(Qt::Orientation orientation, QWidget *parent);
		SingleDatetimeRangeFilterWidget(const SingleDatetimeRangeFilterWidget&) = delete;
		//~SingleDatetimeRangeFilterWidget();

		void SetSliderPositionValuesAndMaxExtents(const SliderPositionValues& valuesAtSliderPosition) override { m_valuesAtSliderPosition = valuesAtSliderPosition; }
		void SetMaxRangeExtents(const DegenerateInterval& range) override {};
		DegenerateInterval GetMaxRangeExtents() const override { return (DegenerateInterval(m_min, m_max)); };

		void SetRange(const DegenerateInterval& range) override;
		DegenerateInterval GetRange() const override { return (DegenerateInterval(GetMinDatetimeEdit(), GetMaxDatetimeEdit())); }

		const SliderPositionValues& GetSliderPositionValues() const override { return m_valuesAtSliderPosition; };

		void SetFieldProperties(FieldProperties fp) override;

	//signals:
		//void RangeUpdated(DegenerateInterval range);

	private slots:
		void OnMinDatetimeEditUpdated();
		void OnMaxDatetimeEditUpdated();
		void EmitRangeUpdate() override;

	private:
		void UseDateTimeLayoutOnly();
		void UseDateAndTimeLayout();

		void SetMinDatetimeEdit(qint64 val);
		qint64 GetMinDatetimeEdit() const;
		void SetMaxDatetimeEdit(qint64 val);
		qint64 GetMaxDatetimeEdit() const;

		QString ConvertDoubleToString(double val, int decimals) const override;

		QBoxLayout* mainLayout;
		qint64 m_min;
		qint64 m_max;
		QDateEdit* m_minDateEdit;
		QDateEdit* m_maxDateEdit;
		QTimeEdit* m_minTimeEdit;
		QTimeEdit* m_maxTimeEdit;
		FieldProperties m_fieldProperties;

		SliderPositionValues m_valuesAtSliderPosition;

	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_SINGLEDATETIMERANGEFILTERWIDGET_H
