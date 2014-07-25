#ifndef GEOGRAPHICBOUNDINGBOX_H
#define GEOGRAPHICBOUNDINGBOX_H

#include "mapdownloading_global.h"
#include "typedefs.h"
#include <vector>
#include <string>

class MAPDOWNLOADING_EXPORT GeographicBoundingBox
{
public:
	GeographicBoundingBox();
    GeographicBoundingBox(const GeographicPoint& swCorner, const GeographicPoint& neCorner);
    GeographicBoundingBox(const GeographicPoint& center, double latRadius, double lonRadius);
	GeographicBoundingBox(const std::vector<GeographicPoint>& points);
    ~GeographicBoundingBox();

	const GeographicPoint& GetSWCorner() const;
	const GeographicPoint& GetNECorner() const;
    const GeographicPoint& GetCenter() const;
    const GeographicPoint& GetWestCenter() const;
    const GeographicPoint& GetEastCenter() const;
    const GeographicPoint& GetNorthCenter() const;
    const GeographicPoint& GetSouthCenter() const;

	std::string ToString() const;

    void WriteToKMLFile(const std::string& filename) const;

private:
    GeographicPoint m_swCorner;
    GeographicPoint m_neCorner;
    GeographicPoint m_center;
    GeographicPoint m_westCenter;
    GeographicPoint m_eastCenter;
    GeographicPoint m_northCenter;
    GeographicPoint m_southCenter;
};

#endif //GEOGRAPHICBOUNDINGBOX_H