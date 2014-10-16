#include "xmlpropertytreefile.h"
#include <boost/property_tree/xml_parser.hpp>

namespace SynGlyphX {

	XMLPropertyTreeFile::XMLPropertyTreeFile()
	{
	}


	XMLPropertyTreeFile::~XMLPropertyTreeFile()
	{
	}

	void XMLPropertyTreeFile::ReadFromFile(const std::string& filename) {

		boost::property_tree::wptree propertyTree;
		boost::property_tree::read_xml(filename, propertyTree, boost::property_tree::xml_parser::trim_whitespace);

		ImportFromPropertyTree(propertyTree);
	}

	void XMLPropertyTreeFile::WriteToFile(const std::string& filename) const {

		boost::property_tree::wptree propertyTree;
		ExportToPropertyTree(propertyTree);

		boost::property_tree::xml_writer_settings<wchar_t> settings(L'\t', 1);
		boost::property_tree::write_xml(filename, propertyTree, std::locale(), settings);
	}

} //namesapce SynGlyphX