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
const double MetersPerPixelAtZoom0 = 156543.034;
const double EarthRadiusInMeters = 6372798.2;
const double MetersPerDegreeLongitude = 111319.892;
const double MetersPerDegreeLatitude = 110540.0;
const double DegToRad = 3.14159 / 180.0;

NetworkDownloader NetworkDownloader::s_instance;
const char* NetworkDownloader::ImageFormat("PNG");

NetworkDownloader::NetworkDownloader() :
	m_showPointsInMap(false),
	m_distanceStrategy(EarthRadiusInMeters)
{
    ReadSettings();
}

NetworkDownloader::~NetworkDownloader()
{
	
}

void NetworkDownloader::ReadSettings() {

    QSettings settings("SynGlyphX", "MapDownloading");
    settings.beginGroup("MapQuestOpenSettings");

    m_mapQuestOpenKey = settings.value("AppKey", "").toString();

    m_showPointsInMap = settings.value("AddPOIs", false).toBool();
    settings.endGroup();
}

void NetworkDownloader::WriteSettings() {

    QSettings settings("SynGlyphX", "MapDownloading");
    settings.beginGroup("MapQuestOpenSettings");
    settings.setValue("AppKey", m_mapQuestOpenKey);
    settings.setValue("AddPOIs", m_showPointsInMap);
    settings.endGroup();
}

NetworkDownloader& NetworkDownloader::Instance() {

	return s_instance;
}

void NetworkDownloader::SetShowPointsInMap(bool show) {

	m_showPointsInMap = show;
    WriteSettings();
}

bool NetworkDownloader::GetShowPointsInMap() {

	return m_showPointsInMap;
}

GeographicBoundingBox NetworkDownloader::DownloadMap(const std::vector<GeographicPoint>& points, const std::string& filename, const SynGlyphX::DownloadedMapProperties* const properties) {
	
	GeographicBoundingBox pointsBoundingBox(points);
	unsigned int zoomLevel = GetZoomLevel(pointsBoundingBox, properties->GetSize());
	
	QString imageUrl;

    if (properties->GetSource() == SynGlyphX::DownloadedMapProperties::MapQuestOpen) {
		imageUrl = GenerateMapQuestOpenString(pointsBoundingBox.GetCenter(), zoomLevel, properties, points);
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

    double cosineAtCenter = std::cos(pointsBoundingBox.GetCenter().get<1>() * DegToRad);
    double metersPerPixelAtCurrentZoom = (MetersPerPixelAtZoom0 * cosineAtCenter) / std::pow(2.0, static_cast<double>(zoomLevel));

    //double lonRadiusInDegrees = std::abs(((MetersPerPixelAtZoom0 * std::cos(pointsBoundingBox.GetCenter().get<1>() * DegToRad) / std::pow(2.0, zoomLevel)) * (imageSize.width() / 2.0)) / MetersPerDegreeLongitude);
	double lonRadiusInDegrees = (metersPerPixelAtCurrentZoom  * (properties->GetSize()[0] / 2.0)) / (MetersPerDegreeLongitude  * cosineAtCenter);
	//double latRadiusInDegrees = lonRadiusInDegrees * (imageSize.height() / static_cast<double>(imageSize.width()));
	double latRadiusInDegrees = (metersPerPixelAtCurrentZoom * (properties->GetSize()[1] / 2.0)) / MetersPerDegreeLatitude;

	return GeographicBoundingBox(pointsBoundingBox.GetCenter(), latRadiusInDegrees, lonRadiusInDegrees);
}

unsigned int NetworkDownloader::GetZoomLevel(const GeographicBoundingBox& boundingBox, const SynGlyphX::DownloadedMapProperties::Size& imageSize) {

	double hDistanceInMeters = std::abs(boost::geometry::distance(boundingBox.GetWestCenter(), boundingBox.GetEastCenter(), m_distanceStrategy));
    double vDistanceInMeters = std::abs(boost::geometry::distance(boundingBox.GetNorthCenter(), boundingBox.GetSouthCenter(), m_distanceStrategy));

    double hZoomLevel = std::log((MetersPerPixelAtZoom0 * std::cos(boundingBox.GetCenter().get<1>() * DegToRad)) / (hDistanceInMeters / imageSize[0])) / std::log(2.0);
    double vZoomLevel = std::log((MetersPerPixelAtZoom0 * std::cos(boundingBox.GetCenter().get<1>() * DegToRad)) / (vDistanceInMeters / imageSize[1])) / std::log(2.0);

	//Zoom level can never go above 18 on MapQuest Open
	return std::min(static_cast<unsigned int>(std::min(hZoomLevel, vZoomLevel)), MaxZoomLevel);
}

QString NetworkDownloader::GenerateMapQuestOpenString(const GeographicPoint& center, unsigned int zoomLevel, const SynGlyphX::DownloadedMapProperties* const properties, const std::vector<GeographicPoint>& points) {

	QString url = "http://open.mapquestapi.com/staticmap/v4/getmap?key=" + m_mapQuestOpenKey;
	url += "&center=" + QString::number(center.get<1>()) + "," + QString::number(center.get<0>());
	url += "&zoom=" + QString::number(zoomLevel) + "&size=" + QString::number(properties->GetSize()[0]) + "," + QString::number(properties->GetSize()[1]);
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
    //url += "&declutter=true";
	url += "&pois=";

	if ((m_showPointsInMap) && (!points.empty())) {
		for (int i = 0; i < points.size(); ++i) {
			url += "red-circle," + QString::number(points[i].get<1>()) + "," + QString::number(points[i].get<0>()) + ",0,0" + QChar('|');
		}
	}

	return url;
}

void NetworkDownloader::SetMapQuestOpenKey(const QString& key) {

    m_mapQuestOpenKey = key;
    WriteSettings();
}

const QString& NetworkDownloader::GetMapQuestOpenKey() const {

    return m_mapQuestOpenKey;
}