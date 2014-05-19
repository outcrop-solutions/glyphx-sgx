#include "GeographicBoundingBox.h"
#include <sstream>

GeographicBoundingBox::GeographicBoundingBox() {

}

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
    m_swCorner(center.get<0>() - lonRadius, center.get<1>() - latRadius),
    m_neCorner(center.get<0>() + lonRadius, center.get<1>() + latRadius),
    m_westCenter(center.get<0>() - lonRadius, center.get<1>()),
    m_eastCenter(center.get<0>() + lonRadius, center.get<1>()) {
    

}

GeographicBoundingBox::GeographicBoundingBox(const std::vector<GeographicPoint>& points) {

	m_swCorner = points[0];
	m_neCorner = points[0];

	for (int i = 1; i < points.size(); ++i) {
		
		if (points[i].get<0>() < m_swCorner.get<0>()) {
			m_swCorner.set<0>(points[i].get<0>());
		}
		if (points[i].get<1>() < m_swCorner.get<1>()) {
			m_swCorner.set<1>(points[i].get<1>());
		}

		if (points[i].get<0>() > m_neCorner.get<0>()) {
			m_neCorner.set<0>(points[i].get<0>());
		}
		if (points[i].get<1>() > m_neCorner.get<1>()) {
			m_neCorner.set<1>(points[i].get<1>());
		}
	}

	m_center.set<0>((m_swCorner.get<0>() + m_neCorner.get<0>()) / 2.0);
	m_center.set<1>((m_swCorner.get<1>() + m_neCorner.get<1>()) / 2.0);

	m_westCenter.set<0>(m_swCorner.get<0>());
	m_westCenter.set<1>(m_center.get<1>());

	m_eastCenter.set<0>(m_neCorner.get<0>());
	m_eastCenter.set<1>(m_center.get<1>());
}

GeographicBoundingBox::~GeographicBoundingBox()
{
}

const GeographicPoint& GeographicBoundingBox::GetSWCorner() const {

	return m_swCorner;
}

const GeographicPoint& GeographicBoundingBox::GetNECorner() const {

	return m_neCorner;
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

std::string GeographicBoundingBox::ToString() const {

	std::stringstream stream;
	stream.precision(8);
	stream << m_swCorner.get<1>() << " " << m_swCorner.get<0>() << " " << m_neCorner.get<1>() << " " << m_neCorner.get<0>();

	return stream.str();
}

void GeographicBoundingBox::WriteToKMLFile(const std::string& path) const {

	//This should probably use an KML library to write out the file eventually
}