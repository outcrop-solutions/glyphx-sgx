#include "singlenumericrangefilterwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

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

		m_minLineEdit = new QLineEdit(this);
		m_minValidator = new QDoubleValidator(m_minLineEdit);
		m_minLineEdit->setValidator(m_minValidator);
		mainLayout->addWidget(m_minLineEdit, 1);

		m_rangeSlider = new RangeSlider(orientation, this);
		m_rangeSlider->setRange(0, 99);
		m_rangeSlider->setTracking(true);
		mainLayout->addWidget(m_rangeSlider, 3);

		m_maxLineEdit = new QLineEdit(this);
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

		SetMaxRangeExtents(DegenerateInterval(0.0, 99.0));
	}

	SingleNumericRangeFilterWidget::~SingleNumericRangeFilterWidget()
	{

	}

	void SingleNumericRangeFilterWidget::SetMaxRangeExtents(const DegenerateInterval& range) {

		double min = range.GetMin();
		double max = range.GetMax();

		bool wasRangeUpdated = false;

		m_minLineEdit->blockSignals(true);
		m_maxLineEdit->blockSignals(true);
		m_rangeSlider->blockSignals(true);

		if ((GetMinLineEdit() < min) || (GetMaxLineEdit() > max)) {

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

	DegenerateInterval SingleNumericRangeFilterWidget::GetMaxRangeExtents() const {

		return (DegenerateInterval(m_minValidator->bottom(), m_maxValidator->top()));
	}

	void SingleNumericRangeFilterWidget::SetRange(const DegenerateInterval& range) {

		if (m_ratio == 0.0) {

			return;
		}

		blockSignals(true);
		SetMinLineEdit(range.GetMin());
		SetMaxLineEdit(range.GetMax());
		OnMinLineEditUpdated();
		OnMaxLineEditUpdated();
		blockSignals(false);

		EmitRangeUpdate();
	}

	DegenerateInterval SingleNumericRangeFilterWidget::GetRange() const {

		return (DegenerateInterval(GetMinLineEdit(), GetMaxLineEdit()));
	}

	void SingleNumericRangeFilterWidget::OnSliderLowerUpdated(int lower) {

		SetMinLineEditBlockSignals(ConvertSliderPositionToValueInRange(lower));
	}

	void SingleNumericRangeFilterWidget::OnSliderUpperUpdated(int upper) {

		SetMaxLineEditBlockSignals(ConvertSliderPositionToValueInRange(upper));
	}

	void SingleNumericRangeFilterWidget::OnMinLineEditUpdated() {

		double min = GetMinLineEdit();

		m_rangeSlider->blockSignals(true);
		m_rangeSlider->SetLowerValue(ConvertValueInRangeToSliderPosition(min));
		m_rangeSlider->blockSignals(false);

		if (min > GetMaxLineEdit()) {

			SetMaxLineEditBlockSignals(min);
		}

		EmitRangeUpdate();
	}

	void SingleNumericRangeFilterWidget::OnMaxLineEditUpdated() {

		double max = GetMaxLineEdit();

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

		return ((m_ratio * value) + m_minValidator->bottom());
	}

	int SingleNumericRangeFilterWidget::ConvertValueInRangeToSliderPosition(double value) {

		return static_cast<int>(std::floor((value - m_minValidator->bottom()) / m_ratio));
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

		m_minLineEdit->setText(ConvertDoubleToString(val, m_minValidator->decimals()));
	}

	double SingleNumericRangeFilterWidget::GetMinLineEdit() const {

		return m_minLineEdit->text().toDouble();
	}

	void SingleNumericRangeFilterWidget::SetMaxLineEdit(double val) {

		m_maxLineEdit->setText(ConvertDoubleToString(val, m_maxValidator->decimals()));
	}

	double SingleNumericRangeFilterWidget::GetMaxLineEdit() const {

		return m_maxLineEdit->text().toDouble();
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