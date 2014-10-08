#include "datasourcefieldtypesdialog.h"
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include "sourcedatamanager.h"

namespace SynGlyphX {

	DatasourceFieldTypesDialog::DatasourceFieldTypesDialog(QStringList fields, const QString& fileFormat, QWidget *parent)
		: QDialog(parent)
	{
		setWindowTitle(tr("Select Field Types"));

		QVBoxLayout* layout = new QVBoxLayout(this);

		m_fieldTypeTable = new QTableWidget(fields.length(), 2, this);
		m_fieldTypeTable->setCornerButtonEnabled(false);
		QStringList labels;
		labels.push_back(tr("Field:"));
		labels.push_back(tr("Type:"));
		m_fieldTypeTable->setHorizontalHeaderLabels(labels);

		QStringList fieldTypes = SourceDataManager::GetSQLiteDataTypesForFormat(fileFormat);

		for (int i = 0; i < fields.length(); ++i) {

			m_fieldTypeTable->setCellWidget(i, 0, new QLabel(fields[i]));
			
			QComboBox* typeComboBox = new QComboBox(this);
			typeComboBox->addItems(fieldTypes);
			typeComboBox->setCurrentIndex(0);
			m_fieldTypeTable->setCellWidget(i, 1, typeComboBox);
		}

		m_fieldTypeTable->resizeColumnsToContents();
		m_fieldTypeTable->resizeRowsToContents();

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
		for (int i = 0; i < m_fieldTypeTable->rowCount(); ++i) {

			QComboBox* comboBox = dynamic_cast<QComboBox*>(m_fieldTypeTable->cellWidget(i, 1));
			types.push_back(comboBox->currentText());
		}
		
		return types;
	}

} //namespace SynGlyphX