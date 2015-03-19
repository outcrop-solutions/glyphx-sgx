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
		SetSourceDataCacheLocation(QDir::toNativeSeparators(cacheBaseDir + QDir::separator() + "sourcedata.db"));
	}


	GlyphViewerANTzTransformer::~GlyphViewerANTzTransformer()
	{
	}

	void GlyphViewerANTzTransformer::Prepare() {

		QDir dir(m_baseOutputDir);
		if (!dir.exists()) {

			if (!dir.mkpath(m_baseOutputDir)) {

				throw std::exception("Instance directory was not created");
			}
		}
		
		if (IsCacheAnOldVersion()) {

			SynGlyphX::Filesystem::RemoveContentsOfDirectory(m_baseOutputDir.toStdString());
		}
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

	bool GlyphViewerANTzTransformer::IsCacheAnOldVersion() const {

		QDir dir(m_baseOutputDir);

		//If the cache directory is not empty and it does not have a source data cache, then it's an old version
		//of the cache so delete the contents of the directory so that there is no problem with an old version of
		//the cache
		QFileInfo fileInfo(m_sourceDataCacheLocation);
		if ((!fileInfo.exists()) && (dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() > 0)) {

			return true;
		}

		return false;
	}

} //namespace SynGlyphXANTz