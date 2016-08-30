#ifndef BOUNDINGBOXCALCULATOR_H_
#define BOUNDINGBOXCALCULATOR_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/tokenizer.hpp>

class BoundingBoxCalculator
{
public:
	BoundingBoxCalculator();
	~BoundingBoxCalculator();

	std::string ReadAndGetBoundingBox(const std::string& filename);

private:
	void Reset();
    void ProcessFeatureChildren(const boost::property_tree::ptree& propertyElement);
    void ProcessCoordinateParent(const boost::property_tree::ptree& propertyElement);
    void ProcessGeometryChildren(const boost::property_tree::ptree& propertyElement);
    void ProcessCoordinateData(const std::string& coordStr);

	double m_minLat;
	double m_maxLat;
	double m_minLon;
	double m_maxLon;

    unsigned int m_numCoordinatesProcessed;

    boost::char_separator<char> m_spaceSeparator;
    boost::char_separator<char> m_commaSeparator;

}; //class BoundingBoxCalculator

#endif //BOUNDINGBOXCALCULATOR_H_