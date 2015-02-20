#include "glyphviewerantztransformer.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include "filesystem.h"
#include <boost/uuid/uuid_io.hpp>
#include "userdefinedbaseimageproperties.h"

namespace SynGlyphXANTz {

	GlyphViewerANTzTransformer::GlyphViewerANTzTransformer(const QString& cacheBaseDir) :
		ANTzTransformer(cacheBaseDir)
	{
		SynGlyphX::SourceDataManager::SetIntermediateDirectory(QDir::toNativeSeparators(cacheBaseDir + QDir::separator() + "intermediate"));
		SetSourceDataCacheLocation(QDir::toNativeSeparators(cacheBaseDir + QDir::separator() + "sourcedata.db"));
	}


	GlyphViewerANTzTransformer::~GlyphViewerANTzTransformer()
	{
	}

	void GlyphViewerANTzTransformer::CreateGlyphsFromMapping(const SynGlyphX::DataTransformMapping& mapping) {

		Clear();

		QString localOutputDir = QDir::toNativeSeparators(m_baseOutputDir + QDir::separator() + "antz" + QDir::separator());

		QStringList csvFiles;
		csvFiles.push_back(localOutputDir + "antz.csv");
		csvFiles.push_back(localOutputDir + "antztag.csv");

		QStringList cacheFiles = csvFiles;
		cacheFiles.push_back(m_sourceDataCacheLocation);
		
		QStringList baseImageFilenames;

		QString cachedMappingFilename = m_baseOutputDir + QDir::separator() + "mapping.sdt";

		if (DoesCacheNeedToBeRegenerated(mapping, cacheFiles, cachedMappingFilename)) {

			QDir antzOutputDir(localOutputDir);
			if (antzOutputDir.exists()) {

				SynGlyphX::Filesystem::RemoveContentsOfDirectory(localOutputDir.toStdString());
			}
			else {

				QDir baseOutputDir(m_baseOutputDir);
				baseOutputDir.mkdir("antz");
			}

			GenerateCache(mapping, csvFiles, localOutputDir);
			//Write the mapping to the cache
			mapping.WriteToFile(cachedMappingFilename.toStdString());
		}
	}

	bool GlyphViewerANTzTransformer::DoesCacheNeedToBeRegenerated(const SynGlyphX::DataTransformMapping& mapping, const QStringList& cacheFilenames, const QString& mappingFilename) const {

		if (cacheFilenames.empty()) {

			return true;
		}

		Q_FOREACH(QString cacheFilename, cacheFilenames) {

			QFile cacheFile(cacheFilename);
			if (!cacheFile.exists()) {
				return true;
			}
		}
		/*
		if (!baseImageFilename.isEmpty()) {

		QFile baseImage(baseImageFilename);
		if (!baseImage.exists()) {

		return true;
		}
		else if (mapping.GetBaseObjects()[0].GetType() == SynGlyphX::BaseImage::Type::UserImage) {

		const SynGlyphX::UserDefinedBaseImageProperties* const properties = dynamic_cast<const SynGlyphX::UserDefinedBaseImageProperties* const>(mapping.GetBaseObjects()[0].GetProperties());
		if (HasFileBeenUpdated(properties->GetFilename(), boost::filesystem::last_write_time(baseImageFilename.toStdWString()))) {

		return true;
		}
		}
		}*/

		QFile cachedMappingFile(mappingFilename);
		if (!cachedMappingFile.exists()) {

			return true;
		}

		SynGlyphX::DataTransformMapping cachedMapping;
		cachedMapping.ReadFromFile(mappingFilename.toStdString());
		if (cachedMapping != mapping) {

			return true;
		}

		boost::filesystem::path firstCSVFilePath(cacheFilenames[0].toStdString());
		return HaveDatasourcesBeenUpdated(mapping, boost::filesystem::last_write_time(firstCSVFilePath));
	}

	QString GlyphViewerANTzTransformer::GenerateBaseImageFilename(unsigned int index) const {

		return "base_image_" + QString::number(index) + ".png";
	}

} //namespace SynGlyphXANTz