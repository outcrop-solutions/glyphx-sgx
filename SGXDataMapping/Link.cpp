#include "Link.h"
#include <boost/uuid/uuid_io.hpp>
namespace SynGlyphX {

	boost::property_tree::wptree& Link::Color::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const  {
		
		if (m_inheritfromParent)
			propertyTree.put(L"<xmlattr>.inherit", L"parent");
		//else ? do we want to save other properties if inherit from parent?

		propertyTree.put<short>(L"<xmlattr>.R", m_color[0]);
		propertyTree.put<short>(L"<xmlattr>.G", m_color[1]);
		propertyTree.put<short>(L"<xmlattr>.B", m_color[2]);
		propertyTree.put<short>(L"<xmlattr>.A", m_alpha);

		return propertyTree;
	}

	boost::property_tree::wptree& Link::Node::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const  {
		propertyTree.put(L"<xmlattr>.label", m_label);
		propertyTree.put(L"<xmlattr>.id", m_treeId);
		propertyTree.put(L"<xmlattr>.binding", m_inputField.GetHashID());
		return propertyTree;
	}

	boost::property_tree::wptree& Link::Function::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const  {
	
		propertyTree.put(L"<xmlattr>.type", L"Match Value");
		return propertyTree;
	}


	Link::Link(const boost::property_tree::wptree& propertyTree):
		m_name(propertyTree.get<std::wstring>(L"<xmlattr>.name"))//,
		//m_start(propertyTree.get_child(L"Begin"))
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