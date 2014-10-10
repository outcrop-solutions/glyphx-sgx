#include "glyphviewerantztransformer.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>

GlyphViewerANTzTransformer::GlyphViewerANTzTransformer(const QString& cacheBaseDir) :
	ANTzTransformer(cacheBaseDir)
{
	SynGlyphX::SourceDataManager::SetIntermediateDirectory(QDir::toNativeSeparators(cacheBaseDir + QDir::separator() + "intermediate"));
}


GlyphViewerANTzTransformer::~GlyphViewerANTzTransformer()
{
}

void GlyphViewerANTzTransformer::CreateGlyphsFromMapping(const SynGlyphX::DataTransformMapping& mapping) {

	QString cacheSubDir = "cache_" + QString::fromStdWString(boost::uuids::to_wstring(mapping.GetID()));

	QDir dir(m_baseOutputDir);
	if (!dir.exists(cacheSubDir)) {
		if (!dir.mkdir(cacheSubDir)) {
			throw std::exception("Cache directory was not created");
		}
	}

	m_csvFilenames.clear();
	m_baseImageFilename = "";

	QString cacheDir = m_baseOutputDir + QDir::separator() + cacheSubDir;

	bool useNonDefaultImage = (mapping.GetBaseImage().GetType() != SynGlyphX::BaseImage::Type::Default);

	QStringList csvFiles;
	csvFiles.push_back(cacheDir + QDir::separator() + "antz.csv");
	csvFiles.push_back(cacheDir + QDir::separator() + "antztag.csv");
	QString baseImageFilename;
	if (useNonDefaultImage) {

		baseImageFilename = cacheDir + QDir::separator() + "baseimage.jpg";
	}

	QString cachedMappingFilename = cacheDir + QDir::separator() + "mapping.sdt";

	if (DoesCacheNeedToBeRegenerated(mapping, csvFiles, baseImageFilename, cachedMappingFilename)) {

		GenerateCache(mapping, csvFiles, baseImageFilename);
		//Write the mapping to the cache
		mapping.WriteToFile(cachedMappingFilename.toStdString());
	}
}

bool GlyphViewerANTzTransformer::DoesCacheNeedToBeRegenerated(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilename, const QString& mappingFilename) const {

	if (csvFilenames.empty()) {

		return true;
	}

	Q_FOREACH(QString csvFilename, csvFilenames) {

		QFile csvFile(csvFilename);
		if (!csvFile.exists()) {
			return true;
		}
	}

	if (!baseImageFilename.isEmpty()) {
		QFile baseImage(baseImageFilename);
		if (!baseImage.exists()) {
			return true;
		}
	}

	QFile cachedMappingFile(mappingFilename);
	if (!cachedMappingFile.exists()) {
	
		return true;
	}
	
	SynGlyphX::DataTransformMapping cachedMapping;
	cachedMapping.ReadFromFile(mappingFilename.toStdString());
	if (cachedMapping != mapping) {

		return true;
	}

	boost::filesystem::path firstCSVFilePath(csvFilenames[0].toStdString());
	return HaveDatasourcesBeenUpdated(mapping, boost::filesystem::last_write_time(firstCSVFilePath));
}