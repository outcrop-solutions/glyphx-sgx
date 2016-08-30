#include "interpolationmappingdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QStackedLayout>
#include "groupboxsinglewidget.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include "datatransformmodel.h"

class RangeBoundWidget : public QWidget
{
public:
	RangeBoundWidget(QWidget* parent) : QWidget(parent)
	{
		m_minBound = new QDoubleSpinBox(this);
		m_minBound->setRange((double)std::numeric_limits<int>::min(), (double)std::numeric_limits<int>::max());
		m_maxBound = new QDoubleSpinBox(this);
		m_maxBound->setRange((double)std::numeric_limits<int>::min(), (double)std::numeric_limits<int>::max());

		m_minValue = new QDoubleSpinBox(this);
		m_minValue->setRange((double)std::numeric_limits<int>::min(), (double)std::numeric_limits<int>::max());
		m_maxValue = new QDoubleSpinBox(this);
		m_maxValue->setRange((double)std::numeric_limits<int>::min(), (double)std::numeric_limits<int>::max());

		//m_minBound->setRange()

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		QHBoxLayout* minLayout = new QHBoxLayout;

		minLayout->addWidget(new QLabel(tr("Min:"), this), 0, Qt::AlignRight);
		minLayout->addWidget(m_minBound, 0, Qt::AlignLeft);
		minLayout->addWidget(new QLabel(tr("Value:"), this), 0, Qt::AlignRight);
		minLayout->addWidget(m_minValue, 0, Qt::AlignLeft);

		QHBoxLayout* maxLayout = new QHBoxLayout;
		maxLayout->addWidget(new QLabel(tr("Max:"), this), 0, Qt::AlignRight);
		maxLayout->addWidget(m_maxBound, 0, Qt::AlignLeft);
		maxLayout->addWidget(new QLabel(tr("Value:"), this), 0, Qt::AlignRight);
		maxLayout->addWidget(m_maxValue, 0, Qt::AlignLeft);

		mainLayout->addLayout(minLayout);
		mainLayout->addLayout(maxLayout);

		setLayout(mainLayout);
	}
	QDoubleSpinBox* m_minBound;
	QDoubleSpinBox* m_maxBound;

	QDoubleSpinBox* m_minValue;
	QDoubleSpinBox* m_maxValue;
};

InterpolationMappingDialog::InterpolationMappingDialog(SynGlyphX::DataTransformModel* model, QWidget *parent)
	: QDialog(parent),
	m_model(model),
	m_isInterpretationLogarithmic(false)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QStringList interpolationOptionsList;
	interpolationOptionsList << tr("Get Min && Max From Bound Input Field") << tr("User Specified Min && Max") << tr("Get Min && Max From Group Of Input Fields") <<
		tr("Range Bound Min && Max");
	m_minMaxTypeWidget = new SynGlyphX::RadioButtonGroupWidget(interpolationOptionsList, Qt::Vertical, this);

	SynGlyphX::GroupBoxSingleWidget* interpolationOptionsGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Input Min/Max Type:"), m_minMaxTypeWidget, this);

	mainLayout->addWidget(interpolationOptionsGroupBox);

	QStackedLayout* minMaxParameterWidgetsLayout = new QStackedLayout(this);
	minMaxParameterWidgetsLayout->setContentsMargins(0, 0, 0, 0);
	minMaxParameterWidgetsLayout->addWidget(new QWidget(this));

	m_userSpecifiedMinMaxWidget = new SynGlyphX::DoubleMinMaxWidget(this);
	
	//For the range of this widget we want a large range, but nothing so large it makes the widget huge.  Using numeric limits for int (even though the widget uses
	//double values) is a good compromise
	m_userSpecifiedMinMaxWidget->SetRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	m_userSpecifiedMinMaxWidget->SetDecimals(4);
	m_userSpecifiedMinMaxWidget->SetValue(SynGlyphX::DoubleMinDiff(0.0, 100.0));

	minMaxParameterWidgetsLayout->addWidget(m_userSpecifiedMinMaxWidget);

	if (!m_model->GetNumericFieldsByTable().empty()) {

		m_fieldGroupWidget = new FieldGroupWidget(m_model, this);
		minMaxParameterWidgetsLayout->addWidget(m_fieldGroupWidget);
	}
	else {

		m_minMaxTypeWidget->SetButtonEnabled(2, false);
	}

	m_rangeBoundWidget = new RangeBoundWidget(this);
	minMaxParameterWidgetsLayout->addWidget(m_rangeBoundWidget);

	mainLayout->addLayout(minMaxParameterWidgetsLayout);
	QObject::connect(m_minMaxTypeWidget, &SynGlyphX::RadioButtonGroupWidget::ButtonClicked, minMaxParameterWidgetsLayout, &QStackedLayout::setCurrentIndex);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &InterpolationMappingDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &InterpolationMappingDialog::reject);

	setLayout(mainLayout);

	setWindowTitle(tr("Edit Interpolation Mapping Properties"));
}

InterpolationMappingDialog::~InterpolationMappingDialog()
{

}

void InterpolationMappingDialog::accept() {

	SynGlyphX::InterpolationMappingData::InputMinMaxType inputMinMaxType = static_cast<SynGlyphX::InterpolationMappingData::InputMinMaxType>(m_minMaxTypeWidget->GetCheckedButton());
	if ((inputMinMaxType == SynGlyphX::InterpolationMappingData::InputMinMaxType::InputFieldGroup) && (m_fieldGroupWidget->GetCurrentGroupName().isEmpty())) {

		QMessageBox::warning(this, tr("No field group selected"), tr("No group of fields has been selected.  Either select a group of fields or change the min & max type to user specified or bound input field."));
	}
	else {

		QDialog::accept();
	}
}

void InterpolationMappingDialog::SetDialogFromMapping(SynGlyphX::InterpolationMappingData::ConstSharedPtr mapping) {

	m_isInterpretationLogarithmic = (mapping->GetFunction() == SynGlyphX::MappingFunctionData::Function::LogarithmicInterpolation);

	SynGlyphX::InterpolationMappingData::InputMinMaxType minMaxType = mapping->GetInputMinMaxType();
	m_minMaxTypeWidget->SetCheckedButton(minMaxType);

	if (minMaxType == SynGlyphX::InterpolationMappingData::InputMinMaxType::UserSpecified) {

		m_userSpecifiedMinMaxWidget->SetValue(mapping->GetUserSpecifiedInputMinMax());
	}
	else if (minMaxType == SynGlyphX::InterpolationMappingData::InputMinMaxType::InputFieldGroup) {

		m_fieldGroupWidget->SetCurrentGroupName(QString::fromStdWString(mapping->GetInputMinMaxFieldGroup()));
	}
	else if (minMaxType == SynGlyphX::InterpolationMappingData::InputMinMaxType::RangeBound) {

		auto range = mapping->GetRangeBound();
		m_rangeBoundWidget->m_minBound->setValue(range.min);
		m_rangeBoundWidget->m_minValue->setValue(range.minValue);

		m_rangeBoundWidget->m_maxBound->setValue(range.max);
		m_rangeBoundWidget->m_maxValue->setValue(range.maxValue);
	}
}

SynGlyphX::InterpolationMappingData::SharedPtr InterpolationMappingDialog::GetMappingFromDialog() const {

	SynGlyphX::InterpolationMappingData::SharedPtr newMappingData = std::make_shared<SynGlyphX::InterpolationMappingData>(m_isInterpretationLogarithmic);

	SynGlyphX::InterpolationMappingData::InputMinMaxType inputMinMaxType = static_cast<SynGlyphX::InterpolationMappingData::InputMinMaxType>(m_minMaxTypeWidget->GetCheckedButton());
	if (inputMinMaxType == SynGlyphX::InterpolationMappingData::InputMinMaxType::BoundInputField) {

		newMappingData->SetInputMinMaxToBoundField();
	}
	else if (inputMinMaxType == SynGlyphX::InterpolationMappingData::InputMinMaxType::UserSpecified) {

		newMappingData->SetUserSpecifiedInputMinMax(m_userSpecifiedMinMaxWidget->GetValue());
	}
	else if (inputMinMaxType == SynGlyphX::InterpolationMappingData::InputMinMaxType::InputFieldGroup) {

		newMappingData->SetInputMinMaxFieldGroup(m_fieldGroupWidget->GetCurrentGroupName().toStdWString());
	}
	else if (inputMinMaxType == SynGlyphX::InterpolationMappingData::InputMinMaxType::RangeBound) {

		newMappingData->SetRangeBound(m_rangeBoundWidget->m_minBound->value(), m_rangeBoundWidget->m_minValue->value(),
			m_rangeBoundWidget->m_maxBound->value(), m_rangeBoundWidget->m_maxValue->value());
	}

	return newMappingData;
}