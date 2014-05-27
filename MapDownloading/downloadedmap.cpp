#include "downloadedmap.h"
#include <QtCore/QProcess>
#include <QtWidgets/QMessageBox>
#include "application.h"
#include <QtCore/QDir>
#include "downloadexception.h"
#include "networkdownloader.h"
#include "georeference.h"

DownloadedMap::DownloadedMap(const std::vector<GeographicPoint>& points, const std::string& filename, const QSize& imageSize, NetworkDownloader::MapSource source, NetworkDownloader::MapType mapType, QObject *parent)
    : QObject(parent),
	m_filename(filename),
    m_imageBoundingBox(),
    m_pointsBoundingBox(points),
    m_mapSource(source),
    m_mapType(mapType)
{
    QString tempImageFilename = QDir::toNativeSeparators(QDir::tempPath() + "/tempimage.png");

	NetworkDownloader& downloader = NetworkDownloader::Instance();
	
	m_imageBoundingBox = downloader.DownloadMap(points, tempImageFilename.toStdString(), imageSize, source, mapType);

	m_showPointsInMap = downloader.GetShowPointsInMap();

    QString imageFilename = QString::fromStdString(filename);

    //if image with this filename exists delete it
    if (QFile::exists(imageFilename)) {
        QFile::remove(imageFilename);
    }

    //QFile::copy(tempImageFilename, imageFilename);

    SynGlyphX::GeoReference& geoReference = SynGlyphX::GeoReference::Instance();
    geoReference.GeoReferenceImage(tempImageFilename.toStdString(), imageFilename.toStdString(), m_imageBoundingBox);

	//Cleanup temporary image file since GDAL will make a copy
	QFile::remove(tempImageFilename);
}

DownloadedMap::~DownloadedMap()
{

}

const GeographicBoundingBox& DownloadedMap::GetImageBoundingBox() {

    return m_imageBoundingBox;
}

const GeographicBoundingBox& DownloadedMap::GetPointsBoundingBox() {

    return m_pointsBoundingBox;
}

NetworkDownloader::MapSource DownloadedMap::GetMapSource() {

	return m_mapSource;
}

NetworkDownloader::MapType DownloadedMap::GetMapType() {

	return m_mapType;
}

bool DownloadedMap::GetShowPointsInMap() {
	
	return m_showPointsInMap;
}