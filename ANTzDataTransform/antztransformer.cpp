#include "antztransformer.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <boost/filesystem.hpp>
#include "databaseservices.h"
#include "csvreaderwriter.h"
#include "downloadedmapproperties.h"
#include "networkdownloader.h"

ANTzTransformer::ANTzTransformer(const QString& cacheBaseDir) :
	SynGlyphX::Transformer(),
	m_cacheBaseDir(cacheBaseDir)
{

}

ANTzTransformer::~ANTzTransformer()
{

}

void ANTzTransformer::CreateGlyphsFromMapping(SynGlyphX::DataTransformMapping& mapping) {

	QString cacheSubDir = "cache_" + QString::fromStdWString(boost::uuids::to_wstring(mapping.GetID()));

	QDir dir(m_cacheBaseDir);
	if (!dir.exists(cacheSubDir)) {
		if (!dir.mkdir(cacheSubDir)) {
			throw std::exception("Cache directory was not created");
		}
	}

	QString cacheDir = m_cacheBaseDir + QDir::separator() + cacheSubDir;

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

	m_generatedFilenames = csvFiles;
	m_baseImageFilename = baseImageFilename;
}

bool ANTzTransformer::DoesCacheNeedToBeRegenerated(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilename) const {

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

void ANTzTransformer::GenerateCache(SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilename) {

	SynGlyphX::DatabaseServices::AddDatabaseConnections(mapping.GetDatasources(), mapping.GetDatasources().size());

	const SynGlyphX::BaseImage& baseImage = mapping.GetBaseImage();

	if (baseImage.GetType() == SynGlyphX::BaseImage::Type::DownloadedMap) {

		DownloadBaseImage(mapping, baseImageFilename);
	}
	
	SynGlyphX::GlyphTree::ConstSharedVector trees = CreateGlyphTreesFromMinMaxTrees(mapping);

	Q_FOREACH(QString csvFilename, csvFilenames) {

		if (QFile::exists(csvFilename)) {
			
			if (!QFile::remove(csvFilename)) {

				throw std::exception("Failed to remove old cache");
			}
		}
	}

	SynGlyphX::CSVReaderWriter& writer = SynGlyphX::CSVReaderWriter::GetInstance();
	writer.Write(csvFilenames[0].toStdString(), csvFilenames[1].toStdString(), trees);
}

const QStringList& ANTzTransformer::GetGeneratedFilenames() const {

	return m_generatedFilenames;
}

const QString& ANTzTransformer::GetBaseImageFilename() const {

	return m_baseImageFilename;
}

void ANTzTransformer::DownloadBaseImage(SynGlyphX::DataTransformMapping& mapping, const QString& baseImageFilename) const {

	if (QFile::exists(baseImageFilename)) {

		if (!QFile::remove(baseImageFilename)) {

			throw std::exception("Failed to remove old base image");
		}
	}

	std::vector<GeographicPoint> points;
	mapping.GetPositionXYForAllGlyphTrees(points);
	const SynGlyphX::DownloadedMapProperties* const properties = dynamic_cast<const SynGlyphX::DownloadedMapProperties* const>(mapping.GetBaseImage().GetProperties());
	NetworkDownloader& downloader = NetworkDownloader::Instance();
	GeographicBoundingBox boundingBox = downloader.DownloadMap(points, baseImageFilename.toStdString(), properties);
	mapping.SetPositionXYMinMaxToGeographicForAllGlyphTrees(boundingBox);
}