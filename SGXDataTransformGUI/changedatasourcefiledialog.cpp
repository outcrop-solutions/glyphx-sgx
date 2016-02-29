#include "changedatasourcefiledialog.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <boost/filesystem.hpp>

namespace SynGlyphX {

	ChangeDatasourceFileDialog::ChangeDatasourceFileDialog(const FileDatasource& oldDatasourceFile, const QString& acceptButtonText, DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent)
		: ReplaceFilenameDialog(QString::fromStdWString(oldDatasourceFile.GetDBName()), acceptButtonText, parent),
		m_fileDatasource(oldDatasourceFile),
		m_dataEngineConnection(dataEngineConnection)
	{
		setWindowTitle(tr("Change Datasource File"));

		if (oldDatasourceFile.CanDatasourceHaveMultipleTables()) {

			m_oldDatasourceTables.insert(oldDatasourceFile.GetTableNames().begin(), oldDatasourceFile.GetTableNames().end());
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

						m_dataEngineConnection->connectToServer(QString("sqlite:") + QString::fromStdWString(m_fileDatasource.GetFilename()), "", "", "sqlite3");
						QStringList newTables = m_dataEngineConnection->getTables();
						for (const std::wstring& oldTable : m_oldDatasourceTables) {

							if (!newTables.contains(QString::fromStdWString(oldTable))) {

								return false;
							}
						}
					}
					
					return true;
				}
			}
		}
		catch (const std::exception& e) {


		}
		m_dataEngineConnection->closeConnection();

		return false;
	}

	bool ChangeDatasourceFileDialog::UpdateDatasourceFiles(const std::vector<boost::uuids::uuid>& datasources, const QString& sdtfilename, DataTransformMapping::SharedPtr mapping, DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget* dialogParent) {

		bool wereMissingFilesUpdated = false;
		for (int i = 0; i < datasources.size(); ++i) {

			QString newpath = IsFileInSameDirectory(mapping->GetDatasources().at(datasources[i])->GetFormattedName(), sdtfilename);
			if (newpath.isEmpty()){

				QString acceptButtonText = tr("Next");
				if (i == datasources.size() - 1) {

					acceptButtonText = tr("Ok");
				}

				ChangeDatasourceFileDialog dialog(*std::dynamic_pointer_cast<FileDatasource>(mapping->GetDatasources().at(datasources[i])), acceptButtonText, dataEngineConnection, dialogParent);
				if (dialog.exec() == QDialog::Accepted) {

					mapping->UpdateDatasourceName(datasources[i], dialog.GetNewFilename().toStdWString());
					wereMissingFilesUpdated = true;
				}
				else {

					throw std::runtime_error("One or more datasources weren't found.");
				}
			}
			else {

				mapping->UpdateDatasourceName(datasources[i], newpath.toStdWString());
				wereMissingFilesUpdated = true;
			}
		}

		return wereMissingFilesUpdated;
	}

	QString ChangeDatasourceFileDialog::IsFileInSameDirectory(std::wstring datasourcename, QString sdtpath){

		QString source = QString::fromStdWString(datasourcename);
		int lstIndex = sdtpath.lastIndexOf(QRegExp("[\/]"));
		sdtpath.truncate(lstIndex);
		boost::filesystem::path path(sdtpath.toStdString());

		if (boost::filesystem::is_directory(path)){

			boost::filesystem::path match((sdtpath + QDir::separator() + source).toStdString());
			if (boost::filesystem::exists(match)){
				return QString(boost::filesystem::canonical(match).string().c_str());
			}

		}
		return QString();

	}

} //namespace SynGlyphX