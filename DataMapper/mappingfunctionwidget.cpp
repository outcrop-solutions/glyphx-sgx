#include "mappingfunctionwidget.h"
#include "datamappingfunction.h"
#include <QtWidgets/QHBoxLayout>
#include "valuemappingdialog.h"
#include "interpolationmappingdialog.h"

const QStringList MappingFunctionWidget::s_numericColorFunctions = MappingFunctionWidget::CreateNumericColorFunctionList();
const QStringList MappingFunctionWidget::s_enumerationFunctions = MappingFunctionWidget::CreateEnumerationFunctionList();

MappingFunctionWidget::MappingFunctionWidget(KeyType keyType, GlyphRolesTableModel* model, int row, QWidget *parent)
	: QWidget(parent),
	m_keyType(keyType),
	m_model(model),
	m_row(row),
	m_dialogOutputMin(-100000.0),
	m_dialogOutputMax(100000.0)
{
	QHBoxLayout* layout = new QHBoxLayout(this);

	m_functionComboBox = new QComboBox(this);
	if ((m_keyType == KeyType::Numeric) || (m_keyType == KeyType::Color)) {

		m_functionComboBox->addItems(s_numericColorFunctions);
	}
	else {

		m_functionComboBox->addItems(s_enumerationFunctions);
	}
	m_functionComboBox->setCurrentIndex(0);
	QObject::connect(m_functionComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MappingFunctionWidget::OnFunctionComboBoxChangedByUser);
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

void MappingFunctionWidget::SetDialogOutputMinMax(double min, double max) {

	m_dialogOutputMin = min;
	m_dialogOutputMax = max;
}

void MappingFunctionWidget::SetFunction(const QString& function) {

	if (m_functionComboBox->currentText() != function) {

		bool blockSignals = m_functionComboBox->signalsBlocked();
		m_functionComboBox->blockSignals(true);
		m_functionComboBox->setCurrentText(function);
		OnFunctionComboBoxChanged();
		m_functionComboBox->blockSignals(blockSignals);
	}
}

void MappingFunctionWidget::OnFunctionComboBoxChangedByUser() {

	OnFunctionComboBoxChanged();
	emit FunctionChanged();
}

void MappingFunctionWidget::OnFunctionComboBoxChanged() {

	SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(m_functionComboBox->currentText().toStdWString());
	bool hasProperties = (function != SynGlyphX::MappingFunctionData::None);
	m_editPropertiesButton->setEnabled(hasProperties);

	if (function == SynGlyphX::MappingFunctionData::Function::Text2Value) {

		emit SupportedInputChanged(SynGlyphX::MappingFunctionData::Input::Text);
	}
	else {

		emit SupportedInputChanged(SynGlyphX::MappingFunctionData::Input::Numeric);
	}
}

void MappingFunctionWidget::OnEditPropertiesClicked() {

	SynGlyphX::MappingFunctionData::ConstSharedPtr mappingData = m_model->GetMappingFunction(m_row);
	//SynGlyphX::MappingFunctionData::Function function = static_cast<SynGlyphX::MappingFunctionData::Function>(m_functionComboBox->currentData().toInt());
	SynGlyphX::MappingFunctionData::SharedPtr newMappingData;
	
	if ((mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::LinearInterpolation) || (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::LogarithmicInterpolation)) {

		InterpolationMappingDialog dialog(m_model->GetSourceModel(), this);
		dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::InterpolationMappingData>(mappingData));
		if (dialog.exec() == QDialog::Accepted) {

			newMappingData = dialog.GetMappingFromDialog();
		}
	}
	else {

		if (m_keyType == KeyType::Color) {

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
		else if (m_keyType == KeyType::Numeric) {

			if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Numeric2Value) {

				Numeric2NumericMappingDialog dialog(this);
				dialog.SetOutputSpinBoxRange(m_dialogOutputMin, m_dialogOutputMax);
				dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Numeric2NumericMappingData>(mappingData));
				if (dialog.exec() == QDialog::Accepted) {

					newMappingData = dialog.GetMappingFromDialog();
				}
			}
			else if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Text2Value) {

				Text2NumericMappingDialog dialog(this);
				dialog.SetOutputSpinBoxRange(m_dialogOutputMin, m_dialogOutputMax);
				dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Text2NumericMappingData>(mappingData));
				if (dialog.exec() == QDialog::Accepted) {

					newMappingData = dialog.GetMappingFromDialog();
				}
			}
			else if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Range2Value) {

				Range2NumericMappingDialog dialog(this);
				dialog.SetOutputSpinBoxRange(m_dialogOutputMin, m_dialogOutputMax);
				dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Range2NumericMappingData>(mappingData));
				if (dialog.exec() == QDialog::Accepted) {

					newMappingData = dialog.GetMappingFromDialog();
				}
			}
		}
		else if (m_keyType == KeyType::GeometryShape) {

			if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Numeric2Value) {

				Numeric2ShapeMappingDialog dialog(this);
				dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Numeric2ShapeMappingData>(mappingData));
				if (dialog.exec() == QDialog::Accepted) {

					newMappingData = dialog.GetMappingFromDialog();
				}
			}
			else if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Text2Value) {

				Text2ShapeMappingDialog dialog(this);
				dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Text2ShapeMappingData>(mappingData));
				if (dialog.exec() == QDialog::Accepted) {

					newMappingData = dialog.GetMappingFromDialog();
				}
			}
			else if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Range2Value) {

				Range2ShapeMappingDialog dialog(this);
				dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Range2ShapeMappingData>(mappingData));
				if (dialog.exec() == QDialog::Accepted) {

					newMappingData = dialog.GetMappingFromDialog();
				}
			}
		}
		else if (m_keyType == KeyType::VirtualTopology) {

			if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Numeric2Value) {

				Numeric2VirtualTopologyMappingDialog dialog(this);
				dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Numeric2VirtualTopologyMappingData>(mappingData));
				if (dialog.exec() == QDialog::Accepted) {

					newMappingData = dialog.GetMappingFromDialog();
				}
			}
			else if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Text2Value) {

				Text2VirtualTopologyMappingDialog dialog(this);
				dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Text2VirtualTopologyMappingData>(mappingData));
				if (dialog.exec() == QDialog::Accepted) {

					newMappingData = dialog.GetMappingFromDialog();
				}
			}
			else if (mappingData->GetFunction() == SynGlyphX::MappingFunctionData::Function::Range2Value) {

				Range2VirtualTopologyMappingDialog dialog(this);
				dialog.SetDialogFromMapping(std::dynamic_pointer_cast<const SynGlyphX::Range2VirtualTopologyMappingData>(mappingData));
				if (dialog.exec() == QDialog::Accepted) {

					newMappingData = dialog.GetMappingFromDialog();
				}
			}
		}
	}

	if (newMappingData) {

		m_model->SetMappingFunction(m_row, newMappingData);
	}
}

QStringList MappingFunctionWidget::CreateNumericColorFunctionList() {

	QStringList functions;

	for (SynGlyphX::MappingFunctionData::FunctionBimap::const_iterator iT = SynGlyphX::MappingFunctionData::s_functionNames.begin(); iT != SynGlyphX::MappingFunctionData::s_functionNames.end(); ++iT) {

		functions.append(QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(iT->left)));
	}

	return functions;
}

QStringList MappingFunctionWidget::CreateEnumerationFunctionList() {

	QStringList functions;

	for (SynGlyphX::MappingFunctionData::FunctionBimap::const_iterator iT = SynGlyphX::MappingFunctionData::s_functionNames.begin(); iT != SynGlyphX::MappingFunctionData::s_functionNames.end(); ++iT) {

		if ((iT->left != SynGlyphX::MappingFunctionData::Function::LinearInterpolation) && (iT->left != SynGlyphX::MappingFunctionData::Function::LogarithmicInterpolation)) {

			functions.append(QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(iT->left)));
		}
	}

	return functions;
}