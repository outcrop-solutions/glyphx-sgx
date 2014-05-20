#include "networkdownloader.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include "downloadexception.h"
#include <QtGui/QImage>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <cmath>
#include <QtGui/QDesktopServices>

/*

Latitude: 1 deg = 110.54 km
Longitude: 1 deg = 111.320*cos(latitude) km

*/

const unsigned int MaxZoomLevel = 18;
const double MetersPerPixel = 156543.034;
const double EarthRadiusInMeters = 6372798.2;
const double MetersPerDegreeLongitude = 111319.892;
const double DegToRad = 3.14159 / 180.0;

NetworkDownloader NetworkDownloader::s_instance;
const char* NetworkDownloader::ImageFormat("PNG");

NetworkDownloader::NetworkDownloader() :
	m_mapSource(MapQuestOpen),
	m_mapType(Hybrid),
	m_showPointsInMap(true),
	m_distanceStrategy(EarthRadiusInMeters)
{
	QSettings settings;
	settings.beginGroup("MapQuestOpenSettings");
	m_mapQuestOpenKey = settings.value("AppKey", "Fmjtd%7Cluur2du8nl%2C2l%3Do5-9arx0r").toString();
	m_showPointsInMap = settings.value("AddPOIs", true).toBool();
	settings.endGroup();
}

NetworkDownloader::~NetworkDownloader()
{
	QSettings settings;
	settings.beginGroup("MapQuestOpenSettings");
	settings.setValue("AppKey", m_mapQuestOpenKey);
	settings.setValue("AddPOIs", m_showPointsInMap);
	settings.endGroup();
}

NetworkDownloader& NetworkDownloader::Instance() {

	return s_instance;
}

void NetworkDownloader::SetMapSource(MapSource source) {

	m_mapSource = source;
}

void NetworkDownloader::SetMapType(MapType type) {

	m_mapType = type;
}

void NetworkDownloader::SetShowPointsInMap(bool show) {

	m_showPointsInMap = show;
}

NetworkDownloader::MapSource NetworkDownloader::GetMapSource() {

	return m_mapSource;
}

NetworkDownloader::MapType NetworkDownloader::GetMapType() {

	return m_mapType;
}

bool NetworkDownloader::GetShowPointsInMap() {

	return m_showPointsInMap;
}

GeographicBoundingBox NetworkDownloader::DownloadMap(const std::vector<GeographicPoint>& points, const std::string& filename, const QSize& imageSize) {
	
	GeographicBoundingBox pointsBoundingBox(points);
	unsigned int zoomLevel = GetZoomLevel(pointsBoundingBox, imageSize);
	
	QString imageUrl;

	if (m_mapSource == MapQuestOpen) {
		imageUrl = GenerateMapQuestOpenString(pointsBoundingBox.GetCenter(), imageSize, zoomLevel, points);
	}
	else {
		//eventually will implement download from Google Maps
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

	if (!image.save(QString::fromStdString(filename), ImageFormat)) {
		throw DownloadException("Failed to save image");
	}

	double lonRadiusInDegrees = std::abs(((MetersPerPixel * std::cos(pointsBoundingBox.GetCenter().get<1>()) / std::pow(2.0, zoomLevel)) * (imageSize.width() / 2.0)) / MetersPerDegreeLongitude);
	double latRadiusInDegrees = std::abs(lonRadiusInDegrees * (imageSize.height() / static_cast<double>(imageSize.width())));

	return GeographicBoundingBox(pointsBoundingBox.GetCenter(), latRadiusInDegrees, lonRadiusInDegrees);
}

unsigned int NetworkDownloader::GetZoomLevel(const GeographicBoundingBox& boundingBox, const QSize& imageSize) {

	double distanceInMeters = std::abs(boost::geometry::distance(boundingBox.GetWestCenter(), boundingBox.GetEastCenter(), m_distanceStrategy));

    double zoomLevel = std::log((MetersPerPixel * std::cos(boundingBox.GetCenter().get<1>() * DegToRad)) / (distanceInMeters / imageSize.width())) / std::log(2.0);

	//Zoom level can never go above 18
	return std::min(static_cast<unsigned int>(zoomLevel), MaxZoomLevel);
}

QString NetworkDownloader::GenerateMapQuestOpenString(const GeographicPoint& center, const QSize& imageSize, unsigned int zoomLevel, const std::vector<GeographicPoint>& points) {

	QString url = "http://open.mapquestapi.com/staticmap/v4/getmap?key=" + m_mapQuestOpenKey;
	url += "&center=" + QString::number(center.get<1>()) + "," + QString::number(center.get<0>());
	url += "&zoom=" + QString::number(zoomLevel) + "&size=" + QString::number(imageSize.width()) + "," + QString::number(imageSize.height());
	url += "&type=";
	if (m_mapType == Map) {
		url += "map";
	}
	else if (m_mapType == Satellite) {
		url += "sat";
	}
	else {
		url += "hyb";
	}
	url += "&imagetype=png";
    //url += "&declutter=true";
	url += "&pois=";

	if ((m_showPointsInMap) && (!points.empty())) {
		for (int i = 0; i < points.size(); ++i) {
			url += "red-circle," + QString::number(points[i].get<1>()) + "," + QString::number(points[i].get<0>()) + ",0,0" + QChar('|');
		}
	}

	return url;
}