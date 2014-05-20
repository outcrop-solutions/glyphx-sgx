#include "downloadedmap.h"
#include <QtCore/QProcess>
#include <QtWidgets/QMessageBox>
#include "application.h"
#include <QtCore/QDir>
#include "downloadexception.h"
#include "networkdownloader.h"

DownloadedMap::DownloadedMap(const std::vector<GeographicPoint>& points, const std::string& filename, const QSize& imageSize, QObject *parent)
    : QObject(parent),
	m_filename(filename),
    m_imageBoundingBox(),
    m_pointsBoundingBox(points)
{
	QString tempImageFilename = QDir::tempPath() + "/tempimage.png";

	NetworkDownloader& downloader = NetworkDownloader::Instance();
	
	m_imageBoundingBox = downloader.DownloadMap(points, tempImageFilename.toStdString(), imageSize);

	m_mapSource = downloader.GetMapSource();
	m_mapType = downloader.GetMapType();
	m_showPointsInMap = downloader.GetShowPointsInMap();

	QFile::copy(tempImageFilename, QString::fromStdString(filename));

	//CreateGeoreferencedImage(tempImageFilename.toStdString());

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

void DownloadedMap::CreateGeoreferencedImage(const std::string& inputImage) {

	//This should probably use GDAL as a library but for faster initial development this will call gdal_translate

	QProcess process;
	process.setProgram(SynGlyphX::Application::applicationDirPath() + "/gdal_translate.exe");

	QStringList args;
	args.append("-of"); 
	args.append(NetworkDownloader::ImageFormat);
	
	//Add bounding box in format of <top_left_lon> <top_left_lat> <bottom_right_lon> <bottom_right_lat>
	args.append("-a_ullr");
	args.append(QString::number(m_imageBoundingBox.GetSWCorner().get<0>()));
    args.append(QString::number(m_imageBoundingBox.GetNECorner().get<1>()));
    args.append(QString::number(m_imageBoundingBox.GetNECorner().get<0>()));
    args.append(QString::number(m_imageBoundingBox.GetSWCorner().get<1>()));
	
	//Mapquest images are in spherical mercator so give it that geo reference
	args.append("-a_srs EPSG: 4326");

	args.append(QString::fromStdString(inputImage));
	args.append(QString::fromStdString(m_filename));

	process.setArguments(args);
	
	process.start();

	if (!process.waitForFinished()) {
		
		throw DownloadException("Failed to georeference: gdal_translate failed to finish");
	}

	if (process.exitStatus() != QProcess::NormalExit) {
		
		throw DownloadException("Failed to georeference: gdal_translate failed");
	}
}