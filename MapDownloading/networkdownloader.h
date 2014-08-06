#ifndef NETWORKDOWNLOADER_H
#define NETWORKDOWNLOADER_H

#include "mapdownloading_global.h"
#include <QtCore/QObject>
#include <string>
#include <vector>
#include "geographicboundingbox.h"
#include <QtNetwork/QNetworkAccessManager>
#include "downloadedmapproperties.h"

typedef boost::geometry::strategy::distance::haversine<double> DistanceStrategy;

class MAPDOWNLOADING_EXPORT NetworkDownloader
{

public:
	static const char* ImageFormat;

	NetworkDownloader();
	~NetworkDownloader();

	static NetworkDownloader& Instance();
	
	GeographicBoundingBox DownloadMap(const std::vector<GeographicPoint>& points, const std::string& filename, const SynGlyphX::DownloadedMapProperties* const properties);

	void SetShowPointsInMap(bool show);
	bool GetShowPointsInMap();

    void SetMapQuestOpenKey(const QString& key);
    const QString& GetMapQuestOpenKey() const;

    //void SetGoogleMapsKey(const QString& key);
    //const QString& GetGoogleMapsKey() const;

private:
	unsigned int GetZoomLevel(const GeographicBoundingBox& boundingBox, const SynGlyphX::DownloadedMapProperties::Size& imageSize);
	QString GenerateMapQuestOpenString(const GeographicPoint& center, unsigned int zoomLevel, const SynGlyphX::DownloadedMapProperties* const properties, const std::vector<GeographicPoint>& points);
    void ReadSettings();
    void WriteSettings();

	static NetworkDownloader s_instance;

	bool m_showPointsInMap;

	DistanceStrategy m_distanceStrategy;

	QString m_mapQuestOpenKey;
    //QString m_googlemapsOpenKey;
};

#endif //NETWORKDOWNLOADER_H