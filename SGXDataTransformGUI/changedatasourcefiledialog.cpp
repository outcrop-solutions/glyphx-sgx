#include "changedatasourcefiledialog.h"
#include <QtCore/QFile>
#include <QtCore/QDir>

namespace SynGlyphX {

	ChangeDatasourceFileDialog::ChangeDatasourceFileDialog(const FileDatasource& oldDatasourceFile, const QString& acceptButtonText, QWidget *parent)
		: ReplaceFilenameDialog(QString::fromStdWString(oldDatasourceFile.GetDBName()), acceptButtonText, parent),
		m_fileSourceType(oldDatasourceFile.GetFileType())
	{
		setWindowTitle(tr("Change Datasource File"));

		if (oldDatasourceFile.CanDatasourceHaveMultipleTables()) {

			for (std::wstring table : oldDatasourceFile.GetTableNames()) {

				m_oldDatasourceTables.append(QString::fromStdWString(table));
			}
			m_oldDatasourceTables.sort();
		}
	}

	ChangeDatasourceFileDialog::~ChangeDatasourceFileDialog()
	{

	}

	bool ChangeDatasourceFileDialog::IsNewFileValid() const {

		QString newDatasource = m_newDatasourceFileLineEdit->GetText();

		try {

			if (m_fileSourceType == FileDatasource::GetFileTypeForFile(newDatasource.toStdWString())) {

				QFile newFile(newDatasource);
				if (newFile.exists()) {

					if (m_fileSourceType == FileDatasource::FileType::SQLITE3) {

						if (DatabaseInfo::IsSQLiteDB(newDatasource)) {

							QStringList tables = DatabaseInfo::GetListOfTablesWithoutAutogeneratedTablesInDatabaseFile(newDatasource);
							tables.sort();
							return (m_oldDatasourceTables == tables);
						}
					}
					else {

						return true;
					}
				}
			}
		}
		catch (const std::exception& e) {


		}

		return false;
	}

	bool ChangeDatasourceFileDialog::UpdateDatasourceFiles(const std::vector<boost::uuids::uuid>& datasources, DataTransformMapping::SharedPtr mapping, QWidget* dialogParent) {

		bool wereMissingFilesUpdated = false;
		for (int i = 0; i < datasources.size(); ++i) {

			QString acceptButtonText = tr("Next");
			if (i == datasources.size() - 1) {

				acceptButtonText = tr("Ok");
			}

			ChangeDatasourceFileDialog dialog(mapping->GetDatasources().GetFileDatasources().at(datasources[i]), acceptButtonText, dialogParent);
			if (dialog.exec() == QDialog::Accepted) {

				mapping->UpdateDatasourceName(datasources[i], dialog.GetNewFilename().toStdWString());
				wereMissingFilesUpdated = true;
			}
			else {

				throw std::runtime_error("One or more datasources weren't found.");
			}
		}

		return wereMissingFilesUpdated;
	}

} //namespace SynGlyphX