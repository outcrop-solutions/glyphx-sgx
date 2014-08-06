#ifndef DOWNLOADEDMAP_H
#define DOWNLOADEDMAP_H

#include <QtCore/QObject>
#include "geographicboundingbox.h"
#include <vector>
#include "networkdownloader.h"
#include "mapdownloading_global.h"
#include "downloadedmapproperties.h"

class MAPDOWNLOADING_EXPORT DownloadedMap : public QObject
{
    Q_OBJECT

public:
    DownloadedMap(const std::vector<GeographicPoint>& points, const std::string& filename, const SynGlyphX::DownloadedMapProperties* const properties, QObject *parent = NULL);
    ~DownloadedMap();

    const GeographicBoundingBox& GetImageBoundingBox();
    const GeographicBoundingBox& GetPointsBoundingBox();

	SynGlyphX::DownloadedMapProperties::MapSource GetMapSource() const;
	SynGlyphX::DownloadedMapProperties::MapType GetMapType() const;
	bool GetShowPointsInMap() const;

private:
	std::string m_filename;
    GeographicBoundingBox m_imageBoundingBox;
    GeographicBoundingBox m_pointsBoundingBox;

	SynGlyphX::DownloadedMapProperties::MapSource m_mapSource;
	SynGlyphX::DownloadedMapProperties::MapType m_mapType;
	bool m_showPointsInMap;
};

#endif // DOWNLOADEDMAP_H
