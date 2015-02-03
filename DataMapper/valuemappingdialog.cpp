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
	m_inputTextWidget(nullptr),
	m_outputSpinBoxMin(-100000.0),
	m_outputSpinBoxMax(100000.0)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout* defaultLayout = new QHBoxLayout(this);
	defaultLayout->addWidget(new QLabel(tr("Default:")));
	if (m_output == OutputType::Numeric) {

		m_defaultDoubleWidget = CreateDoubleSpinBox();
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
		m_inputDoubleWidget = CreateDoubleSpinBox();
		inputLayout->addWidget(m_inputDoubleWidget);
	}
	inputLayout->addWidget(new QLabel(tr("Value:")));
	if (m_output == OutputType::Numeric) {

		m_outputDoubleWidget = CreateDoubleSpinBox();
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

	OnTableSelectionChanged();
	setWindowTitle(tr("Edit Mapping Properties"));
}

ValueMappingDialog::~ValueMappingDialog()
{

}

void ValueMappingDialog::SetOutputSpinBoxRange(double min, double max) {

	if (m_output == OutputType::Numeric) {

		m_outputSpinBoxMin = min;
		m_outputSpinBoxMax = max;

		m_defaultDoubleWidget->setRange(m_outputSpinBoxMin, m_outputSpinBoxMax);
		m_outputDoubleWidget->setRange(m_outputSpinBoxMin, m_outputSpinBoxMax);

		for (int i = 0; i < m_table->rowCount(); ++i) {

			QDoubleSpinBox* outputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(i, 1));
			outputTableWidget->setRange(m_outputSpinBoxMin, m_outputSpinBoxMax);
		}
	}
}

void ValueMappingDialog::accept() {

	try {

		CreateMappingData();
		QDialog::accept();
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("Failed to edit mapping properties"), tr(e.what()));
	}
}

void ValueMappingDialog::OnAddKeyValue() {

	if (m_input == InputType::Text) {

		std::wstring newKey = m_inputTextWidget->text().toStdWString();

		//Line edit shouldn't be empty
		if (newKey.empty()) {

			QMessageBox::warning(this, tr("Error adding new key value pair"), tr("Key value can not be empty"));
			return;
		}
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
	m_table->removeRow(row);
}

void ValueMappingDialog::OnTableSelectionChanged() {

	m_removeEntryButton->setEnabled(m_table->currentRow() != -1);
}

void ValueMappingDialog::AddRow() {

	int row = m_table->rowCount();
	m_table->insertRow(row);

	QWidget* inputWidget = nullptr;
	if (m_input == InputType::Numeric) {

		inputWidget = CreateDoubleSpinBox();
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

		outputWidget = CreateDoubleSpinBox(m_outputSpinBoxMin, m_outputSpinBoxMax);
	}
	else {

		outputWidget = new SynGlyphX::ColorButton(this);
	}

	outputWidget->setContentsMargins(0, 0, 0, 0);
	m_table->setCellWidget(row, 1, outputWidget);
}

void ValueMappingDialog::OnClearAllKeyValues() {

	while (m_table->rowCount() > 0) {

		m_table->removeRow(0);
	}
}

SynGlyphX::Range ValueMappingDialog::GetRangeFromWidget(int row, int column)  {

	SynGlyphX::RangeWidget* widget = dynamic_cast<SynGlyphX::RangeWidget*>(m_table->cellWidget(row, column));
	if (widget == nullptr) {

		throw std::exception("Could not get widget of given type.");
	}

	if (!widget->IsValid()) {

		throw std::exception("All ranges of range value pairs must have a minimum that is less than its maximum.");
	}

	return widget->GetRange();
}

std::wstring ValueMappingDialog::GetTextFromWidget(int row, int column) {

	QLineEdit* widget = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, column));
	if (widget == nullptr) {

		throw std::exception("Could not get widget of given type.");
	}

	QString text = widget->text();
	if (text.isEmpty()) {

		throw std::exception("All keys of key value pairs must not be empty.");
	}

	return text.toStdWString();
}

double ValueMappingDialog::GetDoubleFromWidget(int row, int column) {

	QDoubleSpinBox* widget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, column));
	if (widget == nullptr) {

		throw std::exception("Could not get widget of given type.");
	}

	return widget->value();
}

SynGlyphX::GlyphColor ValueMappingDialog::GetColorFromWidget(int row, int column) {

	SynGlyphX::ColorButton* widget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, 1));
	if (widget == nullptr) {

		throw std::exception("Could not get widget of given type.");
	}

	return SynGlyphX::ColorButton::ConvertQColorToColor(widget->GetColor());
}

QDoubleSpinBox* ValueMappingDialog::CreateDoubleSpinBox(double min, double max) {

	QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
	spinBox->setRange(min, max);
	spinBox->setDecimals(3);

	return spinBox;
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

	return m_mappingData;
}

bool Numeric2NumericMappingDialog::CreateMappingData() {

	SynGlyphX::Numeric2NumericMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Numeric2NumericMappingData>();

	mapping->SetDefaultValue(m_defaultDoubleWidget->value());

	for (int row = 0; row < m_table->rowCount(); ++row) {

		double input = GetDoubleFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::exception("At least one key is the same as another key.  All keys must be unique.");
		}

		double output = GetDoubleFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
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

	return m_mappingData;
}

bool Numeric2ColorMappingDialog::CreateMappingData() {

	SynGlyphX::Numeric2ColorMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Numeric2ColorMappingData>();

	mapping->SetDefaultValue(SynGlyphX::ColorButton::ConvertQColorToColor(m_defaultColorWidget->GetColor()));

	for (int row = 0; row < m_table->rowCount(); ++row) {

		double input = GetDoubleFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::exception("At least one key is the same as another key.  All keys must be unique.");
		}

		SynGlyphX::GlyphColor output = GetColorFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

////////////////////////////////////////////////////////////////////////////////

Text2NumericMappingDialog::Text2NumericMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Text, OutputType::Numeric, parent) {

}

Text2NumericMappingDialog::~Text2NumericMappingDialog() {


}

void Text2NumericMappingDialog::SetDialogFromMapping(SynGlyphX::Text2NumericMappingData::ConstSharedPtr mapping) {

	m_defaultDoubleWidget->setValue(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		QLineEdit* inputTableWidget = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 0));
		inputTableWidget->setText(QString::fromStdWString(keyValuePair.first));

		QDoubleSpinBox* outputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 1));
		outputTableWidget->setValue(keyValuePair.second);
	}
}

SynGlyphX::Text2NumericMappingData::SharedPtr Text2NumericMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Text2NumericMappingDialog::CreateMappingData() {

	SynGlyphX::Text2NumericMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Text2NumericMappingData>();

	mapping->SetDefaultValue(m_defaultDoubleWidget->value());

	for (int row = 0; row < m_table->rowCount(); ++row) {

		std::wstring input = GetTextFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::exception("At least one key is the same as another key.  All keys must be unique.");
		}

		double output = GetDoubleFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

////////////////////////////////////////////////////////////////////////////////

Text2ColorMappingDialog::Text2ColorMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Text, OutputType::Color, parent) {

}

Text2ColorMappingDialog::~Text2ColorMappingDialog() {


}

void Text2ColorMappingDialog::SetDialogFromMapping(SynGlyphX::Text2ColorMappingData::ConstSharedPtr mapping) {

	m_defaultColorWidget->SetColor(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		QLineEdit* inputTableWidget = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 0));
		inputTableWidget->setText(QString::fromStdWString(keyValuePair.first));

		SynGlyphX::ColorButton* outputTableWidget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetColor(keyValuePair.second);
	}
}

SynGlyphX::Text2ColorMappingData::SharedPtr Text2ColorMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Text2ColorMappingDialog::CreateMappingData() {

	SynGlyphX::Text2ColorMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Text2ColorMappingData>();

	mapping->SetDefaultValue(SynGlyphX::ColorButton::ConvertQColorToColor(m_defaultColorWidget->GetColor()));

	for (int row = 0; row < m_table->rowCount(); ++row) {

		std::wstring input = GetTextFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::exception("At least one key is the same as another key.  All keys must be unique.");
		}

		SynGlyphX::GlyphColor output = GetColorFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

////////////////////////////////////////////////////////////////////////////////

Range2NumericMappingDialog::Range2NumericMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Range, OutputType::Numeric, parent) {

}

Range2NumericMappingDialog::~Range2NumericMappingDialog() {


}

void Range2NumericMappingDialog::SetDialogFromMapping(SynGlyphX::Range2NumericMappingData::ConstSharedPtr mapping) {

	m_defaultDoubleWidget->setValue(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		SynGlyphX::RangeWidget* inputTableWidget = dynamic_cast<SynGlyphX::RangeWidget*>(m_table->cellWidget(row, 0));
		inputTableWidget->SetRange(keyValuePair.first);

		QDoubleSpinBox* outputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 1));
		outputTableWidget->setValue(keyValuePair.second);
	}
}

SynGlyphX::Range2NumericMappingData::SharedPtr Range2NumericMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Range2NumericMappingDialog::CreateMappingData() {

	SynGlyphX::Range2NumericMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Range2NumericMappingData>();

	mapping->SetDefaultValue(m_defaultDoubleWidget->value());

	for (int row = 0; row < m_table->rowCount(); ++row) {

		SynGlyphX::Range input = GetRangeFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::exception("At least one range overlaps another range.  All ranges must be distinct from each other.");
		}

		double output = GetDoubleFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

////////////////////////////////////////////////////////////////////////////////

Range2ColorMappingDialog::Range2ColorMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Range, OutputType::Color, parent) {

}

Range2ColorMappingDialog::~Range2ColorMappingDialog() {


}

void Range2ColorMappingDialog::SetDialogFromMapping(SynGlyphX::Range2ColorMappingData::ConstSharedPtr mapping) {

	m_defaultColorWidget->SetColor(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		SynGlyphX::RangeWidget* inputTableWidget = dynamic_cast<SynGlyphX::RangeWidget*>(m_table->cellWidget(row, 0));
		inputTableWidget->SetRange(keyValuePair.first);

		SynGlyphX::ColorButton* outputTableWidget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetColor(keyValuePair.second);
	}
}

SynGlyphX::Range2ColorMappingData::SharedPtr Range2ColorMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Range2ColorMappingDialog::CreateMappingData() {

	SynGlyphX::Range2ColorMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Range2ColorMappingData>();

	mapping->SetDefaultValue(SynGlyphX::ColorButton::ConvertQColorToColor(m_defaultColorWidget->GetColor()));

	for (int row = 0; row < m_table->rowCount(); ++row) {

		SynGlyphX::Range input = GetRangeFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::exception("At least one range overlaps another range.  All ranges must be distinct from each other.");
		}

		SynGlyphX::GlyphColor output = GetColorFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}