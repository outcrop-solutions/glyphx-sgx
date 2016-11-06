#include "geographicboundingbox.h"
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <boost/filesystem.hpp>

GeographicBoundingBox::GeographicBoundingBox() :
	m_center(0.0, 0.0),
	m_swCorner(0.0, 0.0),
	m_neCorner(0.0, 0.0),
	m_westCenter(0.0, 0.0),
	m_eastCenter(0.0, 0.0),
	m_northCenter(0.0, 0.0),
	m_southCenter(0.0, 0.0) {

}

GeographicBoundingBox::GeographicBoundingBox(const GeographicPoint& swCorner, const GeographicPoint& neCorner) :
    m_swCorner(swCorner),
    m_neCorner(neCorner),
    m_center(((swCorner.get<0>() + neCorner.get<0>()) / 2.0, (swCorner.get<1>() + neCorner.get<1>()) / 2.0)),
    m_westCenter(swCorner.get<0>(), m_center.get<1>()),
    m_eastCenter(neCorner.get<0>(), m_center.get<1>()),
    m_northCenter(m_center.get<0>(), neCorner.get<1>()),
    m_southCenter(m_center.get<0>(), swCorner.get<1>())
{
}

GeographicBoundingBox::GeographicBoundingBox(const GeographicPoint& center, double latRadius, double lonRadius) :
    m_center(center),
    m_swCorner(center.get<0>() - lonRadius, center.get<1>() - latRadius),
    m_neCorner(center.get<0>() + lonRadius, center.get<1>() + latRadius),
    m_westCenter(center.get<0>() - lonRadius, center.get<1>()),
    m_eastCenter(center.get<0>() + lonRadius, center.get<1>()),
    m_northCenter(center.get<0>(), center.get<1>() + latRadius),
    m_southCenter(center.get<0>(), center.get<1>() - latRadius) {
    

}

GeographicBoundingBox::GeographicBoundingBox(const std::vector<GeographicPoint>& points) {

	m_swCorner = points[0];
	m_neCorner = points[0];

	for (unsigned int i = 1; i < points.size(); ++i) {
		
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

    m_northCenter.set<0>(m_center.get<0>());
    m_northCenter.set<1>(m_neCorner.get<1>());

    m_southCenter.set<0>(m_center.get<0>());
    m_southCenter.set<1>(m_swCorner.get<1>());
}

GeographicBoundingBox::GeographicBoundingBox(const GeographicBoundingBox& boundingBox) :
	m_center(boundingBox.m_center),
	m_swCorner(boundingBox.m_swCorner),
	m_neCorner(boundingBox.m_neCorner),
	m_westCenter(boundingBox.m_westCenter),
	m_eastCenter(boundingBox.m_eastCenter),
	m_northCenter(boundingBox.m_northCenter),
	m_southCenter(boundingBox.m_southCenter) {

}

GeographicBoundingBox::~GeographicBoundingBox()
{
}

GeographicBoundingBox& GeographicBoundingBox::operator=(const GeographicBoundingBox& boundingBox) {

	m_center = boundingBox.m_center;
	m_swCorner = boundingBox.m_swCorner;
	m_neCorner = boundingBox.m_neCorner;
	m_westCenter = boundingBox.m_westCenter;
	m_eastCenter = boundingBox.m_eastCenter;
	m_northCenter = boundingBox.m_northCenter;
	m_southCenter = boundingBox.m_southCenter;

	return *this;
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

const GeographicPoint& GeographicBoundingBox::GetNorthCenter() const {

    return m_northCenter;
}

const GeographicPoint& GeographicBoundingBox::GetSouthCenter() const {

    return m_southCenter;
}

double GeographicBoundingBox::GetWidth() const {

	return (m_eastCenter.get<0>() - m_westCenter.get<0>());
}

double GeographicBoundingBox::GetHeight() const {

	return (m_northCenter.get<1>() - m_southCenter.get<1>());
}

bool GeographicBoundingBox::IsValid() const {

	return ((GetWidth() > 0.0) && (GetHeight() > 0.0));
}

std::string GeographicBoundingBox::ToString() const {

    //Print bounding box in format of <top_left_lon> <top_left_lat> <bottom_right_lon> <bottom_right_lat> to follow gdal_translate order of values 

	std::stringstream stream;
	stream.precision(8);
    stream << m_swCorner.get<0>() << " " << m_neCorner.get<1>() << " " << m_neCorner.get<0>() << " " << m_swCorner.get<1>();

	return stream.str();
}

void GeographicBoundingBox::WriteToKMLFile(const std::string& filename) const {

	//This should probably use an KML library to write out the file eventually.  Right now just using this code for development speed

    const std::string pointNames[4] = { "NW Corner", "SW Corner", "SE Corner", "NE Corner" };
    GeographicPoint ccwPoints[4];

    boost::filesystem::path kmlpath(filename);

    std::ofstream kmlfile;
    kmlfile.open(filename, std::ios_base::out | std::ios_base::trunc);

    if (!kmlfile.is_open()) {
        throw std::runtime_error("Failed to write bounding box KML file");
    }

    kmlfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    kmlfile << "<kml xmlns = \"http://www.opengis.net/kml/2.2\" xmlns:gx = \"http://www.google.com/kml/ext/2.2\" xmlns:kml = \"http://www.opengis.net/kml/2.2\" xmlns:atom = \"http://www.w3.org/2005/Atom\">" << std::endl;
    kmlfile << "<Document>" << std::endl;
    kmlfile << "\t<name>" << kmlpath.filename() << "</name>" << std::endl;
    kmlfile << "\t<Style id = \"s_ylw-pushpin\">" << std::endl;
    kmlfile << "\t\t<IconStyle>" << std::endl;
    kmlfile << "\t\t\t<scale>1.1</scale>" << std::endl;
    kmlfile << "\t\t\t<Icon>" << std::endl;
    kmlfile << "\t\t\t\t<href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>" << std::endl;
    kmlfile << "\t\t\t</Icon>" << std::endl;
    kmlfile << "\t\t\t<hotSpot x = \"20\" y = \"2\" xunits = \"pixels\" yunits = \"pixels\"/>" << std::endl;
    kmlfile << "\t\t</IconStyle>" << std::endl;
    kmlfile << "\t</Style>" << std::endl;
    kmlfile << "\t<Style id = \"s_ylw-pushpin_hl\">" << std::endl;
    kmlfile << "\t\t<IconStyle>" << std::endl;
    kmlfile << "\t\t\t<scale>1.3</scale>" << std::endl;
    kmlfile << "\t\t\t<Icon>" << std::endl;
    kmlfile << "\t\t\t\t<href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>" << std::endl;
    kmlfile << "\t\t\t</Icon>" << std::endl;
    kmlfile << "\t\t\t<hotSpot x = \"20\" y = \"2\" xunits = \"pixels\" yunits = \"pixels\"/>" << std::endl;
    kmlfile << "\t\t</IconStyle>" << std::endl;
    kmlfile << "\t</Style>" << std::endl;
    kmlfile << "\t<StyleMap id = \"m_ylw-pushpin\">" << std::endl;
    kmlfile << "\t\t<Pair>" << std::endl;
    kmlfile << "\t\t\t<key>normal</key>" << std::endl;
    kmlfile << "\t\t\t<styleUrl>#s_ylw-pushpin</styleUrl>" << std::endl;
    kmlfile << "\t\t</Pair>" << std::endl;
    kmlfile << "\t\t<Pair>" << std::endl;
    kmlfile << "\t\t\t<key>highlight</key>" << std::endl;
    kmlfile << "\t\t\t<styleUrl>#s_ylw-pushpin_hl</styleUrl>" << std::endl;
    kmlfile << "\t\t</Pair>" << std::endl;
    kmlfile << "\t</StyleMap>" << std::endl;
    kmlfile << "\t<Folder>" << std::endl;
    kmlfile << "\t\t<name>" << kmlpath.stem() << " Bounding Box</name>" << std::endl;
    kmlfile << "\t\t<open>1</open >" << std::endl;

    for (int i = 0; i < 4; ++i) {

        double lon, lat;
        if (i < 2) {
            lon = m_swCorner.get<0>();
        }
        else {
            lon = m_neCorner.get<0>();
        }

        if ((i == 0) || (i == 3)) {
            lat = m_neCorner.get<1>();
        }
        else {
            lat = m_swCorner.get<1>();
        }

        kmlfile << "\t\t<Placemark>" << std::endl;
        kmlfile << "\t\t\t<name>" << pointNames[i] << "</name>" << std::endl;
        kmlfile << "\t\t\t<LookAt>" << std::endl;
        kmlfile << "\t\t\t\t<longitude>" << lon << "</longitude>" << std::endl;
        kmlfile << "\t\t\t\t<latitude>" << lat << "</latitude>" << std::endl;
        kmlfile << "\t\t\t\t<altitude>0</altitude>" << std::endl;
        kmlfile << "\t\t\t\t<heading>0</heading>" << std::endl;
        kmlfile << "\t\t\t\t<tilt>0</tilt>" << std::endl;
        kmlfile << "\t\t\t\t<range>100000</range>" << std::endl;
        kmlfile << "\t\t\t\t<gx:altitudeMode>relativeToSeaFloor</gx:altitudeMode>" << std::endl;
        kmlfile << "\t\t\t</LookAt>" << std::endl;
        kmlfile << "\t\t\t<styleUrl>#m_ylw-pushpin</styleUrl>" << std::endl;
        kmlfile << "\t\t\t<Point>" << std::endl;
        kmlfile << "\t\t\t\t<gx:drawOrder>1</gx:drawOrder>" << std::endl;
        kmlfile << "\t\t\t\t<coordinates>" << lon << "," << lat << ", 0</coordinates>" << std::endl;
        kmlfile << "\t\t\t</Point>" << std::endl;
        kmlfile << "\t\t</Placemark>" << std::endl;
    }

    kmlfile << "\t</Folder>" << std::endl;
    kmlfile << "</Document>" << std::endl;
    kmlfile << "</kml>" << std::endl;

    kmlfile.close();
}
