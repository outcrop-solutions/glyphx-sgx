#include "FieldPropertiesDialog.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QDateTime>
#include "singledatetimerangefilterwidget.h"

FieldPropertiesDialog::FieldPropertiesDialog(SynGlyphX::FieldProperties properties, QWidget* parent)
	: QDialog(parent),
	m_properties(properties)
{

	QVBoxLayout* dlgLayout = new QVBoxLayout(this);
	QHBoxLayout* mainLayout = new QHBoxLayout(this);

	QGroupBox* l_groupBox = new QGroupBox(tr("Types:"));
	QVBoxLayout* l_layout = new QVBoxLayout();
	types = new QListWidget();
	types->setStyleSheet("QListWidget::item:selected {background-color: #4acaf8;color: white;}");

	l_layout->addWidget(types);
	l_groupBox->setLayout(l_layout);
	mainLayout->addWidget(l_groupBox);

	stackedWidget = new QStackedWidget();
	mainLayout->addWidget(stackedWidget);

	SetDateTimeFormats();
	SetTypesAndDetails();

	dlgLayout->addLayout(mainLayout);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

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

		typeNames << "Number" << "Currency" << "Percentage" << "Datetime";
		details["Number"] = "The number format is for general display of numbers.";
		details["Currency"] = "The currency format is used for general monetary values.";
		details["Percentage"] = "The percentage format multiplies the fields value by 100 and displays it with a percent symbol.";
		details["Datetime"] = "The datetime format is used to display a timestamp as a date and/or time value.";
	}

	for (const auto& name : typeNames){
		types->addItem(name);
		stackedWidget->addWidget(SetGroupBoxForSelection(name));
	}

	types->setFixedWidth(fontMetrics().width("Percentage") * 2);

	types->setCurrentItem(types->item(m_properties.GetType()));
	stackedWidget->setCurrentIndex(m_properties.GetType());
}

void FieldPropertiesDialog::SetDateTimeFormats()
{
	dateFormats << "N/A" << "dddd, MMMM dd, yyyy" << "MM/dd" << "dd-MMM" <<
		"MM/dd/yy" << "dd-MMM-yy" << "dd-MMM-yyyy" << "MMMM dd, yyyy";

	timeFormats << "N/A" << "h:mm AP" << "hh:mm" << "hh:mm:ss" << "hh:mm:ss.zzz";
}

QGroupBox* FieldPropertiesDialog::SetGroupBoxForSelection(QString type)
{
	QGroupBox* r_groupBox = new QGroupBox(tr("Options:"));
	QFormLayout* formLayout = new QFormLayout(this);

	if (type != "Default" && type != "Datetime"){

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
			symbolCB->addItem(QString::fromStdWString(L"\u00A3"));
			symbolCB->addItem(QString::fromStdWString(L"\u20AC"));
			symbolCB->addItem(QString::fromStdWString(L"\u00A5"));
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
	else if (type == "Datetime"){

		qint64 ts = m_properties.GetSampleValue().toLongLong();
		QDateTime* dt = new QDateTime();
		if (QDateTime::currentMSecsSinceEpoch() / 1000.0 < ts){
			dt->setMSecsSinceEpoch(ts);
		}
		else{
			dt->setMSecsSinceEpoch(ts * 1000.0);
		}

		QHBoxLayout* hbLayout = new QHBoxLayout();

		QListWidget* formatList = new QListWidget();
		formatList->setObjectName("DateFormatList");
		formatList->setStyleSheet("QListWidget::item:selected {background-color: #4acaf8;color: white;}");
		formatList->setSelectionMode(QAbstractItemView::SingleSelection);
		hbLayout->addWidget(formatList);

		formatList->addItem(dateFormats.at(0));
		for (int i = 1; i < dateFormats.size(); i++){
			formatList->addItem(dt->toString(dateFormats.at(i)));
		}
		QString datetest = m_properties.DateFormat();
		int dateindex = dateFormats.indexOf(m_properties.DateFormat());
		formatList->setCurrentItem(formatList->item(dateFormats.indexOf(m_properties.DateFormat())));

		QListWidget* timeFormatList = new QListWidget();
		timeFormatList->setObjectName("TimeFormatList");
		timeFormatList->setStyleSheet("QListWidget::item:selected {background-color: #4acaf8;color: white;}");
		timeFormatList->setSelectionMode(QAbstractItemView::SingleSelection);
		hbLayout->addWidget(timeFormatList);

		timeFormatList->addItem(timeFormats.at(0));
		for (int i = 1; i < timeFormats.size(); i++){
			timeFormatList->addItem(dt->toString(timeFormats.at(i)));
		}
		timeFormatList->setCurrentItem(timeFormatList->item(timeFormats.indexOf(m_properties.TimeFormat())));;

		formLayout->addRow(hbLayout);

		QFontMetrics fm = fontMetrics();
		int w = fm.width(dt->toString("dddd, MMMM dd, yyyy")) * 1.3;

		formatList->setFixedWidth(w);
		timeFormatList->setFixedWidth(w);

	}
	/*
	if (type == "Default"){
		QLabel* date = new QLabel(tr("Datetime: "), this);
		SynGlyphX::SingleDatetimeRangeFilterWidget* dateEdit = new SynGlyphX::SingleDatetimeRangeFilterWidget(Qt::Horizontal, this);
		dateEdit->SetRange(SynGlyphX::DegenerateInterval(m_properties.GetMin().toDouble(), m_properties.GetMax().toDouble()));
		dateEdit->SetFieldProperties(m_properties);
		formLayout->addRow(date);
		formLayout->addRow(dateEdit);
	}
	*/
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

	int dec = typeName == "Default" || typeName == "Datetime" ? 0 : decimals->value();
	std::wstring sym = typeName != "Currency" ? L"" : symbol->currentText().toStdWString();
	QString dFmt = "";
	QString tFmt = "";

	if (typeName == "Datetime"){

		int dateRow = currentGroupBox->findChild<QListWidget*>("DateFormatList")->currentRow();
		int timeRow = currentGroupBox->findChild<QListWidget*>("TimeFormatList")->currentRow();
		dFmt += dateRow == 0 ? "" : dateFormats.at(dateRow);
		tFmt += timeRow == 0 ? "" : timeFormats.at(timeRow);
	}

	m_properties.UpdateProperties(typeName.toStdWString(), dec, sym, dFmt, tFmt);

	model->setFormattedData(model->index(row, 2), m_properties.transformData(2));
	model->setFormattedData(model->index(row, 3), m_properties.transformData(3));

	return m_properties;
}