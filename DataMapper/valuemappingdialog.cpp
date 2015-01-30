#include "valuemappingdialog.h"
#include "glyphcolor.h"
#include "range.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGroupBox>

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
	defaultLayout->addWidget(new QLabel(tr("Default")));
	if (m_output == OutputType::Numeric) {

		m_defaultDoubleWidget = new QDoubleSpinBox(this);
		m_defaultDoubleWidget->setDecimals(3);
		defaultLayout->addWidget(m_defaultDoubleWidget);
	}
	else {

		m_defaultColorWidget = new SynGlyphX::ColorButton(false, this);
		defaultLayout->addWidget(m_defaultDoubleWidget);
	}
	layout->addLayout(defaultLayout);

	m_table = new QTableWidget(this);
	m_table->setColumnCount(2);

	QStringList headers;
	headers << "Key";
	headers << "Value";

	m_table->setHorizontalHeaderLabels(headers);
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->sortByColumn(0, Qt::SortOrder::AscendingOrder);
	m_table->setSortingEnabled(true);
	QObject::connect(m_table, &QTableWidget::itemSelectionChanged, this, &ValueMappingDialog::OnTableSelectionChanged);
	layout->addWidget(m_table);

	QGroupBox* inputGroupBox = new QGroupBox(tr("New Key Value Pair"), this);
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

	inputGroupBox->setLayout(inputLayout);
	layout->addWidget(inputGroupBox);

	QHBoxLayout* addRemoveLayout = new QHBoxLayout(this);

	QPushButton* addEntryButton = new QPushButton(tr("Add Key Value Pair"), this);
	QObject::connect(addEntryButton, &QPushButton::clicked, this, &ValueMappingDialog::OnAddKeyValue);
	addRemoveLayout->addWidget(addEntryButton);

	m_removeEntryButton = new QPushButton(tr("Remove Key Value Pair"), this);
	QObject::connect(m_removeEntryButton, &QPushButton::clicked, this, &ValueMappingDialog::OnRemoveKeyValue);
	addRemoveLayout->addWidget(m_removeEntryButton);

	layout->addLayout(addRemoveLayout);

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

	if (m_input == InputType::Text) {

		if (m_inputTextWidget->text().isEmpty()) {

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

	m_table->removeRow(m_table->currentRow());
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

	m_table->setCellWidget(row, 0, inputWidget);

	QWidget* outputWidget = nullptr;
	if (m_output == OutputType::Numeric) {

		outputWidget = new QDoubleSpinBox(this);
	}
	else {

		outputWidget = new SynGlyphX::ColorButton(this);
	}

	m_table->setCellWidget(row, 1, outputWidget);
}

////////////////////////////////////////////////////////////////////////////////

Numeric2NumericMappingDialog::Numeric2NumericMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Numeric, OutputType::Numeric, parent) {

}

Numeric2NumericMappingDialog::~Numeric2NumericMappingDialog() {


}

////////////////////////////////////////////////////////////////////////////////

Numeric2ColorMappingDialog::Numeric2ColorMappingDialog(QWidget *parent) :
	ValueMappingDialog(InputType::Numeric, OutputType::Color, parent) {

}

Numeric2ColorMappingDialog::~Numeric2ColorMappingDialog() {


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