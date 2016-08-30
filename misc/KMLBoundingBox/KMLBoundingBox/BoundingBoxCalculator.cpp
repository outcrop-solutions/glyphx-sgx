#include "BoundingBoxCalculator.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>

typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;

BoundingBoxCalculator::BoundingBoxCalculator() :
    m_spaceSeparator(" \t\n"),
    m_commaSeparator(",")
{
    Reset();
}


BoundingBoxCalculator::~BoundingBoxCalculator()
{
}

void BoundingBoxCalculator::Reset() {

	m_minLat = 90.0;
	m_maxLat = -90.0;
	m_minLon = 180.0;
	m_maxLon = -180.0;

    m_numCoordinatesProcessed = 0;
}

//Function that does all the work. It takes a filename.  If the file isn't in the current working dir, it should be a full path
std::string BoundingBoxCalculator::ReadAndGetBoundingBox(const std::string& filename) {

	Reset();

	boost::property_tree::ptree propertyTree;

	try {
        //Use the boost property_tree XML parser to read the KML file
		boost::property_tree::xml_parser::read_xml(filename, propertyTree);
        boost::property_tree::ptree::const_assoc_iterator kml = propertyTree.find("kml");
        if (kml != propertyTree.not_found()) {
            ProcessFeatureChildren(kml->second);
        }

#ifdef _DEBUG
        std::cout << "Number of coordinates tags processed: " << m_numCoordinatesProcessed << std::endl;
#endif

        if (m_numCoordinatesProcessed > 0) {
            return boost::lexical_cast<std::string>(m_minLat) + ", " + boost::lexical_cast<std::string>(m_minLon) + " " +
                boost::lexical_cast<std::string>(m_maxLat) + ", " + boost::lexical_cast<std::string>(m_maxLon);
        }
        else {
            return "No Coordinates Processed";
        }
	}
	//All exceptions thrown should inherit from std::exception
	catch (const std::exception& e) {
		//rethrow so that main handles the exception.  may wish to do something here at some point
		throw;
	} 
}

void BoundingBoxCalculator::ProcessFeatureChildren(const boost::property_tree::ptree& propertyElement) {
    boost::property_tree::ptree::const_iterator iT = propertyElement.begin();
    for (; iT != propertyElement.end(); ++iT) {
        //Documents and Folders can have any number of feature elements as children so use ProcessFeatureChildren on its feature children
        if ((iT->first == "Document") || (iT->first == "Folder")) {
            ProcessFeatureChildren(iT->second);
        }
        //When a Placemark is reached, we know it has geometry for children, so process the geometry children
        else if (iT->first == "Placemark"){
            ProcessGeometryChildren(iT->second);
        }
    }
}

void BoundingBoxCalculator::ProcessGeometryChildren(const boost::property_tree::ptree& propertyElement) {
    boost::property_tree::ptree::const_iterator iT = propertyElement.begin();
    for (; iT != propertyElement.end(); ++iT) {
        if ((iT->first == "Point") || (iT->first == "LineString") || (iT->first == "LinearRing")) {
            ProcessCoordinateParent(iT->second);
        }
        else if (iT->first == "Polygon"){
            boost::property_tree::ptree::const_assoc_iterator ring = iT->second.find("outerBoundaryIs");
            if (ring != iT->second.not_found()) {
                ProcessGeometryChildren(ring->second);
            }
        }
        else if (iT->first == "MultiGeometry"){
            ProcessGeometryChildren(iT->second);
        }
    }
}

void BoundingBoxCalculator::ProcessCoordinateParent(const boost::property_tree::ptree& propertyElement) {

    boost::property_tree::ptree::const_assoc_iterator coords = propertyElement.find("coordinates");
    if (coords != propertyElement.not_found()) {
        ProcessCoordinateData(coords->second.get_value(""));
    }
}

void BoundingBoxCalculator::ProcessCoordinateData(const std::string& coordStr) {

    if (!coordStr.empty()) {
        Tokenizer spaceTok(coordStr, m_spaceSeparator);
        for (Tokenizer::iterator sT = spaceTok.begin(); sT != spaceTok.end(); ++sT) {
            Tokenizer commaTok(*sT, m_commaSeparator);
            Tokenizer::iterator cT = commaTok.begin();
            
            double newLon = boost::lexical_cast<double>(*cT);
            ++cT;
            double newLat = boost::lexical_cast<double>(*cT);

            if (newLat > m_maxLat) {
                m_maxLat = newLat;
            }
            if (newLat < m_minLat) {
                m_minLat = newLat;
            }

            if (newLon > m_maxLon) {
                m_maxLon = newLon;
            }
            if (newLon < m_minLon) {
                m_minLon = newLon;
            }
        }
        ++m_numCoordinatesProcessed;
    }
}