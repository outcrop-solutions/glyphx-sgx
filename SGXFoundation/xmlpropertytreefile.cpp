#include "xmlpropertytreefile.h"

namespace SynGlyphX {

	boost::property_tree::xml_writer_settings<wchar_t> XMLPropertyTreeFile::s_writeSettings(L'\t', 1);

	XMLPropertyTreeFile::XMLPropertyTreeFile()
	{
	}


	XMLPropertyTreeFile::~XMLPropertyTreeFile()
	{
	}

	void XMLPropertyTreeFile::ReadFromFile(const std::string& filename) {

		boost::property_tree::wptree propertyTree;
		boost::property_tree::read_xml(filename, propertyTree, GetReadFlags());

		ImportFromPropertyTree(propertyTree);
	}

	void XMLPropertyTreeFile::WriteToFile(const std::string& filename) const {

		boost::property_tree::wptree propertyTree;
		ExportToPropertyTree(propertyTree);

		boost::property_tree::write_xml(filename, propertyTree, std::locale(), GetWriteSettings());
	}

	int XMLPropertyTreeFile::GetReadFlags() {

		return boost::property_tree::xml_parser::trim_whitespace;
	}

	const boost::property_tree::xml_writer_settings<wchar_t>& XMLPropertyTreeFile::GetWriteSettings() {

		return s_writeSettings;
	}

} //namesapce SynGlyphX