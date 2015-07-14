#include "antzexporttransformer.h"
#include <QtCore/QDir>
#include "filesystem.h"
#include "antzcsvwriter.h"
#include <algorithm>

namespace SynGlyphXANTz {

	QString ANTzExportTransformer::s_logoFilename;

	ANTzExportTransformer::ANTzExportTransformer(const QString& baseOutputDir, const QString& antzTemplateDir, const QString& worldImageFilename, bool useOldANTzFilenames) :
		ANTzTransformer(baseOutputDir),
		m_antzTemplateDir(antzTemplateDir),
		m_worldImageFilename(worldImageFilename),
		m_useOldANTzFilenames(useOldANTzFilenames)
	{
		SetSourceDataCacheLocation(QDir::toNativeSeparators(baseOutputDir + QDir::separator() + "sourcedata.db"));
	}


	ANTzExportTransformer::~ANTzExportTransformer()
	{
	}

	void ANTzExportTransformer::SetLogoFilename(const QString& logoFilename) {

		s_logoFilename = logoFilename;
	}

	void ANTzExportTransformer::Prepare() {

		QDir dir(m_baseOutputDir);
		if (!dir.exists()) {

			if (!dir.mkpath(m_baseOutputDir)) {

				throw std::exception("Instance directory was not created");
			}
		}
		else {

			SynGlyphX::Filesystem::RemoveContentsOfDirectory(m_baseOutputDir.toStdString());
		}
	}

	void ANTzExportTransformer::CreateGlyphsFromMapping(const SynGlyphX::DataTransformMapping& mapping) {

		Clear();

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

		SynGlyphXANTz::ANTzCSVWriter::GetInstance().WriteGlobals((baseUsrCSVDir + "antzglobals.csv").toStdString(), mapping.GetSceneProperties().GetBackgroundColor());
	}

	void ANTzExportTransformer::GenerateGrids(std::vector<ANTzGrid>& grids, const SynGlyphX::DataTransformMapping& mapping, const QString& baseImageFilenameDirectory) {

		ANTzTransformer::GenerateGrids(grids, mapping, baseImageFilenameDirectory);

		unsigned int logoTextureID = (*std::max_element(m_textureIDs.begin(), m_textureIDs.end())) + 1;

		CopyImage(s_logoFilename, baseImageFilenameDirectory + GenerateBaseImageFilename(logoTextureID));

		ANTzGrid grid;
		grid.SetPosition({ { 163.7, 86.3, 0.5 } });
		grid.SetRotation({ { 0.0, 0.0, 0.0 } });
		grid.SetScale({ { 0.09, 0.04, 1.0 } });
		grid.SetTextureID(logoTextureID);
		grid.SetVisible(false);

		grids.push_back(grid);
	}

} //namespace SynGlyphXANTz