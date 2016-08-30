#include "downloadedmap.h"
#include <QtCore/QProcess>
#include <QtWidgets/QMessageBox>
#include "application.h"
#include <QtCore/QDir>
#include "downloadexception.h"
#include "networkdownloader.h"
#include "georeference.h"

DownloadedMap::DownloadedMap(const std::vector<GeographicPoint>& points, const std::string& filename, SynGlyphX::DownloadedMapProperties::ConstSharedPtr properties)
    : m_filename(filename),
    m_imageBoundingBox(),
    m_pointsBoundingBox(points),
    m_mapSource(properties->GetSource()),
	m_mapType(properties->GetType())
{
    QString tempImageFilename = QDir::toNativeSeparators(QDir::tempPath() + "/tempimage.png");

	NetworkDownloader& downloader = NetworkDownloader::Instance();
	
	m_imageBoundingBox = downloader.DownloadMap(points, tempImageFilename.toStdString(), properties);

	//m_showPointsInMap = downloader.GetShowPointsInMap();

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

SynGlyphX::DownloadedMapProperties::MapSource DownloadedMap::GetMapSource() const {

	return m_mapSource;
}

SynGlyphX::DownloadedMapProperties::MapType DownloadedMap::GetMapType() const {

	return m_mapType;
}
/*
bool DownloadedMap::GetShowPointsInMap() const {
	
	return m_showPointsInMap;
}*/