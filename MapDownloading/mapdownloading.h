#ifndef MAPDOWNLOADING_H
#define MAPDOWNLOADING_H

#include "mapdownloading_global.h"
#include <boost/geometry/geometry.hpp>

typedef boost::geometry::model::point<double, 2, boost::geometry::cs::geographic<boost::geometry::degree>> GeographicPoint;
typedef boost::geometry::strategy::distance::haversine<double> DistanceStrategy;

class MAPDOWNLOADING_EXPORT MapDownloading
{
public:
    MapDownloading();
    ~MapDownloading();

private:
    unsigned int GetZoomLevel(const GeographicPoint& sw, const GeographicPoint& ne, unsigned int imageWidth);

    DistanceStrategy m_distanceStrategy;
};

#endif // MAPDOWNLOADING_H
