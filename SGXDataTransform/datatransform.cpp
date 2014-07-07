#include "datatransform.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace SynGlyphX {

    DataTransform::DataTransform()
    {
    }


    DataTransform::~DataTransform()
    {
    }

    void DataTransform::ReadFromFile(const std::wstring& filename) {

        Clear();

        std::string narrowFilename(filename.begin(), filename.end());

        boost::property_tree::wptree dataTransformPropertyTree;
        boost::property_tree::read_xml(narrowFilename, dataTransformPropertyTree);
    }

    void DataTransform::WriteToFile(const std::wstring& filename) const {

    }

    const std::map<std::wstring, Datasource>& DataTransform::GetDatasources() const {

        return m_datasources;
    }

    void DataTransform::Clear() {

        m_datasources.clear();
    }

} //namespace SynGlyphX