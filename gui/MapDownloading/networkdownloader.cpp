#include "networkdownloader.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include "downloadexception.h"
#include <QtGui/QImage>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <cmath>
#include <QtGui/QDesktopServices>

/*

Latitude: 1 deg = 110.54 km
Longitude: 1 deg = 111.320*cos(latitude) km

*/

const double PI = 3.14159;
const unsigned int MaxZoomLevel = 18;
const double MetersPerPixelAtZoom0 = 156543.034;
const double EarthRadiusInMeters = 6372798.2;
const double MetersPerDegreeLongitude = 111319.892;
const double MetersPerDegreeLatitude = 110540.0;
const double DegToRad = PI / 180.0;

NetworkDownloader NetworkDownloader::s_instance;
const char* NetworkDownloader::ImageFormat("PNG");

NetworkDownloader::NetworkDownloader() :
	m_distanceStrategy(EarthRadiusInMeters)
{
    ReadSettings();

	for (unsigned int i = 1; i <= MaxZoomLevel; ++i) {

		m_mapQuestMetersPerPixelAtEquator[i] = MetersPerPixelAtZoom0 / std::pow(2.0, static_cast<double>(i));
	}
}

NetworkDownloader::~NetworkDownloader()
{
	
}

void NetworkDownloader::ReadSettings() {

    QSettings settings("SynGlyphX", "MapDownloading");
    settings.beginGroup("MapQuestOpenSettings");

    m_mapQuestOpenKey = settings.value("AppKey", "").toString();

    //m_showPointsInMap = settings.value("AddPOIs", false).toBool();
    settings.endGroup();
}

void NetworkDownloader::WriteSettings() {

    QSettings settings("SynGlyphX", "MapDownloading");
    settings.beginGroup("MapQuestOpenSettings");
    settings.setValue("AppKey", m_mapQuestOpenKey);
    //settings.setValue("AddPOIs", m_showPointsInMap);
    settings.endGroup();
}

NetworkDownloader& NetworkDownloader::Instance() {

	return s_instance;
}
/*
void NetworkDownloader::SetShowPointsInMap(bool show) {

	m_showPointsInMap = show;
    WriteSettings();
}

bool NetworkDownloader::GetShowPointsInMap() {

	return m_showPointsInMap;
}
*/
GeographicBoundingBox NetworkDownloader::DownloadMap(const std::vector<GeographicPoint>& points, SynGlyphX::DownloadedMapProperties::ConstSharedPtr properties) {

	GeographicBoundingBox pointsBoundingBox(points); //Set to sw and ne constructor
	imageSize = properties->GetSize();
	unsigned int zoomLevel = GetZoomLevel(pointsBoundingBox, properties->GetUseBestFit());

	if (properties->GetUseBestFit()) {
		imageSize[0] += 2 * properties->GetMargin();
		imageSize[1] += 2 * properties->GetMargin();

		unsigned int maxSize = SynGlyphX::DownloadedMapProperties::MaxSizeMapQuest;
		if (properties->GetSource() == SynGlyphX::DownloadedMapProperties::GoogleMaps) {

			maxSize = SynGlyphX::DownloadedMapProperties::MaxSizeGoogleMaps;
		}
		imageSize[0] = std::min(imageSize[0], maxSize);
		imageSize[1] = std::min(imageSize[1], maxSize);
	}
	double cosineAtCenter = std::cos(pointsBoundingBox.GetCenter().get<1>() * DegToRad);
	double metersPerPixelAtCurrentZoom = m_mapQuestMetersPerPixelAtEquator[zoomLevel] * cosineAtCenter;

	double lonRadiusInDegrees = (metersPerPixelAtCurrentZoom  * (imageSize[0] / 2.0)) / (MetersPerDegreeLongitude  * cosineAtCenter);
	double latRadiusInDegrees = (metersPerPixelAtCurrentZoom * (imageSize[1] / 2.0)) / MetersPerDegreeLatitude;

	return GeographicBoundingBox(pointsBoundingBox.GetCenter(), latRadiusInDegrees, lonRadiusInDegrees);
}

GeographicBoundingBox NetworkDownloader::DownloadMap(const std::vector<GeographicPoint>& points, const std::string& filename, SynGlyphX::DownloadedMapProperties::ConstSharedPtr properties) {
	
	GeographicBoundingBox pointsBoundingBox(points); //Set to sw and ne constructor
	imageSize = properties->GetSize();
	unsigned int zoomLevel = GetZoomLevel(pointsBoundingBox, properties->GetUseBestFit());

	if (properties->GetUseBestFit()) {

		imageSize[0] += 2 * properties->GetMargin();
		imageSize[1] += 2 * properties->GetMargin();

		unsigned int maxSize = SynGlyphX::DownloadedMapProperties::MaxSizeMapQuest;

		if (properties->GetSource() == SynGlyphX::DownloadedMapProperties::GoogleMaps) {

			maxSize = SynGlyphX::DownloadedMapProperties::MaxSizeGoogleMaps;
		}

		imageSize[0] = std::min(imageSize[0], maxSize);
		imageSize[1] = std::min(imageSize[1], maxSize);
	}

	double cosineAtCenter = std::cos(pointsBoundingBox.GetCenter().get<1>() * DegToRad);
	double metersPerPixelAtCurrentZoom = m_mapQuestMetersPerPixelAtEquator[zoomLevel] * cosineAtCenter;

	QString imageUrl;

    if (properties->GetSource() == SynGlyphX::DownloadedMapProperties::MapQuestOpen) {
		imageUrl = GenerateMapQuestOpenString(pointsBoundingBox.GetCenter(), zoomLevel, properties);
	}
	else {
		//eventually will implement download from Google Maps and other sources
	}

    //Leave this line in for testing purposes if needed later
    //QDesktopServices::openUrl(QUrl(imageUrl));
	 
	QNetworkRequest request(imageUrl);
    QNetworkAccessManager networkManager;
	QNetworkReply *reply = networkManager.get(request);

	QEventLoop loop;
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

	// Execute the event loop here, now we will wait here until readyRead() signal is emitted
	// which in turn will trigger event loop quit.
	if (reply->isRunning()) {
		loop.exec();
	}

	reply->deleteLater();

	if (reply->error() != QNetworkReply::NoError) {
		qDebug() << (reply->errorString().toStdString()).c_str();
		std::cout << "Network Error: " << reply->errorString().toStdString() << std::endl;
		throw DownloadException(("Network Error: "+ reply->errorString().toStdString()).c_str());
	}

	QByteArray imageData = reply->readAll();
	if (imageData.isEmpty()) {
		
		throw DownloadException("Download returned no data");
	}

    QImage image = QImage::fromData(imageData, ImageFormat);
	if (image.isNull()) {
        
		
		throw DownloadException("Failed to get data into image");
	}

	if (properties->GetGrayscale()) {

		int depthInBytes = image.depth() / 8;
		for (int y = 0; y < image.height(); ++y) {

			uchar* scanLine = image.scanLine(y);
			for (int x = 0; x < image.width(); ++x) {

				QRgb* rgb = reinterpret_cast<QRgb*>(scanLine + x*depthInBytes);
				unsigned int gray = qGray(*rgb);
				*rgb = qRgb(gray, gray, gray);
			}
		}
	}

	if (properties->GetInvert()) {

		image.invertPixels(QImage::InvertMode::InvertRgb);
	}

	if (!image.save(QString::fromStdString(filename), ImageFormat)) {
		
		throw DownloadException("Failed to save image");
	}

    //double lonRadiusInDegrees = std::abs(((MetersPerPixelAtZoom0 * std::cos(pointsBoundingBox.GetCenter().get<1>() * DegToRad) / std::pow(2.0, zoomLevel)) * (imageSize.width() / 2.0)) / MetersPerDegreeLongitude);
	double lonRadiusInDegrees = (metersPerPixelAtCurrentZoom  * (imageSize[0] / 2.0)) / (MetersPerDegreeLongitude  * cosineAtCenter);
	//double latRadiusInDegrees = lonRadiusInDegrees * (imageSize.height() / static_cast<double>(imageSize.width()));
	double latRadiusInDegrees = (metersPerPixelAtCurrentZoom * (imageSize[1] / 2.0)) / MetersPerDegreeLatitude;

	return GeographicBoundingBox(pointsBoundingBox.GetCenter(), latRadiusInDegrees, lonRadiusInDegrees);
}

unsigned int NetworkDownloader::GetZoomLevel(const GeographicBoundingBox& boundingBox, bool useBestFit) {

	double cosineAtCenter = std::cos(boundingBox.GetCenter().get<1>() * DegToRad);

	double hDistanceInMeters = std::abs(boost::geometry::distance(boundingBox.GetWestCenter(), boundingBox.GetEastCenter(), m_distanceStrategy));
	if (boundingBox.GetWidth() > 180.0) {

		hDistanceInMeters = (EarthRadiusInMeters * cosineAtCenter * 2.0 * PI) - hDistanceInMeters;
	}
	double vDistanceInMeters = std::abs(boost::geometry::distance(boundingBox.GetNorthCenter(), boundingBox.GetSouthCenter(), m_distanceStrategy));

	if (useBestFit) {

		for (unsigned int i = MaxZoomLevel; i > 0; --i) {

			double metersPerPixelAtZoom = m_mapQuestMetersPerPixelAtEquator[i] * cosineAtCenter;
			SynGlyphX::DoubleSize imageSizeAtZoom;
			imageSizeAtZoom[0] = hDistanceInMeters / metersPerPixelAtZoom;
			imageSizeAtZoom[1] = vDistanceInMeters / metersPerPixelAtZoom;
			if ((imageSizeAtZoom[0] < imageSize[0]) && (imageSizeAtZoom[1] < imageSize[1])) {

				imageSize[0] = std::ceil(imageSizeAtZoom[0]);
				imageSize[1] = std::ceil(imageSizeAtZoom[1]);
				return i;
			}
		}
	}

	double hZoomLevel = std::log((MetersPerPixelAtZoom0 * cosineAtCenter) / (hDistanceInMeters / imageSize[0])) / std::log(2.0); // / cosineAtCenter);
	double vZoomLevel = std::log((MetersPerPixelAtZoom0 * cosineAtCenter) / (vDistanceInMeters / imageSize[1])) / std::log(2.0);

	//Zoom level can never go above 18 on MapQuest Open
	return std::min(static_cast<unsigned int>(std::min(hZoomLevel, vZoomLevel)), MaxZoomLevel);
}

QString NetworkDownloader::GenerateMapQuestOpenString(const GeographicPoint& center, unsigned int zoomLevel, SynGlyphX::DownloadedMapProperties::ConstSharedPtr properties) {

	if (m_mapQuestOpenKey.trimmed().isEmpty()) {

		throw DownloadException("MapQuest Open key is empty.");
	}

	QString url = "http://open.mapquestapi.com/staticmap/v4/getmap?key=" + m_mapQuestOpenKey;
	url += "&center=" + QString::number(center.get<1>()) + "," + QString::number(center.get<0>());
	url += "&zoom=" + QString::number(zoomLevel) + "&size=" + QString::number(imageSize[0]) + "," + QString::number(imageSize[1]);
	url += "&type=";
	if (properties->GetType() == SynGlyphX::DownloadedMapProperties::Map) {
		url += "map";
	}
	else if (properties->GetType() == SynGlyphX::DownloadedMapProperties::Satellite) {
		url += "sat";
	}
	else {
		url += "hyb";
	}
	url += "&imagetype=png";
	url += "&pois=";
	//url += "&declutter=true";

	return url;
}

void NetworkDownloader::SetMapQuestOpenKey(const QString& key) {

    m_mapQuestOpenKey = key;
    WriteSettings();
}

const QString& NetworkDownloader::GetMapQuestOpenKey() const {

    return m_mapQuestOpenKey;
}

std::vector<double> NetworkDownloader::getImageSize(){

	std::vector<double> temp = { (double)imageSize[0], (double)imageSize[1] };
	return temp;
}
