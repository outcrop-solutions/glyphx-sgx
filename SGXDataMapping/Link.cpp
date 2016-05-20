#include "Link.h"
#include <boost/uuid/uuid_io.hpp>
namespace SynGlyphX {

	Link::Color::Color(const boost::property_tree::wptree& propertyTree) :
		m_r(propertyTree.get<unsigned short>(L"<xmlattr>.R")), 
		m_g(propertyTree.get<unsigned short>(L"<xmlattr>.G")), 
		m_b(propertyTree.get<unsigned short>(L"<xmlattr>.B")),
		m_alpha(propertyTree.get<unsigned short>(L"<xmlattr>.A")),
		m_inheritfromParent(propertyTree.get_optional<std::wstring>(L"<xmlattr>.inherit").is_initialized())
	{
	}
	boost::property_tree::wptree& Link::Color::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const  {
		
		if (m_inheritfromParent)
			propertyTree.put(L"<xmlattr>.inherit", L"parent");
		//else ? do we want to save other properties if inherit from parent?

		propertyTree.put<short>(L"<xmlattr>.R", m_r);
		propertyTree.put<short>(L"<xmlattr>.G", m_g);
		propertyTree.put<short>(L"<xmlattr>.B", m_b);
		propertyTree.put<short>(L"<xmlattr>.A", m_alpha);

		return propertyTree;
	}

	Link::Node::Node(const boost::property_tree::wptree& propertyTree) :
		m_treeId(propertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id")),
		m_label(propertyTree.get<unsigned long>(L"<xmlattr>.label")),
		m_inputFieldId(propertyTree.get<size_t>(L"<xmlattr>.binding"))
	{
	}
	boost::property_tree::wptree& Link::Node::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const  {
		propertyTree.put(L"<xmlattr>.label", m_label);
		propertyTree.put(L"<xmlattr>.id", m_treeId);
		propertyTree.put(L"<xmlattr>.binding", m_inputFieldId);
		return propertyTree;
	}
	Link::Function::Function(const boost::property_tree::wptree& parentPropertyTree) {

	}
	boost::property_tree::wptree& Link::Function::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const  {
	
		propertyTree.put(L"<xmlattr>.type", L"Match Value");
		return propertyTree;
	}


	Link::Link(const boost::property_tree::wptree& propertyTree):
		m_name(propertyTree.get<std::wstring>(L"<xmlattr>.name")),
		m_start(propertyTree.get_child(L"Begin")), 
		m_end(propertyTree.get_child(L"End")),
		m_function(propertyTree.get_child(L"Function")),
		m_color(propertyTree.get_child(L"Color"))
		
	{

	}

	boost::property_tree::wptree& Link::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const  {
		boost::property_tree::wptree& rootPropertyTree = propertyTree.add(L"Link", L"");
		rootPropertyTree.put<std::wstring>(L"<xmlattr>.name", m_name);
		rootPropertyTree.add(L"Shape", L"Cube");
		m_color.ExportToPropertyTree(rootPropertyTree.add(L"Color", L""));
		m_start.ExportToPropertyTree(rootPropertyTree.add(L"Begin", L""));
		m_end.ExportToPropertyTree(rootPropertyTree.add(L"End", L""));
		m_function.ExportToPropertyTree(rootPropertyTree.add(L"Function", L""));
		return rootPropertyTree;
	}


}