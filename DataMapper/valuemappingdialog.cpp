#include "valuemappingdialog.h"
#include "glyphcolor.h"
#include "range.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>

ValueMappingDialog::ValueMappingDialog(InputType input, OutputType output, QWidget *parent)
	: QDialog(parent),
	m_input(input),
	m_output(output),
	m_defaultDoubleWidget(nullptr),
	m_defaultColorWidget(nullptr),
	m_table(nullptr),
	m_inputRangeWidget(nullptr),
	m_inputDoubleWidget(nullptr),
	m_inputTextWidget(nullptr)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout* defaultLayout = new QHBoxLayout(this);
	defaultLayout->addWidget(new QLabel(tr("Default:")));
	if (m_output == OutputType::Numeric) {

		m_defaultDoubleWidget = new QDoubleSpinBox(this);
		m_defaultDoubleWidget->setDecimals(3);
		defaultLayout->addWidget(m_defaultDoubleWidget);
	}
	else {

		m_defaultColorWidget = new SynGlyphX::ColorButton(false, this);
		defaultLayout->addWidget(m_defaultColorWidget);
	}
	defaultLayout->addStretch(1);
	layout->addLayout(defaultLayout);

	m_table = new QTableWidget(this);
	m_table->setColumnCount(2);

	QStringList headers;
	if (m_input == InputType::Range) {

		headers << "Range";
	}
	else {

		headers << "Key";
	}
	headers << "Value";

	m_table->setHorizontalHeaderLabels(headers);
	m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->sortByColumn(0, Qt::SortOrder::AscendingOrder);
	m_table->setSortingEnabled(true);
	QObject::connect(m_table, &QTableWidget::itemSelectionChanged, this, &ValueMappingDialog::OnTableSelectionChanged);
	layout->addWidget(m_table);

	QHBoxLayout* clearRemoveLayout = new QHBoxLayout(this);

	m_removeEntryButton = new QPushButton(tr("Remove"), this);
	QObject::connect(m_removeEntryButton, &QPushButton::clicked, this, &ValueMappingDialog::OnRemoveKeyValue);
	clearRemoveLayout->addWidget(m_removeEntryButton);

	QPushButton* clearAllButton = new QPushButton(tr("ClearAll"), this);
	QObject::connect(clearAllButton, &QPushButton::clicked, this, &ValueMappingDialog::OnClearAllKeyValues);
	clearRemoveLayout->addWidget(clearAllButton);

	clearRemoveLayout->addStretch(1);

	layout->addLayout(clearRemoveLayout);

	QString groupBoxName;
	if (m_input == InputType::Range) {

		groupBoxName = tr("New Range Value Pair");
	}
	else {

		groupBoxName = tr("New Key Value Pair");
	}
	QGroupBox* inputGroupBox = new QGroupBox(groupBoxName, this);
	QHBoxLayout* inputLayout = new QHBoxLayout(inputGroupBox);

	if (m_input == InputType::Range) {

		m_inputRangeWidget = new SynGlyphX::RangeWidget(this);
		inputLayout->addWidget(m_inputRangeWidget);
	}
	else if (m_input == InputType::Text) {

		inputLayout->addWidget(new QLabel(tr("Key:")));
		m_inputTextWidget = new QLineEdit(this);
		inputLayout->addWidget(m_inputTextWidget);
	}
	else {
		
		inputLayout->addWidget(new QLabel(tr("Key:")));
		m_inputDoubleWidget = new QDoubleSpinBox(this);
		inputLayout->addWidget(m_inputDoubleWidget);
	}
	inputLayout->addWidget(new QLabel(tr("Value:")));
	if (m_output == OutputType::Numeric) {

		m_outputDoubleWidget = new QDoubleSpinBox(this);
		m_outputDoubleWidget->setDecimals(3);
		inputLayout->addWidget(m_outputDoubleWidget);
	}
	else {

		m_outputColorWidget = new SynGlyphX::ColorButton(false, this);
		inputLayout->addWidget(m_outputColorWidget);
	}

	QPushButton* addEntryButton = new QPushButton(tr("Add"), this);
	QObject::connect(addEntryButton, &QPushButton::clicked, this, &ValueMappingDialog::OnAddKeyValue);
	inputLayout->addWidget(addEntryButton);

	inputGroupBox->setLayout(inputLayout);
	layout->addWidget(inputGroupBox);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	layout->addWidget(dialogButtonBox);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ValueMappingDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ValueMappingDialog::reject);

	setLayout(layout);

	setWindowTitle(tr("Edit Mapping Properties"));
}

ValueMappingDialog::~ValueMappingDialog()
{

}

void ValueMappingDialog::OnAddKeyValue() {

	if (m_input == InputType::Numeric) {

		double newKey = m_inputDoubleWidget->value();
		if (m_doubleInputValues.count(newKey) == 1) {

			QMessageBox::warning(this, tr("Error adding new key value pair"), tr("Key value can not the same as a key already in the list"));
			return;
		}

		m_doubleInputValues.insert(newKey);
	}
	else if (m_input == InputType::Range) {

		SynGlyphX::Range newKey = m_inputRangeWidget->GetRange();
		if (m_rangeInputValues.count(newKey) == 1) {

			QMessageBox::warning(this, tr("Error adding new range value pair"), tr("Range value can not overlap with a range already in the list"));
			return;
		}

		m_rangeInputValues.insert(newKey);
	}
	else {

		std::wstring newKey = m_inputTextWidget->text().toStdWString();

		//Line edit shouldn't be empty
		if (newKey.empty()) {

			QMessageBox::warning(this, tr("Error adding new key value pair"), tr("Key value can not be empty"));
			return;
		}

		if (m_textInputValues.count(newKey) == 1) {

			QMessageBox::warning(this, tr("Error adding new key value pair"), tr("Key value can not the same as a key already in the list"));
			return;
		}

		m_textInputValues.insert(newKey);
	}

	AddRow();

	int row = m_table->rowCount() - 1;
	if (m_input == InputType::Numeric) {

		QDoubleSpinBox* inputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 0));
		inputTableWidget->setValue(m_inputDoubleWidget->value());
	}
	else if (m_input == InputType::Range) {

		SynGlyphX::RangeWidget* inputTableWidget = dynamic_cast<SynGlyphX::RangeWidget*>(m_table->cellWidget(row, 0));
		inputTableWidget->SetRange(m_inputRangeWidget->GetRange());
	}
	else {

		QLineEdit* inputTableWidget = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 0));
		inputTableWidget->setText(m_inputTextWidget->text());
	}

	if (m_output == OutputType::Numeric) {

		QDoubleSpinBox* outputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 1));
		outputTableWidget->setValue(m_outputDoubleWidget->value());
	}
	else {

		SynGlyphX::ColorButton* outputTableWidget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetColor(m_outputColorWidget->GetColor());
	}
}

void ValueMappingDialog::OnRemoveKeyValue() {

	int row = m_table->currentRow();

	if (m_input == InputType::Numeric) {

		QDoubleSpinBox* keyWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 0));
		m_doubleInputValues.erase(keyWidget->value());
	}
	else if (m_input == InputType::Range) {

		SynGlyphX::RangeWidget* keyWidget = dynamic_cast<SynGlyphX::RangeWidget*>(m_table->cellWidget(row, 0));
		m_rangeInputValues.erase(keyWidget->GetRange());
	}
	else {

		QLineEdit* keyWidget = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 0));
		m_textInputValues.erase(keyWidget->text().toStdWString());
	}

	m_table->removeRow(row);
}

void ValueMappingDialog::OnTableSelectionChanged() {

	m_removeEntryButton->setEnabled(!m_table->selectedItems().empty());
}

void ValueMappingDialog::AddRow() {

	int row = m_table->rowCount();
	m_table->insertRow(row);

	QWidget* inputWidget = nullptr;
	if (m_input == InputType::Numeric) {

		inputWidget = new QDoubleSpinBox(this);
	}
	else if (m_input == InputType::Range) {

		inputWidget = new SynGlyphX::RangeWidget(this);
	}
	else {

		inputWidget = new QLineEdit(this);
	}

	inputWidget->setContentsMargins(0, 0, 0, 0);
	m_table->setCellWidget(row, 0, inputWidget);

	QWidget* outputWidget = nullptr;
	if (m_output == OutputType::Numeric) {

		outputWidget = new QDoubleSpinBox(this);
	}
	else {

		outputWidget = new SynGlyphX::ColorButton(this);
	}

	outputWidget->setContentsMargins(0, 0, 0, 0);
	m_table->setCellWidget(row, 1, outputWidget);
}

void ValueMappingDialog::OnClearAllKeyValues() {

	m_table->clear();
	m_doubleInputValues.clear();
	m_textInputValues.clear();
	m_rangeInputValues.clear();
}

////////////////////////////////////////////////////////////////////////////////

Numeric2NumericMappingDialog::Numeric2NumericMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Numeric, OutputType::Numeric, parent) {

}

Numeric2NumericMappingDialog::~Numeric2NumericMappingDialog() {

}

void Numeric2NumericMappingDialog::SetDialogFromMapping(SynGlyphX::Numeric2NumericMappingData::ConstSharedPtr mapping) {

	m_defaultDoubleWidget->setValue(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		QDoubleSpinBox* inputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 0));
		inputTableWidget->setValue(keyValuePair.first);

		QDoubleSpinBox* outputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 1));
		outputTableWidget->setValue(keyValuePair.second);
	}
}

SynGlyphX::Numeric2NumericMappingData::SharedPtr Numeric2NumericMappingDialog::GetMappingFromDialog() const {

	SynGlyphX::Numeric2NumericMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Numeric2NumericMappingData>();

	mapping->SetDefaultValue(m_defaultDoubleWidget->value());

	for (int row = 0; row < m_table->rowCount(); ++row) {

		QDoubleSpinBox* inputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 0));
		QDoubleSpinBox* outputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 1));
		
		mapping->AddKeyValueIntoMap(m_inputDoubleWidget->value(), m_outputDoubleWidget->value());
	}

	return mapping;
}

////////////////////////////////////////////////////////////////////////////////

Numeric2ColorMappingDialog::Numeric2ColorMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Numeric, OutputType::Color, parent) {

}

Numeric2ColorMappingDialog::~Numeric2ColorMappingDialog() {

}

void Numeric2ColorMappingDialog::SetDialogFromMapping(SynGlyphX::Numeric2ColorMappingData::ConstSharedPtr mapping) {

	m_defaultColorWidget->SetColor(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		QDoubleSpinBox* inputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 0));
		inputTableWidget->setValue(keyValuePair.first);

		SynGlyphX::ColorButton* outputTableWidget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetColor(keyValuePair.second);
	}
}

SynGlyphX::Numeric2ColorMappingData::SharedPtr Numeric2ColorMappingDialog::GetMappingFromDialog() const {

	SynGlyphX::Numeric2ColorMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Numeric2ColorMappingData>();

	mapping->SetDefaultValue(SynGlyphX::ColorButton::ConvertQColorToColor(m_defaultColorWidget->GetColor()));

	for (int row = 0; row < m_table->rowCount(); ++row) {

		QDoubleSpinBox* inputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 0));
		SynGlyphX::ColorButton* outputTableWidget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, 1));

		mapping->AddKeyValueIntoMap(inputTableWidget->value(), SynGlyphX::ColorButton::ConvertQColorToColor(outputTableWidget->GetColor()));
	}

	return mapping;
}

////////////////////////////////////////////////////////////////////////////////

Text2NumericMappingDialog::Text2NumericMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Text, OutputType::Numeric, parent) {

}

Text2NumericMappingDialog::~Text2NumericMappingDialog() {


}

////////////////////////////////////////////////////////////////////////////////

Text2ColorMappingDialog::Text2ColorMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Text, OutputType::Color, parent) {

}

Text2ColorMappingDialog::~Text2ColorMappingDialog() {


}

////////////////////////////////////////////////////////////////////////////////

Range2NumericMappingDialog::Range2NumericMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Range, OutputType::Numeric, parent) {

}

Range2NumericMappingDialog::~Range2NumericMappingDialog() {


}

////////////////////////////////////////////////////////////////////////////////

Range2ColorMappingDialog::Range2ColorMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Range, OutputType::Color, parent) {

}

Range2ColorMappingDialog::~Range2ColorMappingDialog() {


}