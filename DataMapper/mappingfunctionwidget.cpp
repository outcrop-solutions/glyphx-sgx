#include "mappingfunctionwidget.h"
#include "datamappingfunction.h"
#include <QtWidgets/QHBoxLayout>
#include "valuemappingdialog.h"

MappingFunctionWidget::MappingFunctionWidget(MinMaxGlyphModel* model, int row, QWidget *parent)
	: QWidget(parent),
	m_model(model),
	m_row(row)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(1, 1, 1, 1);

	m_functionComboBox = new QComboBox(this);
	for (SynGlyphX::MappingFunctionData::FunctionBimap::const_iterator iT = SynGlyphX::MappingFunctionData::s_functionNames.begin(); iT != SynGlyphX::MappingFunctionData::s_functionNames.end(); ++iT) {

		m_functionComboBox->addItem(QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(iT->left)), iT->left);
	}
	m_functionComboBox->setCurrentIndex(0);
	QObject::connect(m_functionComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MappingFunctionWidget::OnFunctionComboBoxChanged);
	layout->addWidget(m_functionComboBox);

	m_editPropertiesButton = new QPushButton(tr("Edit Properties"), this);
	m_editPropertiesButton->setEnabled(false);
	QObject::connect(m_editPropertiesButton, &QPushButton::clicked, this, &MappingFunctionWidget::OnEditPropertiesClicked);
	layout->addWidget(m_editPropertiesButton);

	setLayout(layout);
}

MappingFunctionWidget::~MappingFunctionWidget()
{

}

QString MappingFunctionWidget::GetFunction() const {

	return m_functionComboBox->currentText();
}

void MappingFunctionWidget::SetFunction(const QString& function) {

	if (m_functionComboBox->currentText() != function) {

		m_functionComboBox->setCurrentText(function);
		OnFunctionComboBoxChanged();
	}
}

void MappingFunctionWidget::OnFunctionComboBoxChanged() {

	SynGlyphX::MappingFunctionData::Function function = static_cast<SynGlyphX::MappingFunctionData::Function>(m_functionComboBox->currentData().toInt());
	bool isNotInterpolation = ((function != SynGlyphX::MappingFunctionData::LinearInterpolation) && (function != SynGlyphX::MappingFunctionData::LogarithmicInterpolation));
	m_editPropertiesButton->setEnabled(isNotInterpolation);

	emit FunctionChanged();

	if (function == SynGlyphX::MappingFunctionData::Function::Text2Value) {

		emit SupportedInputChanged(SynGlyphX::MappingFunctionData::Input::Text);
	}
	else {

		emit SupportedInputChanged(SynGlyphX::MappingFunctionData::Input::Numeric);
	}
}

void MappingFunctionWidget::OnEditPropertiesClicked() {

	SynGlyphX::MappingFunctionData::ConstSharedPtr mappingData = m_model->GetMappingFunction(m_row);
	SynGlyphX::MappingFunctionData::Function function = static_cast<SynGlyphX::MappingFunctionData::Function>(m_functionComboBox->currentData().toInt());
	SynGlyphX::MappingFunctionData::SharedPtr newMappingData;
	
	if (mappingData->GetSupportedOutput() == SynGlyphX::MappingFunctionData::Output::Color) {

		if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Numeric2Value) {

			Numeric2ColorMappingDialog dialog(this);
			dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Numeric2ColorMappingData>(mappingData));
			if (dialog.exec() == QDialog::Accepted) {
				
				newMappingData = dialog.GetMappingFromDialog();
			}
		}
		else if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Text2Value) {

			Text2ColorMappingDialog dialog(this);
			dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Text2ColorMappingData>(mappingData));
			if (dialog.exec() == QDialog::Accepted) {
				
				newMappingData = dialog.GetMappingFromDialog();
			}
		}
		else if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Range2Value) {

			Range2ColorMappingDialog dialog(this);
			dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Range2ColorMappingData>(mappingData));
			if (dialog.exec() == QDialog::Accepted) {
				
				newMappingData = dialog.GetMappingFromDialog();
			}
		}
	}
	else {

		if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Numeric2Value) {

			Numeric2NumericMappingDialog dialog(this);
			dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Numeric2NumericMappingData>(mappingData));
			if (dialog.exec() == QDialog::Accepted) {
				
				newMappingData = dialog.GetMappingFromDialog();
			}
		}
		else if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Text2Value) {

			Text2NumericMappingDialog dialog(this);
			dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Text2NumericMappingData>(mappingData));
			if (dialog.exec() == QDialog::Accepted) {
				
				newMappingData = dialog.GetMappingFromDialog();
			}
		}
		else if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Range2Value) {

			Range2NumericMappingDialog dialog(this);
			dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Range2NumericMappingData>(mappingData));
			if (dialog.exec() == QDialog::Accepted) {
				
				newMappingData = dialog.GetMappingFromDialog();
			}
		}
	}

	if (newMappingData) {

		m_model->SetMappingFunction(m_row, newMappingData);
	}
}