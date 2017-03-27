#include "FieldPropertiesDialog.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDialogButtonBox>

FieldPropertiesDialog::FieldPropertiesDialog(SynGlyphX::FieldProperties properties, QWidget* parent)
	: QDialog(parent),
	m_properties(properties)
{

	QVBoxLayout* dlgLayout = new QVBoxLayout(this);
	QHBoxLayout* mainLayout = new QHBoxLayout(this);

	QGroupBox* l_groupBox = new QGroupBox(tr("Types:"));
	QVBoxLayout* l_layout = new QVBoxLayout();
	types = new QListWidget();

	l_layout->addWidget(types);
	l_groupBox->setLayout(l_layout);
	mainLayout->addWidget(l_groupBox);

	stackedWidget = new QStackedWidget();
	mainLayout->addWidget(stackedWidget);

	SetTypesAndDetails();

	dlgLayout->addLayout(mainLayout);

	QSize size = sizeHint();
	l_groupBox->setFixedWidth(size.width() * 3 / 10);
	stackedWidget->setFixedWidth(size.width() * 7 / 10);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, this);
	dlgLayout->addWidget(buttonBox);

	setLayout(dlgLayout);

	QObject::connect(types, SIGNAL(currentRowChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));
	QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

FieldPropertiesDialog::~FieldPropertiesDialog()
{
}

void FieldPropertiesDialog::SetTypesAndDetails()
{
	QStringList typeNames;
	typeNames << "Default";
	details["Default"] = "The default format has no specific number format.";

	if (m_properties.GetDefaultFieldType() == "real"){

		typeNames << "Number" << "Currency" << "Percentage";
		details["Number"] = "The number format is for general display of numbers.";
		details["Currency"] = "The currency format is used for general monetary values.";
		details["Percentage"] = "The percentage format multiplies the fields value by 100 and displays it with a percent symbol.";
	}

	for (const auto& name : typeNames){
		types->addItem(name);
		stackedWidget->addWidget(SetGroupBoxForSelection(name));
	}

	types->setCurrentItem(types->item(m_properties.GetType()));
	stackedWidget->setCurrentIndex(m_properties.GetType());
}

QGroupBox* FieldPropertiesDialog::SetGroupBoxForSelection(QString type)
{
	QGroupBox* r_groupBox = new QGroupBox(tr("Options:"));
	QFormLayout* formLayout = new QFormLayout(this);

	if (type != "Default"){

		QLabel* decimals = new QLabel(tr("Decimals: "), this);
		QSpinBox* decimalSP = new QSpinBox(this);
		decimalSP->setMaximum(9);
		formLayout->addRow(decimals);
		formLayout->addRow(decimalSP);

		if (type == "Currency")
		{
			QLabel* symbol = new QLabel(tr("Symbol: "), this);
			QComboBox* symbolCB = new QComboBox(this);
			symbolCB->addItem(QString::fromWCharArray(L"$"));
			symbolCB->addItem(QString::fromWCharArray(L"£"));
			symbolCB->addItem(QString::fromWCharArray(L"€"));
			symbolCB->addItem(QString::fromWCharArray(L"¥"));
			formLayout->addRow(symbol);
			formLayout->addRow(symbolCB);
			decimalSP->setValue(2);
			QString sym = QString::fromStdWString(m_properties.GetSymbol());
			symbolCB->setCurrentIndex(symbolCB->findText(sym == "" ? "$" : sym));
		}

		if (m_properties.GetType() == SynGlyphX::FieldProperties::s_fieldTypeStrings.right.at(type.toStdWString()))
		{
			decimalSP->setValue(m_properties.GetDecimalsToDisplay());
		}
	}

	QLabel* description = new QLabel();
	description->setWordWrap(true);
	description->setText(details[type]);
	formLayout->addRow(description);

	r_groupBox->setLayout(formLayout);

	return r_groupBox;
}

SynGlyphX::FieldProperties FieldPropertiesDialog::SaveSelections(SynGlyphX::DataStatsModel* model, int row)
{
	QGroupBox* currentGroupBox = reinterpret_cast<QGroupBox*>(stackedWidget->currentWidget());

	QString typeName = types->item(stackedWidget->currentIndex())->text();

	QSpinBox* decimals = currentGroupBox->findChild<QSpinBox*>();
	QComboBox* symbol = currentGroupBox->findChild<QComboBox*>();

	int dec = typeName == "Default" ? 0 : decimals->value();
	std::wstring sym = typeName != "Currency" ? L"" : symbol->currentText().toStdWString();

	m_properties.UpdateProperties(typeName.toStdWString(), dec, sym);

	model->setData(model->index(row, 2), m_properties.transformData(2));
	model->setData(model->index(row, 3), m_properties.transformData(3));

	return m_properties;
}