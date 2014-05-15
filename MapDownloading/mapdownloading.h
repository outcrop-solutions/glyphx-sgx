#ifndef MAPDOWNLOADING_H
#define MAPDOWNLOADING_H

#include "mapdownloading_global.h"
#include "typedefs.h"
#include <QtCore/QSize>
#include "geographicboundingbox.h"

class MAPDOWNLOADING_EXPORT MapDownloading
{
public:
    MapDownloading();
    ~MapDownloading();


private:
    unsigned int GetZoomLevel(const GeographicBoundingBox& boundingBox, const QSize& imageSize);
    void CalculateImageBoundingBox(const GeographicPoint& center, unsigned int zoomLevel, const QSize& imageSize);

    DistanceStrategy m_distanceStrategy;
    //GeographicBoundingBox m_outputBoundingBox;
};

#endif // MAPDOWNLOADING_H
