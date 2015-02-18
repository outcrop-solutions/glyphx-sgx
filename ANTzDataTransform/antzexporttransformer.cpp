#include "antzexporttransformer.h"
#include <QtCore/QDir>
#include "filesystem.h"

namespace SynGlyphXANTz {

	ANTzExportTransformer::ANTzExportTransformer(const QString& baseOutputDir, const QString& antzTemplateDir, const QString& worldImageFilename, bool useOldANTzFilenames) :
		ANTzTransformer(baseOutputDir),
		m_antzTemplateDir(antzTemplateDir),
		m_worldImageFilename(worldImageFilename),
		m_useOldANTzFilenames(useOldANTzFilenames)
	{
	}


	ANTzExportTransformer::~ANTzExportTransformer()
	{
	}

	void ANTzExportTransformer::CreateGlyphsFromMapping(const SynGlyphX::DataTransformMapping& mapping) {

		Clear();

		QDir dir(m_baseOutputDir);
		if (!dir.exists()) {
			if (!dir.mkpath(m_baseOutputDir)) {
				throw std::exception("Instance directory was not created");
			}
		}
		else {

			SynGlyphX::Filesystem::RemoveContentsOfDirectory(m_baseOutputDir.toStdString());
		}

		QString baseUsrImageDir = QDir::toNativeSeparators(m_baseOutputDir + QDir::separator() + "usr" + QDir::separator() + "images" + QDir::separator());
		QString baseUsrCSVDir = QDir::toNativeSeparators(m_baseOutputDir + QDir::separator() + "usr" + QDir::separator() + "csv" + QDir::separator());

		SynGlyphX::Filesystem::CopyDirectoryOverwrite(QDir::toNativeSeparators(m_antzTemplateDir).toStdString(), m_baseOutputDir.toStdString(), true);
		QFile::copy(m_worldImageFilename, baseUsrImageDir + "map00001.jpg");

		QStringList csvFiles;
		if (m_useOldANTzFilenames) {

			csvFiles.push_back(baseUsrCSVDir + "antz0001.csv");
			csvFiles.push_back(baseUsrCSVDir + "antztag0001.csv");
		}
		else {

			csvFiles.push_back(baseUsrCSVDir + "antz0001node.csv");
			csvFiles.push_back(baseUsrCSVDir + "antz0001tag.csv");
		}

		GenerateCache(mapping, csvFiles, baseUsrImageDir);
	}

} //namespace SynGlyphXANTz