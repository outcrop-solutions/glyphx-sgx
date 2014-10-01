#include "antztransformer.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <boost/filesystem.hpp>
#include "sourcedatamanager.h"
#include "csvreaderwriter.h"
#include "downloadedmapproperties.h"
#include "networkdownloader.h"

ANTzTransformer::ANTzTransformer(const QString& baseOutputDir) :
	SynGlyphX::Transformer(),
	m_baseOutputDir(baseOutputDir)
{

}

ANTzTransformer::~ANTzTransformer()
{

}

const QStringList& ANTzTransformer::GetCSVFilenames() const {

	return m_csvFilenames;
}

const QString& ANTzTransformer::GetBaseImageFilename() const {

	return m_baseImageFilename;
}

void ANTzTransformer::CreateGlyphsFromMapping(SynGlyphX::DataTransformMapping& mapping) {

	m_csvFilenames.clear();
	m_baseImageFilename = "";

	QDir dir(m_baseOutputDir);
	if (!dir.exists()) {
		if (!dir.mkpath(m_baseOutputDir)) {
			throw std::exception("Instance directory was not created");
		}
	}

	GenerateCache(mapping, m_csvFilenames, m_baseImageFilename);
}

void ANTzTransformer::GenerateCache(SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilename) {

	SynGlyphX::SourceDataManager::AddDatabaseConnections(mapping.GetDatasources());

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

	m_csvFilenames = csvFilenames;
	m_baseImageFilename = baseImageFilename;
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