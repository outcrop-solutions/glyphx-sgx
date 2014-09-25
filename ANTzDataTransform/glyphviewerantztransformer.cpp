#include "glyphviewerantztransformer.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>

GlyphViewerANTzTransformer::GlyphViewerANTzTransformer(const QString& cacheBaseDir) :
	ANTzTransformer(cacheBaseDir)
{
}


GlyphViewerANTzTransformer::~GlyphViewerANTzTransformer()
{
}

void GlyphViewerANTzTransformer::CreateGlyphsFromMapping(SynGlyphX::DataTransformMapping& mapping) {

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

	QString mappingVersion = QString::number(mapping.GetVersion());

	QStringList csvFiles;
	csvFiles.push_back(cacheDir + QDir::separator() + "antz_" + mappingVersion + ".csv");
	csvFiles.push_back(cacheDir + QDir::separator() + "antztag_" + mappingVersion + ".csv");
	QString baseImageFilename;
	if (useNonDefaultImage) {

		baseImageFilename = cacheDir + QDir::separator() + "baseimage_" + mappingVersion + ".jpg";
	}
	else {

		baseImageFilename = "";
	}

	if (DoesCacheNeedToBeRegenerated(mapping, csvFiles, baseImageFilename)) {

		GenerateCache(mapping, csvFiles, baseImageFilename);
	}
}

bool GlyphViewerANTzTransformer::DoesCacheNeedToBeRegenerated(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilename) const {

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

	boost::filesystem::path firstCSVFilePath(csvFilenames[0].toStdString());
	return HaveDatasourcesBeenUpdated(mapping, boost::filesystem::last_write_time(firstCSVFilePath));
}