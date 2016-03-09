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

#ifndef SYNGLYPHX_RANGESLIDER_H
#define SYNGLYPHX_RANGESLIDER_H

#include "sgxgui_global.h"
#include <QtWidgets/QSlider>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStylePainter>

namespace SynGlyphX {

	class SGXGUI_EXPORT RangeSlider : public QSlider
	{
		Q_OBJECT

	public:
		enum Handle {
			NoHandle,
			LowerHandle,
			UpperHandle
		};

		RangeSlider(Qt::Orientation orientation, QWidget *parent);
		~RangeSlider();

		int GetLowerValue() const;
		int GetUpperValue() const;

		int GetLowerPosition() const;
		int GetUpperPosition() const;

	signals:
		void LowerValueChanged(int lower);
		void UpperValueChanged(int upper);

		void LowerPositionChanged(int lower);
		void UpperPositionChanged(int upper);

	public slots:
		void SetLowerValue(int lower);
		void SetUpperValue(int upper);

		void SetLowerPosition(int lower);
		void SetUpperPosition(int upper);

	protected:
		void paintEvent(QPaintEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		
		QStyle::SubControl UpdateHandleOnMousePress(const QPoint& pos, int value, const QStyle::SubControl& oldControl, Handle handle);
		void InitStyleOption(QStyleOptionSlider* option, Handle handle = NoHandle) const;
		void DrawHandle(QStylePainter& painter, Handle handle);
		int GetPointValFromOrientation(const QPoint& point) const;
		int ConvertPixelPositionToRangeValue(int pos) const;

		int m_lowerValue;
		int m_upperValue;

		int m_lowerPosition;
		int m_upperPosition;

		int m_previousPosition;
		int m_previousOffset;

		Handle m_lastPressedHandle;
		QStyle::SubControl m_lowerHandleState;
		QStyle::SubControl m_upperHandleState;
	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_RANGESLIDER_H
