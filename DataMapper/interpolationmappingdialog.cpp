#include "interpolationmappingdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QStackedLayout>
#include "groupboxsinglewidget.h"
#include <QtWidgets/QLabel>

InterpolationMappingDialog::InterpolationMappingDialog(SingleGlyphRolesTableModel* model, QWidget *parent)
	: QDialog(parent),
	m_model(model),
	m_isInterpretationLogarithmic(false)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QStringList interpolationOptionsList;
	interpolationOptionsList << tr("Get Min & Max From Bound Input Field") << tr("User Specified Min & Max") << tr("Get Min & Max From Group Of Input Fields");
	m_minMaxTypeWidget = new SynGlyphX::RadioButtonGroupWidget(interpolationOptionsList, Qt::Vertical, this);

	SynGlyphX::GroupBoxSingleWidget* interpolationOptionsGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Input Min/Max Type:"), m_minMaxTypeWidget, this);

	mainLayout->addWidget(interpolationOptionsGroupBox);

	m_userSpecifiedMinMaxWidget = new SynGlyphX::DoubleMinMaxWidget(this);

	QStackedLayout* minMaxParameterWidgetsLayout = new QStackedLayout(this);
	minMaxParameterWidgetsLayout->addWidget(new QWidget(this));
	minMaxParameterWidgetsLayout->addWidget(m_userSpecifiedMinMaxWidget);
	minMaxParameterWidgetsLayout->addWidget(new QLabel(tr("No datasource table is associated with this glyph so a field group can't be selected yet."), this));

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

void InterpolationMappingDialog::SetDialogFromMapping(SynGlyphX::InterpolationMappingData::ConstSharedPtr mapping) {

	m_isInterpretationLogarithmic = (mapping->GetFunction() == SynGlyphX::MappingFunctionData::Function::LogarithmicInterpolation);

	m_minMaxTypeWidget->SetCheckedButton(mapping->GetInputMinMaxType());
	m_userSpecifiedMinMaxWidget->SetValue(mapping->GetUserSpecifiedInputMinMax());
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

		
	}

	return newMappingData;
}