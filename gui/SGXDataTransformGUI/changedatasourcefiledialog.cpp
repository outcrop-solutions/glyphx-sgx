#include "changedatasourcefiledialog.h"
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include "filesystem.h"
#include "browselineedit.h"
#include "datatransformmapping.h"
//#include "dataengineconnection.h"

namespace SynGlyphX {

	ChangeDatasourceFileDialog::ChangeDatasourceFileDialog(const FileDatasource& oldDatasourceFile, const QString& acceptButtonText, QWidget *parent)
		: ReplaceFilenameDialog(QString::fromStdWString(oldDatasourceFile.GetDBName()), acceptButtonText, parent),
        m_fileDatasource(oldDatasourceFile)
	{
		setWindowTitle(tr("Change Datasource File"));

		if (oldDatasourceFile.CanDatasourceHaveMultipleTables()) {

			Datasource::TableNames tables = oldDatasourceFile.GetTableNames();
			std::copy(tables.begin(), tables.end(), std::inserter(m_oldDatasourceTables, m_oldDatasourceTables.end()));
		}
	}

	ChangeDatasourceFileDialog::~ChangeDatasourceFileDialog()
	{

	}

	bool ChangeDatasourceFileDialog::IsNewFileValid() const {

		QString newDatasource = m_newDatasourceFileLineEdit->GetText();

		try {

			if (m_fileDatasource.GetFileType() == FileDatasource::GetFileTypeForFile(newDatasource.toStdWString())) {

				QFile newFile(newDatasource);
				if (newFile.exists()) {

					if (m_fileDatasource.GetFileType() == FileDatasource::FileType::SQLITE3) {

						/*m_dataEngineConnection->connectToServer(QString("sqlite:") + newDatasource, "", "", "sqlite3");
						QStringList newTables = m_dataEngineConnection->getTables();
						for (const std::wstring& oldTable : m_oldDatasourceTables) {

							if (!newTables.contains(QString::fromStdWString(oldTable))) {

								return false;
							}
						}*/
					}
					
					return true;
				}
			}
		}
		catch (const std::exception&) {


		}
		//m_dataEngineConnection->closeConnection();

		return false;
	}

	bool ChangeDatasourceFileDialog::UpdateDatasourceFiles(const std::vector<boost::uuids::uuid>& datasources, const QString& sdtfilename, DataTransformMapping::SharedPtr mapping, QWidget* dialogParent) {

		bool wereMissingFilesUpdated = false;
		QFileInfo fileInfo(sdtfilename);
		std::wstring stdDir = fileInfo.canonicalPath().toStdWString();

		for (unsigned int i = 0; i < datasources.size(); ++i) {
			auto dataSource = mapping->GetDatasources().at(datasources[i]);
			std::wstring name;
			if (dataSource->IsFile())
				name = static_cast<FileDatasource*>(dataSource.get())->GetFilename();
			else 
				name = mapping->GetDatasources().at(datasources[i])->GetFormattedName();

            std::replace(name.begin(), name.end(), L'\\', L'/');      // forward slash supported on all platforms
            
			std::wstring newpath = Filesystem::IsFileInDirectory(name, stdDir);
			if (newpath.empty()) {

				QString acceptButtonText = tr("Next");
				if (i == datasources.size() - 1) {

					acceptButtonText = tr("Ok");
				}

				ChangeDatasourceFileDialog dialog(*std::dynamic_pointer_cast<FileDatasource>(mapping->GetDatasources().at(datasources[i])), acceptButtonText, dialogParent);
				dialog.setWindowTitle(tr("Replace Missing Data Source"));
				if (dialog.exec() == QDialog::Accepted) {

					mapping->UpdateDatasourceName(datasources[i], dialog.GetNewFilename().toStdWString());
					wereMissingFilesUpdated = true;
				}
				else {

					return false;
				}
			}
			else {

				mapping->UpdateDatasourceName(datasources[i], newpath);
				wereMissingFilesUpdated = true;
			}
		}

		return wereMissingFilesUpdated;
	}

} //namespace SynGlyphX
