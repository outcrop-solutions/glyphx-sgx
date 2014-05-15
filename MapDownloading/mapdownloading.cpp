#include "mapdownloading.h"
#include <cmath>

/*
1. Calculate Center
2. Find nearest but larger zoom level
3. Find new bounding box based on zoom level

Latitude: 1 deg = 110.54 km
Longitude: 1 deg = 111.320*cos(latitude) km

*/

const unsigned int MaxZoomLevel = 18;
const double MetersPerPixel = 156543.034;
const double EarthRadiusInMeters = 63727982.0;
const double MetersPerDegreeLongitude = 111319.892;

MapDownloading::MapDownloading() :
    m_distanceStrategy(EarthRadiusInMeters)
{

}

MapDownloading::~MapDownloading()
{

}

unsigned int MapDownloading::GetZoomLevel(const GeographicBoundingBox& boundingBox, const QSize& imageSize) {

    double distanceInMeters = std::abs(boost::geometry::distance(boundingBox.GetWestCenter(), boundingBox.GetEastCenter(), m_distanceStrategy));

    unsigned int zoomLevel = static_cast<unsigned int>(std::log(imageSize.width() * MetersPerPixel * std::cos(boundingBox.GetCenter().get<1>()) / distanceInMeters) / std::log(2.0));

    return std::min(zoomLevel, MaxZoomLevel);
}

void MapDownloading::CalculateImageBoundingBox(const GeographicPoint& center, unsigned int zoomLevel, const QSize& imageSize) {

    double lonRadiusInDegrees = ((MetersPerPixel * std::cos(center.get<1>()) / std::pow(2.0, zoomLevel)) * imageSize.width() / 2.0) / MetersPerDegreeLongitude;
    double latRadiusInDegrees = lonRadiusInDegrees * (imageSize.height() / static_cast<double>(imageSize.width()));


}