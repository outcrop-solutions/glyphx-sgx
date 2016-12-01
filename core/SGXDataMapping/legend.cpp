#include "legend.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	const std::wstring Legend::s_propertyTreeName(L"Legend");

	const Legend::TypeBimap Legend::s_typeStrings = boost::assign::list_of < Legend::TypeBimap::relation >
		(Legend::Type::Image, L"Image")
		(Legend::Type::Text, L"Text");

	Legend::Legend()
	{
	}

	Legend::Legend(const boost::property_tree::wptree& propertyTree) :
		m_title(propertyTree.get<std::wstring>(L"<xmlattr>.Title")),
        m_filename(propertyTree.get_value<std::wstring>()),
		m_type(s_typeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Type"))) {


	}

	Legend::Legend(const Legend& legend) :
		m_title(legend.m_title),
		m_filename(legend.m_filename),
		m_type(legend.m_type) {

	}

	Legend::~Legend()
	{
	}

	Legend& Legend::operator=(const Legend& legend) {

		m_title = legend.m_title;
		m_filename = legend.m_filename;
		m_type = legend.m_type;

		return *this;
	}

	bool Legend::operator==(const Legend& legend) const {

		if (m_title == legend.m_title) {

			return false;
		}

		if (m_filename == legend.m_filename) {

			return false;
		}

		if (m_type == legend.m_type) {

			return false;
		}

		return true;
	}

	bool Legend::operator!=(const Legend& legend) const {

		return (!operator==(legend));
	}

	boost::property_tree::wptree& Legend::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& rootPropertyTree = propertyTree.add(s_propertyTreeName, m_filename);

		rootPropertyTree.put<std::wstring>(L"<xmlattr>.Title", m_title);
		rootPropertyTree.put<std::wstring>(L"<xmlattr>.Type", s_typeStrings.left.at(m_type));

		return rootPropertyTree;
	}

	void Legend::SetTitle(const std::wstring& title) {

		m_title = title;
	}

	void Legend::SetFilename(const std::wstring& filename) {

		m_filename = filename;
		m_type = Type::Image;
	}

	const std::wstring& Legend::GetTitle() const {

		return m_title;
	}

	const std::wstring& Legend::GetFilename() const {

		return m_filename;
	}

	Legend::Type Legend::GetType() const {

		return m_type;
	}

	bool Legend::IsValid() const {

		return !(m_title.empty() || m_filename.empty());
	}

	bool Legend::CanFileBeFound() const {

		boost::filesystem::path filePath(m_filename);
		return (boost::filesystem::exists(filePath) && boost::filesystem::is_regular_file(filePath));
	}

} //namespace SynGlyphX
