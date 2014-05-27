#ifndef NETWORKDOWNLOADER_H
#define NETWORKDOWNLOADER_H

#include "mapdownloading_global.h"
#include <QtCore/QObject>
#include <string>
#include <vector>
#include "geographicboundingbox.h"
#include <QtNetwork/QNetworkAccessManager>

typedef boost::geometry::strategy::distance::haversine<double> DistanceStrategy;

class MAPDOWNLOADING_EXPORT NetworkDownloader
{

public:
	static const char* ImageFormat;

	enum MapSource {
		MapQuestOpen, //MapQuestOpen gets data from OpenStreetMap
		GoogleMaps
	};

	enum MapType {
		Map,
		Satellite,
		Hybrid
	};

	NetworkDownloader();
	~NetworkDownloader();

	static NetworkDownloader& Instance();
	
    GeographicBoundingBox DownloadMap(const std::vector<GeographicPoint>& points, const std::string& filename, const QSize& imageSize, MapSource source, MapType mapType);

	void SetShowPointsInMap(bool show);
	bool GetShowPointsInMap();

    void SetMapQuestOpenKey(const QString& key);
    const QString& GetMapQuestOpenKey() const;

    //void SetGoogleMapsKey(const QString& key);
    //const QString& GetGoogleMapsKey() const;

private:
	unsigned int GetZoomLevel(const GeographicBoundingBox& boundingBox, const QSize& imageSize);
    QString GenerateMapQuestOpenString(const GeographicPoint& center, const QSize& imageSize, unsigned int zoomLevel, MapType mapType, const std::vector<GeographicPoint>& points);

	static NetworkDownloader s_instance;

	bool m_showPointsInMap;

	DistanceStrategy m_distanceStrategy;

	QString m_mapQuestOpenKey;
    //QString m_googlemapsOpenKey;
};

#endif //NETWORKDOWNLOADER_H