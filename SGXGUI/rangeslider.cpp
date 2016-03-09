#include "rangeslider.h"
#include <QtWidgets/QStyleOptionSlider>
#include <QtGui/QMouseEvent>

namespace SynGlyphX {

	RangeSlider::RangeSlider(Qt::Orientation orientation, QWidget *parent)
		: QSlider(orientation, parent),
		m_lowerHandleState(QStyle::SC_None),
		m_upperHandleState(QStyle::SC_None),
		m_lastPressedHandle(NoHandle)
	{

	}

	RangeSlider::~RangeSlider()
	{

	}

	int RangeSlider::GetLowerValue() const {
	
		return m_lowerValue;
	}

	int RangeSlider::GetUpperValue() const {

		return m_upperValue;
	}

	void RangeSlider::SetLowerValue(int lower) {

		lower = qBound(minimum(), lower, m_upperValue);
		if (m_lowerValue != lower) {

			m_lowerValue = lower;
			emit LowerPositionChanged(m_lowerValue);
			update();
		}
	}

	void RangeSlider::SetUpperValue(int upper) {

		upper = qBound(m_lowerValue, upper, maximum());
		if (m_upperValue != upper) {

			m_upperValue = upper;
			emit UpperValueChanged(upper);
			update();
		}	
	}

	int RangeSlider::GetLowerPosition() const {

		return m_lowerPosition;
	}

	int RangeSlider::GetUpperPosition() const {

		return m_upperPosition;
	}

	void RangeSlider::SetLowerPosition(int lower) {

		if (m_lowerPosition != lower) {

			m_lowerPosition = lower;
			if (isSliderDown()) {

				emit LowerPositionChanged(m_lowerPosition);
			}

			if (hasTracking()) {

				SetLowerValue(m_lowerPosition);
			}
			else {

				update();
			}
		}
	}

	void RangeSlider::SetUpperPosition(int upper) {

		if (m_upperPosition != upper) {

			m_upperPosition = upper;
			if (isSliderDown()) {

				emit UpperPositionChanged(m_upperPosition);
			}

			if (hasTracking()) {

				SetUpperValue(m_upperPosition);
			}
			else {

				update();
			}
		}
		
	}

	void RangeSlider::paintEvent(QPaintEvent* event) {

		QStylePainter painter(this);
		QStyleOptionSlider opt;
		InitStyleOption(&opt);

		opt.subControls = QStyle::SC_SliderGroove;
		if (tickPosition() != NoTicks) {

			opt.subControls |= QStyle::SC_SliderTickmarks;
		}
		painter.drawComplexControl(QStyle::CC_Slider, opt);
		
		//Whichever handle was last pressed should be on top so it must be drawn last
		if (m_lastPressedHandle == Handle::UpperHandle) {

			DrawHandle(painter, Handle::LowerHandle);
			DrawHandle(painter, Handle::UpperHandle);
		}
		else {

			DrawHandle(painter, Handle::UpperHandle);
			DrawHandle(painter, Handle::LowerHandle);
		}
	}

	void RangeSlider::mousePressEvent(QMouseEvent* event) {

		if (minimum() == maximum()) {

			event->ignore();
			return;
		}

		m_upperHandleState = UpdateHandleOnMousePress(event->pos(), m_upperValue, m_upperHandleState, Handle::UpperHandle);
		if (m_upperHandleState != QStyle::SC_SliderHandle) {

			m_lowerHandleState = UpdateHandleOnMousePress(event->pos(), m_lowerValue, m_lowerHandleState, Handle::LowerHandle);
		}

		event->accept();
	}

	void RangeSlider::mouseMoveEvent(QMouseEvent* event) {

		if ((m_upperHandleState != QStyle::SC_SliderHandle) && (m_lowerHandleState != QStyle::SC_SliderHandle)) {
		
			event->ignore();
			return;
		}

		QStyleOptionSlider opt;
		InitStyleOption(&opt);
		int maxDragDistance = style()->pixelMetric(QStyle::PM_MaximumDragDistance, &opt, this);
		int newPosition = ConvertPixelPositionToRangeValue(GetPointValFromOrientation(event->pos()) - m_previousOffset);
		if (maxDragDistance > 0) {

			if (!QRect().adjusted(-maxDragDistance, -maxDragDistance, maxDragDistance, maxDragDistance).contains(event->pos())) {

				newPosition = m_previousPosition;
			}
		}

		if (m_upperHandleState == QStyle::SC_SliderHandle) {

			if (newPosition < m_lowerPosition) {

				SetLowerPosition(newPosition);
			}
			SetUpperPosition(newPosition);
		}
		else if (m_lowerHandleState == QStyle::SC_SliderHandle) {

			if (newPosition > m_upperPosition) {

				SetUpperPosition(newPosition);
			}
			SetLowerPosition(newPosition);
		}

		event->accept();
	}

	void RangeSlider::mouseReleaseEvent(QMouseEvent* event) {

		QSlider::mouseReleaseEvent(event);
		setSliderDown(false);
		m_upperHandleState = QStyle::SC_None;
		m_lowerHandleState = QStyle::SC_None;
		update();
	}

	QStyle::SubControl RangeSlider::UpdateHandleOnMousePress(const QPoint& pos, int value, const QStyle::SubControl& oldControl, Handle handle) {

		QStyleOptionSlider opt;
		InitStyleOption(&opt, handle);

		QStyle::SubControl control = style()->hitTestComplexControl(QStyle::CC_Slider, &opt, pos, this);
		QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

		if (control == QStyle::SC_SliderHandle) {

			m_previousPosition = value;
			m_previousOffset = GetPointValFromOrientation(pos - handleRect.topLeft());
			m_lastPressedHandle = handle;
			setSliderDown(true);
		}

		if (control != oldControl) {

			update(handleRect);
		}

		return control;
	}

	void RangeSlider::InitStyleOption(QStyleOptionSlider* option, Handle handle) const {

		QSlider::initStyleOption(option);

		if (handle == Handle::LowerHandle) {

			option->sliderPosition = m_lowerPosition;
			option->sliderValue = m_lowerValue;
		}
		else if (handle == Handle::UpperHandle) {

			option->sliderPosition = m_upperPosition;
			option->sliderValue = m_upperValue;
		}
		else {

			option->sliderPosition = 0;
			option->sliderValue = 0;
		}
	}

	void RangeSlider::DrawHandle(QStylePainter& painter, Handle handle) {

		QStyleOptionSlider opt;
		InitStyleOption(&opt, handle);
		opt.subControls = QStyle::SC_SliderHandle;

		if (m_lastPressedHandle == handle) {

			opt.activeSubControls = QStyle::SC_SliderHandle;
			opt.state |= QStyle::State_Sunken;
		}

		painter.drawComplexControl(QStyle::CC_Slider, opt);
	}

	int RangeSlider::GetPointValFromOrientation(const QPoint& point) const {

		return ((orientation() == Qt::Horizontal) ? point.x() : point.y());
	}

	int RangeSlider::ConvertPixelPositionToRangeValue(int pos) const {

		QStyleOptionSlider opt;
		InitStyleOption(&opt);

		int sliderMin = 0;
		int sliderMax = 0;
		int sliderLength = 0;
		
		QRect grooveRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
		QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

		if (orientation() == Qt::Horizontal) {
		
			sliderLength = sliderRect.width();
			sliderMin = grooveRect.x();
			sliderMax = grooveRect.right() - sliderLength + 1;
		}
		else {
		
			sliderLength = sliderRect.height();
			sliderMin = grooveRect.y();
			sliderMax = grooveRect.bottom() - sliderLength + 1;
		}

		return QStyle::sliderValueFromPosition(minimum(), maximum(), pos - sliderMin, sliderMax - sliderMin, opt.upsideDown);
	}

} //namespace SynGlyphX