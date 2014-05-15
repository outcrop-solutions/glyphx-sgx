#include "GeographicBoundingBox.h"

GeographicBoundingBox::GeographicBoundingBox(const GeographicPoint& swCorner, const GeographicPoint& neCorner) :
    m_swCorner(swCorner),
    m_neCorner(neCorner),
    m_center(((swCorner.get<0>() + neCorner.get<0>()) / 2.0, (swCorner.get<1>() + neCorner.get<1>()) / 2.0)),
    m_westCenter(swCorner.get<0>(), m_center.get<1>()),
    m_eastCenter(neCorner.get<0>(), m_center.get<1>())
{
}

GeographicBoundingBox::GeographicBoundingBox(const GeographicPoint& center, double latRadius, double lonRadius) :
    m_center(center),
    m_swCorner(m_center.get<0>() - lonRadius, m_center.get<1>() - latRadius),
    m_neCorner(m_center.get<0>() + lonRadius, m_center.get<1>() + latRadius),
    m_westCenter(m_center.get<0>() - lonRadius, m_center.get<1>()),
    m_eastCenter(m_center.get<0>() + lonRadius, m_center.get<1>()) {
    

}

GeographicBoundingBox::~GeographicBoundingBox()
{
}

const GeographicPoint& GeographicBoundingBox::GetCenter() const {

    return m_center;
}

const GeographicPoint& GeographicBoundingBox::GetWestCenter() const {

    return m_westCenter;
}

const GeographicPoint& GeographicBoundingBox::GetEastCenter() const {

    return m_eastCenter;
}