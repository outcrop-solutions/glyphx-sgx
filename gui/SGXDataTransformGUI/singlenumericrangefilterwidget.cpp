#include "singlenumericrangefilterwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <cmath>

namespace SynGlyphX {

	SingleNumericRangeFilterWidget::SingleNumericRangeFilterWidget(Qt::Orientation orientation, QWidget *parent)
		: QWidget(parent)
	{
		QBoxLayout* mainLayout = nullptr;
		if (orientation == Qt::Horizontal) {

			mainLayout = new QHBoxLayout(this);
		}
		else {

			mainLayout = new QVBoxLayout(this);
		}

		m_minLineEdit = new QLineEdit("", this);
		m_minValidator = new QDoubleValidator(m_minLineEdit);
		m_minLineEdit->setValidator(m_minValidator);
		mainLayout->addWidget(m_minLineEdit, 1);

		m_rangeSlider = new RangeSlider(orientation, this);
		m_rangeSlider->setRange(0, 99);
		m_rangeSlider->setTracking(true);
		mainLayout->addWidget(m_rangeSlider, 3);

		m_maxLineEdit = new QLineEdit("", this);
		m_maxValidator = new QDoubleValidator(m_maxLineEdit);
		m_maxLineEdit->setValidator(m_maxValidator);
		mainLayout->addWidget(m_maxLineEdit, 1);

		setLayout(mainLayout);

		QObject::connect(m_rangeSlider, &RangeSlider::LowerPositionChanged, this, &SingleNumericRangeFilterWidget::OnSliderLowerUpdated);
		QObject::connect(m_rangeSlider, &RangeSlider::UpperPositionChanged, this, &SingleNumericRangeFilterWidget::OnSliderUpperUpdated);

		QObject::connect(m_rangeSlider, &RangeSlider::LowerValueChanged, this, &SingleNumericRangeFilterWidget::EmitRangeUpdate);
		QObject::connect(m_rangeSlider, &RangeSlider::UpperValueChanged, this, &SingleNumericRangeFilterWidget::EmitRangeUpdate);

		QObject::connect(m_minLineEdit, &QLineEdit::editingFinished, this, &SingleNumericRangeFilterWidget::OnMinLineEditUpdated);
		QObject::connect(m_maxLineEdit, &QLineEdit::editingFinished, this, &SingleNumericRangeFilterWidget::OnMaxLineEditUpdated);

		SetMaxRangeExtents(DegenerateInterval(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max()));
	}

	SingleNumericRangeFilterWidget::~SingleNumericRangeFilterWidget()
	{

	}

	void SingleNumericRangeFilterWidget::SetMaxRangeExtents(const DegenerateInterval& range) {

		m_valuesAtSliderPosition.clear();
		m_rangeSlider->setRange(0, 99);
		SetMaxRangeExtents(range.GetMin(), range.GetMax());
	}

	void SingleNumericRangeFilterWidget::SetMaxRangeExtents(double min, double max) {

		bool wasRangeUpdated = false;

		m_minLineEdit->blockSignals(true);
		m_maxLineEdit->blockSignals(true);
		m_rangeSlider->blockSignals(true);

		if (GetMinLineEdit() < min) {

			SetMinLineEditBlockSignals(min);
			wasRangeUpdated = true;
		}
		if (GetMaxLineEdit() > max) {

			SetMaxLineEditBlockSignals(max);
			wasRangeUpdated = true;
		}
		
		m_minValidator->setRange(min, max, 3);
		m_maxValidator->setRange(min, max, 3);

		double difference = max - min;
		if (difference > 0.001) {

			m_ratio = difference / (m_rangeSlider->maximum() - m_rangeSlider->minimum());

			m_rangeSlider->SetLowerValue(ConvertValueInRangeToSliderPosition(GetMinLineEdit()));
			m_rangeSlider->SetUpperValue(ConvertValueInRangeToSliderPosition(GetMaxLineEdit()));
			setEnabled(true);
		}
		else {

			m_ratio = 0.0;

			m_rangeSlider->SetLowerValue(m_rangeSlider->minimum());
			m_rangeSlider->SetUpperValue(m_rangeSlider->maximum());
			setEnabled(false);
		}
			

		m_minLineEdit->blockSignals(false);
		m_maxLineEdit->blockSignals(false);
		m_rangeSlider->blockSignals(false);

		if (wasRangeUpdated) {

			EmitRangeUpdate();
		}
	}

	void SingleNumericRangeFilterWidget::SetSliderPositionValuesAndMaxExtents(const SliderPositionValues& valuesAtSliderPosition) {

		m_valuesAtSliderPosition = valuesAtSliderPosition;
		m_rangeSlider->setRange(0, int(m_valuesAtSliderPosition.size()) - 1);
		SetMaxRangeExtents(*m_valuesAtSliderPosition.begin(), *m_valuesAtSliderPosition.rbegin());
	}

	const SingleNumericRangeFilterWidget::SliderPositionValues& SingleNumericRangeFilterWidget::GetSliderPositionValues() const {

		return m_valuesAtSliderPosition;
	}

	DegenerateInterval SingleNumericRangeFilterWidget::GetMaxRangeExtents() const {

		return (DegenerateInterval(m_minValidator->bottom(), m_maxValidator->top()));
	}

	void SingleNumericRangeFilterWidget::SetRange(const DegenerateInterval& range) {

		if (m_ratio == 0.0) {

			return;
		}

		double min = qBound(m_minValidator->bottom(), range.GetMin(), m_minValidator->top());
		double max = qBound(m_maxValidator->bottom(), range.GetMax(), m_maxValidator->top());

		bool unblockSignals = signalsBlocked();
		blockSignals(true);
		SetMinLineEdit(min);
		SetMaxLineEdit(max);
		m_rangeSlider->blockSignals(true);
		m_rangeSlider->SetLowerValue(ConvertValueInRangeToSliderPosition(min));
		m_rangeSlider->SetUpperValue(ConvertValueInRangeToSliderPosition(max));
		m_rangeSlider->blockSignals(false);
		blockSignals(unblockSignals);

		EmitRangeUpdate();
	}

	DegenerateInterval SingleNumericRangeFilterWidget::GetRange() const {

		return (DegenerateInterval(GetMinLineEdit(), GetMaxLineEdit()));
	}

	void SingleNumericRangeFilterWidget::OnSliderLowerUpdated(int lower) {

		SetMinLineEditBlockSignals(ConvertSliderPositionToValueInRange(lower));
		EmitRangeUpdate();
	}

	void SingleNumericRangeFilterWidget::OnSliderUpperUpdated(int upper) {

		SetMaxLineEditBlockSignals(ConvertSliderPositionToValueInRange(upper));
		EmitRangeUpdate();
	}

	void SingleNumericRangeFilterWidget::OnMinLineEditUpdated() {

		double min = qBound(m_minValidator->bottom(), GetMinLineEdit(), m_minValidator->top());
		SetMinLineEditBlockSignals(min);

		m_rangeSlider->blockSignals(true);
		m_rangeSlider->SetLowerValue(ConvertValueInRangeToSliderPosition(min));
		m_rangeSlider->blockSignals(false);

		if (min > GetMaxLineEdit()) {

			SetMaxLineEditBlockSignals(min);
		}

		EmitRangeUpdate();
	}

	void SingleNumericRangeFilterWidget::OnMaxLineEditUpdated() {

		double max = qBound(m_maxValidator->bottom(), GetMaxLineEdit(), m_maxValidator->top());
		SetMaxLineEditBlockSignals(max);

		m_rangeSlider->blockSignals(true);
		m_rangeSlider->SetUpperValue(ConvertValueInRangeToSliderPosition(max));
		m_rangeSlider->blockSignals(false);

		if (max < GetMinLineEdit()) {

			SetMinLineEditBlockSignals(max);
		}

		EmitRangeUpdate();
	}

	void SingleNumericRangeFilterWidget::EmitRangeUpdate() {

		emit RangeUpdated(DegenerateInterval(GetMinLineEdit(), GetMaxLineEdit()));
	}

	double SingleNumericRangeFilterWidget::ConvertSliderPositionToValueInRange(int value) {

		if (m_valuesAtSliderPosition.empty()) {

			return ((m_ratio * value) + m_minValidator->bottom());
		}
		else {

			SliderPositionValues::const_iterator iT = m_valuesAtSliderPosition.begin();
			std::advance(iT, value);
			return *iT;
		}
	}

	int SingleNumericRangeFilterWidget::ConvertValueInRangeToSliderPosition(double value) {

		if (m_valuesAtSliderPosition.empty()) {

			return static_cast<int>(std::floor((value - m_minValidator->bottom()) / m_ratio));
		}
		else {

			SliderPositionValues::iterator iT = std::lower_bound(m_valuesAtSliderPosition.begin(), m_valuesAtSliderPosition.end(), value);
			return std::distance(m_valuesAtSliderPosition.begin(), iT);
		}
	}

	void SingleNumericRangeFilterWidget::SetMinLineEditBlockSignals(double min) {

		m_minLineEdit->blockSignals(true);
		SetMinLineEdit(min);
		m_minLineEdit->blockSignals(false);
	}

	void SingleNumericRangeFilterWidget::SetMaxLineEditBlockSignals(double max) {

		m_maxLineEdit->blockSignals(true);
		SetMaxLineEdit(max);
		m_maxLineEdit->blockSignals(false);
	}

	void SingleNumericRangeFilterWidget::SetMinLineEdit(double val) {

		m_minLineEdit->setText(m_fieldProperties.transformData(ConvertDoubleToString(val, m_minValidator->decimals())));
	}

	double SingleNumericRangeFilterWidget::GetMinLineEdit() const {

		QString lineEditText = m_minLineEdit->text();
		if (lineEditText.isEmpty()) {

			return std::numeric_limits<double>::lowest();
		}
		else {
			double to_return = lineEditText.toDouble();
			if (to_return == 0.0){
				SliderPositionValues::const_iterator iT = m_valuesAtSliderPosition.begin();
				std::advance(iT, m_rangeSlider->GetLowerPosition());
				to_return = *iT;
			}
			return to_return;
		}
	}

	void SingleNumericRangeFilterWidget::SetMaxLineEdit(double val) {

		m_maxLineEdit->setText(m_fieldProperties.transformData(ConvertDoubleToString(val, m_maxValidator->decimals())));
	}

	double SingleNumericRangeFilterWidget::GetMaxLineEdit() const {

		QString lineEditText = m_maxLineEdit->text();
		if (lineEditText.isEmpty()) {

			return std::numeric_limits<double>::max();
		}
		else {
			double to_return = lineEditText.toDouble();
			if (to_return == 0.0){
				SliderPositionValues::const_iterator iT = m_valuesAtSliderPosition.begin();
				std::advance(iT, m_rangeSlider->GetUpperPosition());
				to_return = *iT;
			}
			return to_return;
		}
	}

	QString SingleNumericRangeFilterWidget::ConvertDoubleToString(double val, int decimals) const {

		QString str = QString::number(val, 'f', decimals);
		while (str.endsWith('0')) {

			str.chop(1);
		}
		if (str.endsWith('.')) {

			str.chop(1);
		}

		return str;
	}

} //namespace SynGlyphX