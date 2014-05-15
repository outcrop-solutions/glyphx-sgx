#include "mapdownloading.h"
#include <cmath>

/*
1. Calculate Center
2. Find nearest but larger zoom level
3. Find new bounding box based on zoom level
*/

const unsigned int MaxZoomLevel = 18;
const double MetersPerPixel = 156543.034;
const double EarthRadiusInMeters = 63727982.0;
const double MetersPerDegree = 111319.892;

MapDownloading::MapDownloading() :
    m_distanceStrategy(EarthRadiusInMeters)
{

}

MapDownloading::~MapDownloading()
{

}

unsigned int MapDownloading::GetZoomLevel(const GeographicPoint& sw, const GeographicPoint& ne, unsigned int imageWidth) {

    GeographicPoint center((sw.get<0>() + ne.get<0>()) / 2.0, (sw.get<1>() + ne.get<1>()) / 2.0);

    GeographicPoint leftCenter(sw.get<0>(), center.get<1>());
    GeographicPoint rightCenter(ne.get<0>(), center.get<1>());

    double distanceInMeters = std::abs(boost::geometry::distance(leftCenter, rightCenter, m_distanceStrategy));

    unsigned int zoomLevel = static_cast<unsigned int>(std::log(imageWidth * MetersPerPixel * std::cos(center.get<1>()) / distanceInMeters) / std::log(2.0));

    return std::min(zoomLevel, MaxZoomLevel);
}