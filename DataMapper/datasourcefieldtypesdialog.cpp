#include "datasourcefieldtypesdialog.h"
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollBar>
#include "databaseinfo.h"

DatasourceFieldTypesDialog::DatasourceFieldTypesDialog(QStringList fields, const QString& fileFormat, QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Select Field Types"));

	QVBoxLayout* layout = new QVBoxLayout(this);

	m_fieldTypeTable = new QTableWidget(fields.length(), 2, this);
	m_fieldTypeTable->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	m_fieldTypeTable->setCornerButtonEnabled(false);
	QStringList labels;
	labels.push_back(tr("Field:"));
	labels.push_back(tr("Type:"));
	m_fieldTypeTable->setHorizontalHeaderLabels(labels);

	QStringList fieldTypes = SynGlyphX::DatabaseInfo::GetSQLiteDataTypesForFormat(fileFormat);

	for (int i = 0; i < fields.length(); ++i) {

		m_fieldTypeTable->setCellWidget(i, 0, new QLabel(fields[i]));
			
		QWidget* radioButtonBox = new QWidget(this);
		QHBoxLayout* radioButtonBoxLayout = new QHBoxLayout(radioButtonBox);
		QButtonGroup* buttonGroup = new QButtonGroup(this);
			
		for (int j = 0; j < fieldTypes.size(); ++j) {

			QRadioButton* radioButton = new QRadioButton(fieldTypes[j], radioButtonBox);
			buttonGroup->addButton(radioButton);
			radioButtonBoxLayout->addWidget(radioButton);

			if (j == 0) {

				radioButton->setChecked(true);
			}
		}

		m_radioButtonGroups.push_back(buttonGroup);
		radioButtonBox->setLayout(radioButtonBoxLayout);
		m_fieldTypeTable->setCellWidget(i, 1, radioButtonBox);
	}

	m_fieldTypeTable->resizeColumnsToContents();
	m_fieldTypeTable->resizeRowsToContents();

	int minTableWidth = m_fieldTypeTable->model()->columnCount() + m_fieldTypeTable->verticalHeader()->width() + m_fieldTypeTable->verticalScrollBar()->sizeHint().width();
	for (int column = 0; column < m_fieldTypeTable->model()->columnCount(); column++) {

		minTableWidth += m_fieldTypeTable->columnWidth(column);

	}
	m_fieldTypeTable->setMinimumWidth(minTableWidth);
	int minTableHeight = (m_fieldTypeTable->model()->rowCount() * (1 + m_fieldTypeTable->rowHeight(0))) + m_fieldTypeTable->horizontalHeader()->height();
	if (minTableHeight < 0.75 * parent->height()) {

		m_fieldTypeTable->setMinimumHeight(minTableHeight);
	}
	else {

		m_fieldTypeTable->setMinimumHeight(0.75 * parent->height());
	}
		

	layout->addWidget(m_fieldTypeTable);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &DatasourceFieldTypesDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &DatasourceFieldTypesDialog::reject);
	layout->addWidget(dialogButtonBox);

	setLayout(layout);
}

DatasourceFieldTypesDialog::~DatasourceFieldTypesDialog()
{

}

QStringList DatasourceFieldTypesDialog::GetFieldTypes() const {

	QStringList types;
	Q_FOREACH(QButtonGroup* buttonGroup, m_radioButtonGroups) {

		types.push_back(buttonGroup->checkedButton()->text());
	}
		
	return types;
}
