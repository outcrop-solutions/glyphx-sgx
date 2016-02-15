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
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include <QtWidgets/QMessageBox>
#include "colorconverter.h"
#include "valuemappingfile.h"

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

	if (m_output == OutputType::GeometryShape) {

		m_defaultShapeWidget = new SynGlyphX::GlyphShapeComboBox(this);
		defaultLayout->addWidget(m_defaultShapeWidget);
	}
	else if (m_output == OutputType::VirtualTopology) {

		m_defaultVirtualTopologyWidget = new SynGlyphX::VirtualTopologyComboBox(this);
		defaultLayout->addWidget(m_defaultVirtualTopologyWidget);
	}
	else if (m_output == OutputType::Numeric) {

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

	QHBoxLayout* clearRemoveLoadSaveLayout = new QHBoxLayout(this);

	m_removeEntryButton = new QPushButton(tr("Remove"), this);
	QObject::connect(m_removeEntryButton, &QPushButton::clicked, this, &ValueMappingDialog::OnRemoveKeyValue);
	clearRemoveLoadSaveLayout->addWidget(m_removeEntryButton);

	QPushButton* clearAllButton = new QPushButton(tr("ClearAll"), this);
	QObject::connect(clearAllButton, &QPushButton::clicked, this, &ValueMappingDialog::OnClearAllKeyValues);
	clearRemoveLoadSaveLayout->addWidget(clearAllButton);

	clearRemoveLoadSaveLayout->addStretch(1);

	QPushButton* loadFileButton = new QPushButton(tr("Load"), this);
	QObject::connect(loadFileButton, &QPushButton::clicked, this, &ValueMappingDialog::OnLoadFromFile);
	clearRemoveLoadSaveLayout->addWidget(loadFileButton);

	m_saveToFileButton = new QPushButton(tr("Save"), this);
	QObject::connect(m_saveToFileButton, &QPushButton::clicked, this, &ValueMappingDialog::OnSaveToFile);
	clearRemoveLoadSaveLayout->addWidget(m_saveToFileButton);

	layout->addLayout(clearRemoveLoadSaveLayout);

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

	if (m_output == OutputType::GeometryShape) {

		m_outputShapeWidget = new SynGlyphX::GlyphShapeComboBox(this);
		inputLayout->addWidget(m_outputShapeWidget);
	}
	else if (m_output == OutputType::VirtualTopology) {

		m_outputVirtualTopologyWidget = new SynGlyphX::VirtualTopologyComboBox(this);
		inputLayout->addWidget(m_outputVirtualTopologyWidget);
	}
	else if (m_output == OutputType::Numeric) {

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

	OnNumberOfRowsInTableChanged();
	OnTableSelectionChanged();
	setWindowTitle(tr("Edit Value Mapping Properties"));
}

ValueMappingDialog::~ValueMappingDialog()
{

}

void ValueMappingDialog::OnNumberOfRowsInTableChanged() {

	m_saveToFileButton->setEnabled(m_table->rowCount() > 0);
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

	try {

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

		if (m_output == OutputType::GeometryShape) {

			SynGlyphX::GlyphShapeComboBox* outputTableWidget = dynamic_cast<SynGlyphX::GlyphShapeComboBox*>(m_table->cellWidget(row, 1));
			outputTableWidget->SetCurrentValue(m_outputShapeWidget->GetCurrentValue());
		}
		else if (m_output == OutputType::VirtualTopology) {

			SynGlyphX::VirtualTopologyComboBox* outputTableWidget = dynamic_cast<SynGlyphX::VirtualTopologyComboBox*>(m_table->cellWidget(row, 1));
			outputTableWidget->SetCurrentValue(m_outputVirtualTopologyWidget->GetCurrentValue());
		}
		else if (m_output == OutputType::Numeric) {

			QDoubleSpinBox* outputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 1));
			outputTableWidget->setValue(m_outputDoubleWidget->value());
		}
		else {

			SynGlyphX::ColorButton* outputTableWidget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, 1));
			outputTableWidget->SetColor(m_outputColorWidget->GetColor());
		}

		OnNumberOfRowsInTableChanged();
	}
	catch (const std::exception& e) {

		m_table->removeRow(row);
		QMessageBox::warning(this, tr("Error adding value"), tr("Error adding value: ") + e.what());
	}
}

void ValueMappingDialog::OnRemoveKeyValue() {

	int row = m_table->currentRow();
	m_table->removeRow(row);
	OnNumberOfRowsInTableChanged();
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
	if (m_output == OutputType::GeometryShape) {

		outputWidget = new SynGlyphX::GlyphShapeComboBox(this);
	}
	else if (m_output == OutputType::VirtualTopology) {

		outputWidget = new SynGlyphX::VirtualTopologyComboBox(this);
	}
	else if (m_output == OutputType::Numeric) {

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

		throw std::runtime_error("Could not get widget of given type.");
	}

	if (!widget->IsValid()) {

		throw std::runtime_error("All ranges of range value pairs must have a minimum that is less than its maximum.");
	}

	return widget->GetRange();
}

std::wstring ValueMappingDialog::GetTextFromWidget(int row, int column) {

	QLineEdit* widget = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, column));
	if (widget == nullptr) {

		throw std::runtime_error("Could not get widget of given type.");
	}

	QString text = widget->text();
	if (text.isEmpty()) {

		throw std::runtime_error("All keys of key value pairs must not be empty.");
	}

	return text.toStdWString();
}

double ValueMappingDialog::GetDoubleFromWidget(int row, int column) {

	QDoubleSpinBox* widget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, column));
	if (widget == nullptr) {

		throw std::runtime_error("Could not get widget of given type.");
	}

	return widget->value();
}

SynGlyphX::GlyphColor ValueMappingDialog::GetColorFromWidget(int row, int column) {

	SynGlyphX::ColorButton* widget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, column));
	if (widget == nullptr) {

		throw std::runtime_error("Could not get widget of given type.");
	}

	return SynGlyphX::ColorConverter::QColor2GlyphColor(widget->GetColor());
}

SynGlyphX::GlyphGeometryInfo::Shape ValueMappingDialog::GetShapeFromWidget(int row, int column) {

	SynGlyphX::GlyphShapeComboBox* widget = dynamic_cast<SynGlyphX::GlyphShapeComboBox*>(m_table->cellWidget(row, column));
	if (widget == nullptr) {

		throw std::runtime_error("Could not get widget of given type.");
	}

	return widget->GetCurrentValue();
}

SynGlyphX::VirtualTopologyInfo::Type ValueMappingDialog::GetVirtualTopologyFromWidget(int row, int column) {

	SynGlyphX::VirtualTopologyComboBox* widget = dynamic_cast<SynGlyphX::VirtualTopologyComboBox*>(m_table->cellWidget(row, column));
	if (widget == nullptr) {

		throw std::runtime_error("Could not get widget of given type.");
	}

	return widget->GetCurrentValue();
}

QDoubleSpinBox* ValueMappingDialog::CreateDoubleSpinBox(double min, double max) {

	QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
	spinBox->setRange(min, max);
	spinBox->setDecimals(3);

	return spinBox;
}

void ValueMappingDialog::OnLoadFromFile() {

	QSettings settings;
	settings.beginGroup("ValueMapping");
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Value Mappings"), settings.value("LastDir", QDir::currentPath()).toString(), "SynGlyphX Value Mapping Files (*.svm)");
	if (!filename.isEmpty()) {

		bool loadFile = true;
		if (m_table->rowCount() > 0) {

			loadFile = (QMessageBox::question(this, tr("Value Mapping Data Currently Exists"), tr("A value mapping is currently in this dialog.  Loading this file will overwrite it.  Do you wish to continue?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes);
			if (loadFile) {

				OnClearAllKeyValues();
			}
		}

		if (loadFile) {

			try {

				LoadFile(filename.toStdString());
				QFileInfo fileInfo(filename);
				settings.setValue("LastDir", fileInfo.absolutePath());
			}
			catch (const std::exception& e) {

				QMessageBox::warning(this, tr("Load Value Mapping File Error"), tr("Load Value Mapping File Error: ") + e.what());
			}
		}
	}

	settings.endGroup();
}

void ValueMappingDialog::OnSaveToFile() {

	QSettings settings;
	settings.beginGroup("ValueMapping");
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Value Mappings"), settings.value("LastDir", QDir::currentPath()).toString(), "SynGlyphX Value Mapping Files (*.svm)");
	if (!filename.isEmpty()) {

		try {

			SaveFile(filename.toStdString());
			QFileInfo fileInfo(filename);
			settings.setValue("LastDir", fileInfo.absolutePath());
		}
		catch (const std::exception& e) {

			QMessageBox::warning(this, tr("Save Value Mapping File Error"), tr("Save Value Mapping File Error: ") + e.what());
		}
	}

	settings.endGroup();
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

	OnNumberOfRowsInTableChanged();
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

			throw std::runtime_error("At least one key is the same as another key.  All keys must be unique.");
		}

		double output = GetDoubleFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Numeric2NumericMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Numeric2NumericMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Numeric2NumericMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Numeric2NumericMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
}

////////////////////////////////////////////////////////////////////////////////

Numeric2ColorMappingDialog::Numeric2ColorMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Numeric, OutputType::Color, parent) {

}

Numeric2ColorMappingDialog::~Numeric2ColorMappingDialog() {

}

void Numeric2ColorMappingDialog::SetDialogFromMapping(SynGlyphX::Numeric2ColorMappingData::ConstSharedPtr mapping) {

	m_defaultColorWidget->SetColor(SynGlyphX::ColorConverter::GlyphColor2QColor(mapping->GetDefaultValue()));

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		QDoubleSpinBox* inputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 0));
		inputTableWidget->setValue(keyValuePair.first);

		SynGlyphX::ColorButton* outputTableWidget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetColor(SynGlyphX::ColorConverter::GlyphColor2QColor(keyValuePair.second));
	}

	OnNumberOfRowsInTableChanged();
}

SynGlyphX::Numeric2ColorMappingData::SharedPtr Numeric2ColorMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Numeric2ColorMappingDialog::CreateMappingData() {

	SynGlyphX::Numeric2ColorMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Numeric2ColorMappingData>();

	mapping->SetDefaultValue(SynGlyphX::ColorConverter::QColor2GlyphColor(m_defaultColorWidget->GetColor()));

	for (int row = 0; row < m_table->rowCount(); ++row) {

		double input = GetDoubleFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::runtime_error("At least one key is the same as another key.  All keys must be unique.");
		}

		SynGlyphX::GlyphColor output = GetColorFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Numeric2ColorMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Numeric2ColorMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Numeric2ColorMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Numeric2ColorMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
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

	OnNumberOfRowsInTableChanged();
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

			throw std::runtime_error("At least one key is the same as another key.  All keys must be unique.");
		}

		double output = GetDoubleFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Text2NumericMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Text2NumericMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Text2NumericMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Text2NumericMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
}

////////////////////////////////////////////////////////////////////////////////

Text2ColorMappingDialog::Text2ColorMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Text, OutputType::Color, parent) {

}

Text2ColorMappingDialog::~Text2ColorMappingDialog() {


}

void Text2ColorMappingDialog::SetDialogFromMapping(SynGlyphX::Text2ColorMappingData::ConstSharedPtr mapping) {

	m_defaultColorWidget->SetColor(SynGlyphX::ColorConverter::GlyphColor2QColor(mapping->GetDefaultValue()));

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		QLineEdit* inputTableWidget = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 0));
		inputTableWidget->setText(QString::fromStdWString(keyValuePair.first));

		SynGlyphX::ColorButton* outputTableWidget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetColor(SynGlyphX::ColorConverter::GlyphColor2QColor(keyValuePair.second));
	}

	OnNumberOfRowsInTableChanged();
}

SynGlyphX::Text2ColorMappingData::SharedPtr Text2ColorMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Text2ColorMappingDialog::CreateMappingData() {

	SynGlyphX::Text2ColorMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Text2ColorMappingData>();

	mapping->SetDefaultValue(SynGlyphX::ColorConverter::QColor2GlyphColor(m_defaultColorWidget->GetColor()));

	for (int row = 0; row < m_table->rowCount(); ++row) {

		std::wstring input = GetTextFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::runtime_error("At least one key is the same as another key.  All keys must be unique.");
		}

		SynGlyphX::GlyphColor output = GetColorFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Text2ColorMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Text2ColorMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Text2ColorMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Text2ColorMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
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

	OnNumberOfRowsInTableChanged();
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

			throw std::runtime_error("At least one range overlaps another range.  All ranges must be distinct from each other.");
		}

		double output = GetDoubleFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Range2NumericMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Range2NumericMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Range2NumericMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Range2NumericMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
}

////////////////////////////////////////////////////////////////////////////////

Range2ColorMappingDialog::Range2ColorMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Range, OutputType::Color, parent) {

}

Range2ColorMappingDialog::~Range2ColorMappingDialog() {


}

void Range2ColorMappingDialog::SetDialogFromMapping(SynGlyphX::Range2ColorMappingData::ConstSharedPtr mapping) {

	m_defaultColorWidget->SetColor(SynGlyphX::ColorConverter::GlyphColor2QColor(mapping->GetDefaultValue()));

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		SynGlyphX::RangeWidget* inputTableWidget = dynamic_cast<SynGlyphX::RangeWidget*>(m_table->cellWidget(row, 0));
		inputTableWidget->SetRange(keyValuePair.first);

		SynGlyphX::ColorButton* outputTableWidget = dynamic_cast<SynGlyphX::ColorButton*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetColor(SynGlyphX::ColorConverter::GlyphColor2QColor(keyValuePair.second));
	}

	OnNumberOfRowsInTableChanged();
}

SynGlyphX::Range2ColorMappingData::SharedPtr Range2ColorMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Range2ColorMappingDialog::CreateMappingData() {

	SynGlyphX::Range2ColorMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Range2ColorMappingData>();

	mapping->SetDefaultValue(SynGlyphX::ColorConverter::QColor2GlyphColor(m_defaultColorWidget->GetColor()));

	for (int row = 0; row < m_table->rowCount(); ++row) {

		SynGlyphX::Range input = GetRangeFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::runtime_error("At least one range overlaps another range.  All ranges must be distinct from each other.");
		}

		SynGlyphX::GlyphColor output = GetColorFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Range2ColorMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Range2ColorMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Range2ColorMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Range2ColorMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
}

////////////////////////////////////////////////////////////////////////////////

Numeric2ShapeMappingDialog::Numeric2ShapeMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Numeric, OutputType::GeometryShape, parent) {

}

Numeric2ShapeMappingDialog::~Numeric2ShapeMappingDialog() {

}

void Numeric2ShapeMappingDialog::SetDialogFromMapping(SynGlyphX::Numeric2ShapeMappingData::ConstSharedPtr mapping) {

	m_defaultShapeWidget->SetCurrentValue(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		QDoubleSpinBox* inputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 0));
		inputTableWidget->setValue(keyValuePair.first);

		SynGlyphX::GlyphShapeComboBox* outputTableWidget = dynamic_cast<SynGlyphX::GlyphShapeComboBox*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetCurrentValue(keyValuePair.second);
	}

	OnNumberOfRowsInTableChanged();
}

SynGlyphX::Numeric2ShapeMappingData::SharedPtr Numeric2ShapeMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Numeric2ShapeMappingDialog::CreateMappingData() {

	SynGlyphX::Numeric2ShapeMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Numeric2ShapeMappingData>();

	mapping->SetDefaultValue(m_defaultShapeWidget->GetCurrentValue());

	for (int row = 0; row < m_table->rowCount(); ++row) {

		double input = GetDoubleFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::runtime_error("At least one key is the same as another key.  All keys must be unique.");
		}

		SynGlyphX::GlyphGeometryInfo::Shape output = GetShapeFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Numeric2ShapeMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Numeric2ShapeMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Numeric2ShapeMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Numeric2ShapeMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
}

////////////////////////////////////////////////////////////////////////////////

Numeric2VirtualTopologyMappingDialog::Numeric2VirtualTopologyMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Numeric, OutputType::VirtualTopology, parent) {

}

Numeric2VirtualTopologyMappingDialog::~Numeric2VirtualTopologyMappingDialog() {

}

void Numeric2VirtualTopologyMappingDialog::SetDialogFromMapping(SynGlyphX::Numeric2VirtualTopologyMappingData::ConstSharedPtr mapping) {

	m_defaultVirtualTopologyWidget->SetCurrentValue(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		QDoubleSpinBox* inputTableWidget = dynamic_cast<QDoubleSpinBox*>(m_table->cellWidget(row, 0));
		inputTableWidget->setValue(keyValuePair.first);

		SynGlyphX::VirtualTopologyComboBox* outputTableWidget = dynamic_cast<SynGlyphX::VirtualTopologyComboBox*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetCurrentValue(keyValuePair.second);
	}

	OnNumberOfRowsInTableChanged();
}

SynGlyphX::Numeric2VirtualTopologyMappingData::SharedPtr Numeric2VirtualTopologyMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Numeric2VirtualTopologyMappingDialog::CreateMappingData() {

	SynGlyphX::Numeric2VirtualTopologyMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Numeric2VirtualTopologyMappingData>();

	mapping->SetDefaultValue(m_defaultVirtualTopologyWidget->GetCurrentValue());

	for (int row = 0; row < m_table->rowCount(); ++row) {

		double input = GetDoubleFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::runtime_error("At least one key is the same as another key.  All keys must be unique.");
		}

		SynGlyphX::VirtualTopologyInfo::Type output = GetVirtualTopologyFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Numeric2VirtualTopologyMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Numeric2VirtualTopologyMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Numeric2VirtualTopologyMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Numeric2VirtualTopologyMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
}

////////////////////////////////////////////////////////////////////////////////

Text2ShapeMappingDialog::Text2ShapeMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Text, OutputType::GeometryShape, parent) {

}

Text2ShapeMappingDialog::~Text2ShapeMappingDialog() {


}

void Text2ShapeMappingDialog::SetDialogFromMapping(SynGlyphX::Text2ShapeMappingData::ConstSharedPtr mapping) {

	m_defaultShapeWidget->SetCurrentValue(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		QLineEdit* inputTableWidget = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 0));
		inputTableWidget->setText(QString::fromStdWString(keyValuePair.first));

		SynGlyphX::GlyphShapeComboBox* outputTableWidget = dynamic_cast<SynGlyphX::GlyphShapeComboBox*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetCurrentValue(keyValuePair.second);
	}

	OnNumberOfRowsInTableChanged();
}

SynGlyphX::Text2ShapeMappingData::SharedPtr Text2ShapeMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Text2ShapeMappingDialog::CreateMappingData() {

	SynGlyphX::Text2ShapeMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Text2ShapeMappingData>();

	mapping->SetDefaultValue(m_defaultShapeWidget->GetCurrentValue());

	for (int row = 0; row < m_table->rowCount(); ++row) {

		std::wstring input = GetTextFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::runtime_error("At least one key is the same as another key.  All keys must be unique.");
		}

		SynGlyphX::GlyphGeometryInfo::Shape output = GetShapeFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Text2ShapeMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Text2ShapeMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Text2ShapeMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Text2ShapeMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
}

////////////////////////////////////////////////////////////////////////////////

Text2VirtualTopologyMappingDialog::Text2VirtualTopologyMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Text, OutputType::VirtualTopology, parent) {

}

Text2VirtualTopologyMappingDialog::~Text2VirtualTopologyMappingDialog() {


}

void Text2VirtualTopologyMappingDialog::SetDialogFromMapping(SynGlyphX::Text2VirtualTopologyMappingData::ConstSharedPtr mapping) {

	m_defaultVirtualTopologyWidget->SetCurrentValue(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		QLineEdit* inputTableWidget = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 0));
		inputTableWidget->setText(QString::fromStdWString(keyValuePair.first));

		SynGlyphX::VirtualTopologyComboBox* outputTableWidget = dynamic_cast<SynGlyphX::VirtualTopologyComboBox*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetCurrentValue(keyValuePair.second);
	}

	OnNumberOfRowsInTableChanged();
}

SynGlyphX::Text2VirtualTopologyMappingData::SharedPtr Text2VirtualTopologyMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Text2VirtualTopologyMappingDialog::CreateMappingData() {

	SynGlyphX::Text2VirtualTopologyMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Text2VirtualTopologyMappingData>();

	mapping->SetDefaultValue(m_defaultVirtualTopologyWidget->GetCurrentValue());

	for (int row = 0; row < m_table->rowCount(); ++row) {

		std::wstring input = GetTextFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::runtime_error("At least one key is the same as another key.  All keys must be unique.");
		}

		SynGlyphX::VirtualTopologyInfo::Type output = GetVirtualTopologyFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Text2VirtualTopologyMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Text2VirtualTopologyMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Text2VirtualTopologyMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Text2VirtualTopologyMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
}

////////////////////////////////////////////////////////////////////////////////

Range2ShapeMappingDialog::Range2ShapeMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Range, OutputType::GeometryShape, parent) {

}

Range2ShapeMappingDialog::~Range2ShapeMappingDialog() {


}

void Range2ShapeMappingDialog::SetDialogFromMapping(SynGlyphX::Range2ShapeMappingData::ConstSharedPtr mapping) {

	m_defaultShapeWidget->SetCurrentValue(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		SynGlyphX::RangeWidget* inputTableWidget = dynamic_cast<SynGlyphX::RangeWidget*>(m_table->cellWidget(row, 0));
		inputTableWidget->SetRange(keyValuePair.first);

		SynGlyphX::GlyphShapeComboBox* outputTableWidget = dynamic_cast<SynGlyphX::GlyphShapeComboBox*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetCurrentValue(keyValuePair.second);
	}

	OnNumberOfRowsInTableChanged();
}

SynGlyphX::Range2ShapeMappingData::SharedPtr Range2ShapeMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Range2ShapeMappingDialog::CreateMappingData() {

	SynGlyphX::Range2ShapeMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Range2ShapeMappingData>();

	mapping->SetDefaultValue(m_defaultShapeWidget->GetCurrentValue());

	for (int row = 0; row < m_table->rowCount(); ++row) {

		SynGlyphX::Range input = GetRangeFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::runtime_error("At least one key is the same as another key.  All keys must be unique.");
		}

		SynGlyphX::GlyphGeometryInfo::Shape output = GetShapeFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Range2ShapeMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Range2ShapeMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Range2ShapeMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Range2ShapeMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
}

////////////////////////////////////////////////////////////////////////////////

Range2VirtualTopologyMappingDialog::Range2VirtualTopologyMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Range, OutputType::VirtualTopology, parent) {

}

Range2VirtualTopologyMappingDialog::~Range2VirtualTopologyMappingDialog() {


}

void Range2VirtualTopologyMappingDialog::SetDialogFromMapping(SynGlyphX::Range2VirtualTopologyMappingData::ConstSharedPtr mapping) {

	m_defaultVirtualTopologyWidget->SetCurrentValue(mapping->GetDefaultValue());

	for (auto keyValuePair : mapping->GetKeyValueMap()) {

		AddRow();
		int row = m_table->rowCount() - 1;

		SynGlyphX::RangeWidget* inputTableWidget = dynamic_cast<SynGlyphX::RangeWidget*>(m_table->cellWidget(row, 0));
		inputTableWidget->SetRange(keyValuePair.first);

		SynGlyphX::VirtualTopologyComboBox* outputTableWidget = dynamic_cast<SynGlyphX::VirtualTopologyComboBox*>(m_table->cellWidget(row, 1));
		outputTableWidget->SetCurrentValue(keyValuePair.second);
	}

	OnNumberOfRowsInTableChanged();
}

SynGlyphX::Range2VirtualTopologyMappingData::SharedPtr Range2VirtualTopologyMappingDialog::GetMappingFromDialog() const {

	return m_mappingData;
}

bool Range2VirtualTopologyMappingDialog::CreateMappingData() {

	SynGlyphX::Range2VirtualTopologyMappingData::SharedPtr mapping = std::make_shared<SynGlyphX::Range2VirtualTopologyMappingData>();

	mapping->SetDefaultValue(m_defaultVirtualTopologyWidget->GetCurrentValue());

	for (int row = 0; row < m_table->rowCount(); ++row) {

		SynGlyphX::Range input = GetRangeFromWidget(row, 0);
		if (mapping->IsKeyInKeyValueMap(input)) {

			throw std::runtime_error("At least one key is the same as another key.  All keys must be unique.");
		}

		SynGlyphX::VirtualTopologyInfo::Type output = GetVirtualTopologyFromWidget(row, 1);
		mapping->SetValueForKey(input, output);
	}

	m_mappingData = mapping;
	return true;
}

void Range2VirtualTopologyMappingDialog::LoadFile(const std::string& filename) {

	SynGlyphX::Range2VirtualTopologyMappingFile mappingFile;
	mappingFile.ReadFromFile(filename);
	SetDialogFromMapping(mappingFile.GetValueMapping());
}

void Range2VirtualTopologyMappingDialog::SaveFile(const std::string& filename) {

	CreateMappingData();
	SynGlyphX::Range2VirtualTopologyMappingFile mappingFile(GetMappingFromDialog());
	mappingFile.WriteToFile(filename);
}