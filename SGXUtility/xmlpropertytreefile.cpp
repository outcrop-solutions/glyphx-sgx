#include "xmlpropertytreefile.h"
#include <chrono>
#include <boost/lexical_cast.hpp>

namespace SynGlyphX {

    boost::property_tree::xml_writer_settings<std::wstring> XMLPropertyTreeFile::s_writeSettings(L'\t', 1);
	const std::wstring XMLPropertyTreeFile::s_timestampName = L"<xmlattr>.Timestamp";

	XMLPropertyTreeFile::XMLPropertyTreeFile(bool includeTimestamp) :
		m_includeTimestamp(includeTimestamp)
	{
	}


	XMLPropertyTreeFile::~XMLPropertyTreeFile()
	{
	}

	void XMLPropertyTreeFile::ReadFromFile(const std::string& filename) {

		boost::property_tree::read_xml(filename, m_originalPropertyTree, GetReadFlags());

		ImportFromPropertyTree(m_originalPropertyTree);
	}

	void XMLPropertyTreeFile::WriteToFile(const std::string& filename) const {

		boost::property_tree::wptree propertyTree;
		ExportToPropertyTree(propertyTree);

		if (propertyTree.empty()) {

			throw std::runtime_error("Unable to write to property tree file since property tree is empty.");
		}

		if (m_includeTimestamp) {

			boost::optional<std::chrono::seconds::rep> timestamp = boost::none;
			if (!m_originalPropertyTree.empty()) {

				timestamp = m_originalPropertyTree.begin()->second.get_optional<long long>(s_timestampName);
			}

			if ((!timestamp.is_initialized()) || IsDifferentFromGivenPropertyTree(m_originalPropertyTree)) {

				timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
			
			propertyTree.begin()->second.put<std::chrono::seconds::rep>(s_timestampName, timestamp.get());
		}

		boost::property_tree::write_xml(filename, propertyTree, std::locale(), GetWriteSettings());
	}

	bool XMLPropertyTreeFile::IsDifferentFromGivenPropertyTree(const boost::property_tree::wptree& originalPropertyTree) const {

		return false;
	}

	int XMLPropertyTreeFile::GetReadFlags() {

		return boost::property_tree::xml_parser::trim_whitespace;
	}

    const boost::property_tree::xml_writer_settings<std::wstring>& XMLPropertyTreeFile::GetWriteSettings() {

		return s_writeSettings;
	}

} //namesapce SynGlyphX
