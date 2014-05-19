#ifndef DOWNLOADEDMAP_H
#define DOWNLOADEDMAP_H

#include <QtCore/QObject>
#include "geographicboundingbox.h"
#include <vector>
#include "networkdownloader.h"
#include "mapdownloading_global.h"

class MAPDOWNLOADING_EXPORT DownloadedMap : public QObject
{
    Q_OBJECT

public:
	DownloadedMap(const std::vector<GeographicPoint>& points, const std::string& filename, const QSize& imageSize, QObject *parent = NULL);
    ~DownloadedMap();

    const GeographicBoundingBox& GetGeographicBoundingBox();

	NetworkDownloader::MapSource GetMapSource();
	NetworkDownloader::MapType GetMapType();
	bool GetShowPointsInMap();

private:
	void CreateGeoreferencedImage(const std::string& inputImage);

	std::string m_filename;
    GeographicBoundingBox m_geographicBoundingBox;

	NetworkDownloader::MapSource m_mapSource;
	NetworkDownloader::MapType m_mapType;
	bool m_showPointsInMap;
};

#endif // DOWNLOADEDMAP_H
