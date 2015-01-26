#include "mappingfunctionwidget.h"
#include "datamappingfunction.h"
#include <QtWidgets/QHBoxLayout>

MappingFunctionWidget::MappingFunctionWidget(QWidget *parent)
	: QWidget(parent)
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

	m_functionComboBox->setCurrentText(function);
}

void MappingFunctionWidget::OnFunctionComboBoxChanged(int index) {

	SynGlyphX::MappingFunctionData::Function function = static_cast<SynGlyphX::MappingFunctionData::Function>(m_functionComboBox->itemData(index).toInt());
	bool isNotInterpolation = ((function != SynGlyphX::MappingFunctionData::LinearInterpolation) || (function != SynGlyphX::MappingFunctionData::LogarithmicInterpolation));
	m_editPropertiesButton->setEnabled(isNotInterpolation);

	if (function == SynGlyphX::MappingFunctionData::Function::Text2Value) {

		emit SupportedInputChanged(SynGlyphX::MappingFunctionData::Input::Text);
	}
	else {

		emit SupportedInputChanged(SynGlyphX::MappingFunctionData::Input::Numeric);
	}
}

void MappingFunctionWidget::OnEditPropertiesClicked() {


}