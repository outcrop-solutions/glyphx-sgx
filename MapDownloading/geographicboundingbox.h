#ifndef GEOGRAPHICBOUNDINGBOX_H
#define GEOGRAPHICBOUNDINGBOX_H

#include "typedefs.h"

class GeographicBoundingBox
{
public:
    GeographicBoundingBox(const GeographicPoint& swCorner, const GeographicPoint& neCorner);
    GeographicBoundingBox(const GeographicPoint& center, double latRadius, double lonRadius);
    ~GeographicBoundingBox();

    const GeographicPoint& GetCenter() const;
    const GeographicPoint& GetWestCenter() const;
    const GeographicPoint& GetEastCenter() const;

private:
    GeographicPoint m_swCorner;
    GeographicPoint m_neCorner;
    GeographicPoint m_center;
    GeographicPoint m_westCenter;
    GeographicPoint m_eastCenter;
};

#endif //GEOGRAPHICBOUNDINGBOX_H