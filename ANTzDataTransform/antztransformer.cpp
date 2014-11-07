#include "antztransformer.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <boost/filesystem.hpp>
#include "sourcedatamanager.h"
#include "antzcsvwriter.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"
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

void ANTzTransformer::CreateGlyphsFromMapping(const SynGlyphX::DataTransformMapping& mapping) {

	m_csvFilenames.clear();
	m_baseImageFilename = "";

	QDir dir(m_baseOutputDir);
	if (!dir.exists()) {
		if (!dir.mkpath(m_baseOutputDir)) {
			throw std::exception("Instance directory was not created");
		}
	}

	QStringList csvFiles;
	csvFiles.push_back(QDir::toNativeSeparators(m_baseOutputDir + QDir::separator() + "usr" + QDir::separator() + "csv" + QDir::separator() + "antz0001.csv"));
	csvFiles.push_back(QDir::toNativeSeparators(m_baseOutputDir + QDir::separator() + "usr" + QDir::separator() + "csv" + QDir::separator() + "antztag0001.csv"));
	QString antzImageDirectory = QDir::toNativeSeparators(m_baseOutputDir + QDir::separator() + "usr" + QDir::separator() + "images" + QDir::separator());
	QString baseImageFilename = antzImageDirectory + "map00001.jpg";

	if (mapping.GetBaseObjects()[0].GetType() != SynGlyphX::BaseImage::Type::Default) {

		QFile::rename(baseImageFilename, antzImageDirectory + "world.jpg");
	}

	GenerateCache(mapping, csvFiles, baseImageFilename);
}

void ANTzTransformer::GenerateCache(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilename) {

	const SynGlyphX::BaseImage& baseImage = mapping.GetBaseObjects()[0];
	if (baseImage.GetType() == SynGlyphX::BaseImage::Type::DownloadedMap) {

		DownloadBaseImage(mapping, baseImageFilename);
	}
	else if (baseImage.GetType() == SynGlyphX::BaseImage::Type::UserImage) {

		CopyUserImage(mapping, baseImageFilename);
	}
	
	SynGlyphX::GlyphTree::ConstSharedVector trees = CreateGlyphTreesFromMinMaxTrees(mapping);

	Q_FOREACH(QString csvFilename, csvFilenames) {

		if (QFile::exists(csvFilename)) {
			
			if (!QFile::remove(csvFilename)) {

				throw std::exception("Failed to remove old cache");
			}
		}
	}

	SynGlyphX::ANTzCSVWriter& writer = SynGlyphX::ANTzCSVWriter::GetInstance();
	writer.Write(csvFilenames[0].toStdString(), csvFilenames[1].toStdString(), trees);

	m_csvFilenames = csvFilenames;
	m_baseImageFilename = baseImageFilename;
}

void ANTzTransformer::DownloadBaseImage(const SynGlyphX::DataTransformMapping& mapping, const QString& baseImageFilename) {

	if (QFile::exists(baseImageFilename)) {

		if (!QFile::remove(baseImageFilename)) {

			throw std::exception("Failed to remove old base image");
		}
	}

	std::vector<GeographicPoint> points;
	GetPositionXYForAllGlyphTrees(mapping, points);
	const SynGlyphX::DownloadedMapProperties* const properties = dynamic_cast<const SynGlyphX::DownloadedMapProperties* const>(mapping.GetBaseObjects()[0].GetProperties());
	NetworkDownloader& downloader = NetworkDownloader::Instance();
	m_overrideRootXYBoundingBox = downloader.DownloadMap(points, baseImageFilename.toStdString(), properties);
}

void ANTzTransformer::CopyUserImage(const SynGlyphX::DataTransformMapping& mapping, const QString& baseImageFilename) {

	const SynGlyphX::UserDefinedBaseImageProperties* const properties = dynamic_cast<const SynGlyphX::UserDefinedBaseImageProperties* const>(mapping.GetBaseObjects()[0].GetProperties());
	if (!QFile::copy(QString::fromStdWString(properties->GetFilename()), baseImageFilename)) {

		throw std::exception("Failed to copy base image");
	}
}