#include "changedatasourcefiledialog.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include "databaseinfo.h"
#include <boost/filesystem.hpp>

namespace SynGlyphX {

	ChangeDatasourceFileDialog::ChangeDatasourceFileDialog(const FileDatasource& oldDatasourceFile, const QString& acceptButtonText, QWidget *parent)
		: ReplaceFilenameDialog(QString::fromStdWString(oldDatasourceFile.GetDBName()), acceptButtonText, parent),
		m_fileSourceType(oldDatasourceFile.GetType())
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

		if (m_fileSourceType == FileDatasource::SourceType::SQLITE3) {

			if (DatabaseInfo::IsSQLiteDB(newDatasource)) {

				QStringList tables = DatabaseInfo::GetListOfTablesWithoutAutogeneratedTablesInDatabaseFile(newDatasource);
				tables.sort();
				return (m_oldDatasourceTables == tables);
			}
		}
		else if (m_fileSourceType == FileDatasource::SourceType::CSV) {

			if (newDatasource.right(4) == ".csv") {

				QFile csvFile(newDatasource);
				QFile csvtFile(newDatasource + 't');

				if (csvFile.exists() && csvtFile.exists()) {

					return true;
				}
			}
		}

		return false;
	}

	bool ChangeDatasourceFileDialog::UpdateDatasourceFiles(const std::vector<boost::uuids::uuid>& datasources, QString sdtfilename, DataTransformMapping::SharedPtr mapping, QWidget* dialogParent) {

		bool wereMissingFilesUpdated = false;
		for (int i = 0; i < datasources.size(); ++i) {

			QString newpath = IsFileInSameDirectory(mapping->GetDatasources().GetFileDatasources().at(datasources[i]).GetFormattedName(), sdtfilename);
			if (newpath.isEmpty()){

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

					throw std::exception("One or more datasources weren't found.");
				}
			}
			else{

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