#ifndef NETWORKDOWNLOADER_H
#define NETWORKDOWNLOADER_H

#include "mapdownloading_global.h"
#include <QtCore/QObject>
#include <string>
#include "typedefs.h"
#include <vector>
#include "geographicboundingbox.h"
#include <QtNetwork/QNetworkAccessManager>

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
	
	GeographicBoundingBox DownloadMap(const std::vector<GeographicPoint>& points, const std::string& filename, const QSize& imageSize);

	void SetMapSource(MapSource source);
	void SetMapType(MapType type);
	void SetShowPointsInMap(bool show);

	MapSource GetMapSource();
	MapType GetMapType();
	bool GetShowPointsInMap();

private:
	unsigned int GetZoomLevel(const GeographicBoundingBox& boundingBox, const QSize& imageSize);
	QString GenerateMapQuestOpenString(const GeographicPoint& center, const QSize& imageSize, unsigned int zoomLevel, const std::vector<GeographicPoint>& points);

	static NetworkDownloader s_instance;

	MapSource m_mapSource;
	MapType m_mapType;
	bool m_showPointsInMap;

	DistanceStrategy m_distanceStrategy;

	QString m_mapQuestOpenKey;
};

#endif //NETWORKDOWNLOADER_H