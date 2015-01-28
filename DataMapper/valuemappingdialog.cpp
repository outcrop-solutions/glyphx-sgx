#include "valuemappingdialog.h"
#include "glyphcolor.h"
#include "range.h"

ValueMappingDialog::ValueMappingDialog(QWidget *parent)
	: QDialog(parent),
	m_defaultDoubleWidget(nullptr),
	m_defaultColorWidget(nullptr),
	m_table(nullptr)
{
	SetupDialog();
}

ValueMappingDialog::~ValueMappingDialog()
{

}
/*
template <>
void ValueMappingDialog<double, double, double>::SetupDialog() {

	m_defaultDoubleWidget = new QDoubleSpinBox(this);
}

template <>
void ValueMappingDialog<double, std::wstring, std::wstring>::SetupDialog() {

	m_defaultDoubleWidget = new QDoubleSpinBox(this);
}

template <>
void ValueMappingDialog<SynGlyphX::GlyphColor, double, double>::SetupDialog() {

	m_defaultColorWidget = new SynGlyphX::ColorButton(false, this);
}

template <>
void ValueMappingDialog<SynGlyphX::GlyphColor, std::wstring, std::wstring>::SetupDialog() {

	m_defaultColorWidget = new SynGlyphX::ColorButton(false, this);
}

template <>
void ValueMappingDialog<double, double, SynGlyphX::Range>::SetupDialog() {

	m_defaultDoubleWidget = new QDoubleSpinBox(this);
}

template <>
void ValueMappingDialog<SynGlyphX::GlyphColor, double, SynGlyphX::Range>::SetupDialog() {

	m_defaultColorWidget = new SynGlyphX::ColorButton(false, this);
}

template <typename OutputType, typename InputType, typename KeyType>
void ValueMappingDialog<OutputType, InputType, KeyType>::SetupDialogLayout(QWidget* defaultWidget, QHBoxLayout* editLayout) {


}
*/