#include "interpolationmappingdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QStackedLayout>
#include "groupboxsinglewidget.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include "datatransformmodel.h"


InterpolationMappingDialog::InterpolationMappingDialog(SynGlyphX::DataTransformModel* model, QWidget *parent)
	: QDialog(parent),
	m_model(model),
	m_isInterpretationLogarithmic(false)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QStringList interpolationOptionsList;
	interpolationOptionsList << tr("Get Min && Max From Bound Input Field") << tr("User Specified Min && Max") << tr("Get Min && Max From Group Of Input Fields");
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

	return newMappingData;
}