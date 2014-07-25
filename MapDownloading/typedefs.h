#ifndef MAPDOWNLOADING_TYPEDEFS_H
#define MAPDOWNLOADING_TYPEDEFS_H

#include <boost/geometry/geometry.hpp>

typedef boost::geometry::model::point<double, 2, boost::geometry::cs::geographic<boost::geometry::degree>> GeographicPoint;
typedef boost::geometry::strategy::distance::haversine<double> DistanceStrategy;

#endif //MAPDOWNLOADING_TYPEDEFS_H