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
#ifndef SYNGLYPHX_SINGLERANGEFILTERWIDGET_H
#define SYNGLYPHX_SINGLERANGEFILTERWIDGET_H

#include "sgxdatatransformgui_global.h"
#include <QtWidgets/QWidget>
#include "rangeslider.h"
#include "interval.h"
#include <set>
#include "FieldProperties.h"

namespace SynGlyphX {

	class SGXDATATRANSFORMGUI_EXPORT SingleRangeFilterWidget : public QWidget
	{
		Q_OBJECT

	public:
		typedef std::set<double> SliderPositionValues;

		SingleRangeFilterWidget(QWidget *parent) : QWidget(parent) {};
		virtual ~SingleRangeFilterWidget() {};

		virtual void SetSliderPositionValuesAndMaxExtents(const SliderPositionValues& valuesAtSliderPosition) = 0;
		virtual void SetMaxRangeExtents(const DegenerateInterval& range) = 0;
		virtual DegenerateInterval GetMaxRangeExtents() const = 0;

		virtual void SetRange(const DegenerateInterval& range) = 0;
		virtual DegenerateInterval GetRange() const = 0;

		virtual const SliderPositionValues& GetSliderPositionValues() const = 0;

		virtual void SetFieldProperties(FieldProperties fp) = 0;

	signals:
		void RangeUpdated(DegenerateInterval range);

	private slots:
		virtual void EmitRangeUpdate() = 0;

	private:
		virtual QString ConvertDoubleToString(double val, int decimals) const = 0;

	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_SINGLERANGEFILTERWIDGET_H