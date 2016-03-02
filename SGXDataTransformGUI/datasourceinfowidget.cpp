#include "datasourceinfowidget.h"
#include <QtWidgets/QVBoxLayout>
#include "groupboxsinglewidget.h"
#include <QtWidgets/QListWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtCore/QDir>
#include "databaseserverinfowidget.h"

namespace SynGlyphX {

	DatasourceInfoWidget::DatasourceInfoWidget(const Datasource& datasource, QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		setLayout(mainLayout);

		QHBoxLayout* nameLayout = new QHBoxLayout(this);

		QLabel* nameLabel = new QLabel(tr("Name") + ":", this);
		nameLayout->addWidget(nameLabel);
		QLineEdit* nameLineEdit = new QLineEdit(QString::fromStdWString(datasource.GetFormattedName()), this);
		nameLineEdit->setMinimumWidth(384);
		nameLineEdit->setReadOnly(true);
		nameLayout->addWidget(nameLineEdit);

		mainLayout->addLayout(nameLayout);

		if (datasource.GetSourceType() == Datasource::SourceType::File) {

			CreateFileDatasourceWidgets(dynamic_cast<const FileDatasource&>(datasource));
		}
		else if (datasource.GetSourceType() == Datasource::SourceType::DatabaseServer) {

			CreateDatabaseServerDatasourceWidgets(dynamic_cast<const DatabaseServerDatasource&>(datasource));
		}

		if (datasource.CanDatasourceHaveMultipleTables()) {

			Datasource::Tables::const_iterator tablesQueryIterator = datasource.GetTables().begin();

			if (tablesQueryIterator->second.GetQuery().empty()) {

				QListWidget* tableListWidget = new QListWidget(this);
				tableListWidget->setSelectionMode(QAbstractItemView::NoSelection);
				for (const std::wstring& table : datasource.GetTableNames()) {

					tableListWidget->addItem(QString::fromStdWString(table));
				}
				SynGlyphX::GroupBoxSingleWidget* tableGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Tables"), tableListWidget, this);

				mainLayout->addWidget(tableGroupBox);
			}
			else {

				QFormLayout* tableQueryLayout = new QFormLayout(this);
				tableQueryLayout->setContentsMargins(0, 0, 0, 0);

				QLineEdit* tableLineEdit = new QLineEdit(QString::fromStdWString(tablesQueryIterator->first), this);
				tableLineEdit->setReadOnly(true);
				tableQueryLayout->addRow(tr("Table:"), tableLineEdit);

				QLineEdit* queryLineEdit = new QLineEdit(QString::fromStdWString(tablesQueryIterator->second.GetQuery()), this);
				queryLineEdit->setReadOnly(true);
				tableQueryLayout->addRow(tr("Query:"), queryLineEdit);

				mainLayout->addLayout(tableQueryLayout);
			}
		}
	}

	DatasourceInfoWidget::~DatasourceInfoWidget()
	{

	}

	void DatasourceInfoWidget::CreateFileDatasourceWidgets(const FileDatasource& datasource) {

		QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());

		QGroupBox* fileInfoGroupBox = new QGroupBox(tr("File"), this);
		QVBoxLayout* fileInfoGroupBoxLayout = new QVBoxLayout(fileInfoGroupBox);

		QLineEdit* filenameLineEdit = new QLineEdit(QDir::toNativeSeparators(QString::fromStdWString(datasource.GetFilename())), fileInfoGroupBox);
		filenameLineEdit->setMinimumWidth(384);
		filenameLineEdit->setReadOnly(true);
		fileInfoGroupBoxLayout->addWidget(filenameLineEdit);

		QLabel* typeLabel = new QLabel(tr("Type") + ": " + QString::fromStdWString(FileDatasource::s_fileTypeStrings.left.at(datasource.GetFileType())), fileInfoGroupBox);
		fileInfoGroupBoxLayout->addWidget(typeLabel);

		fileInfoGroupBox->setLayout(fileInfoGroupBoxLayout);

		mainLayout->addWidget(fileInfoGroupBox);
	}

	void DatasourceInfoWidget::CreateDatabaseServerDatasourceWidgets(const DatabaseServerDatasource& datasource) {

		QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());

		SynGlyphX::DatabaseServerInfoWidget* databaseServerInfoWidget = new SynGlyphX::DatabaseServerInfoWidget(false, this);
		databaseServerInfoWidget->SetConnection(QString::fromStdWString(datasource.GetHost()));
		databaseServerInfoWidget->SetUsername(QString::fromStdWString(datasource.GetUsername()));
		databaseServerInfoWidget->SetPassword(QString::fromStdWString(datasource.GetPassword()));

		mainLayout->addWidget(databaseServerInfoWidget);
	}

} //namespace SynGlyphX