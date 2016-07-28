#include "Link.h"
#include <boost/uuid/uuid_io.hpp>
namespace SynGlyphX {

	//class MatchFunction : public Link::Function {
	//public:
	//	virtual ~MatchFunction() {}
	//	virtual boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) {
	//		propertyTree.put(L"<xmlattr>.type", L"Match Value");
	//		return propertyTree;
	//	}
	//	virtual Type GetType() { return Type::MatchValue; }
	//};

	//class MatchFunction : public Link::Function {
	//public:
	//	virtual ~MatchFunction() {}
	//	virtual boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) {
	//		propertyTree.put(L"<xmlattr>.type", L"Key to Value");
	//		return propertyTree;
	//	}
	//	virtual Type GetType() { return Type::MatchValue; }
	//};

	//std::shared_ptr<Link::Function> Link::Function::CreateFunction(const boost::property_tree::wptree& parentPropertyTree) {
	//	
	//	return std::make_shared<MatchFunction>();
	//}

	Link::Function::Function(const boost::property_tree::wptree& propertyTree) {

		m_propertyTree = propertyTree;
		//std::wstring type = m_propertyTree.get<std::wstring>(L"<xmlattr>.type");
		//if (type == L"Match Value")
		//	m_type = Type::MatchValue;
		//else if (type == L"Key to Value")
		//	m_type = Type::KeyToValue;
		//else if (type == L"Key to Range")
		//	m_type = Type::KeyToRange;
	}

	//Link::Function::Function() {

	//}
	boost::property_tree::wptree& Link::Function::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const  {

		//propertyTree.put(L"<xmlattr>.type", L"Match Value");
		//propertyTree.add_child(L"Function", m_propertyTree);
		//std::wstring type = m_propertyTree.get<std::wstring>(L"<xmlattr>.type");
		propertyTree.put_child(L"Function", m_propertyTree);
		//propertyTree.push_back(m_propertyTree);
		return propertyTree;
	}

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

	LinkNode::LinkNode(const boost::property_tree::wptree& propertyTree) :
		m_treeId(propertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id")),
		m_label(propertyTree.get<unsigned long>(L"<xmlattr>.label")),
		m_inputFieldId(propertyTree.get<std::wstring>(L"<xmlattr>.binding"))
	{
	}

	boost::property_tree::wptree& LinkNode::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const  {
		propertyTree.put(L"<xmlattr>.label", m_label);
		propertyTree.put(L"<xmlattr>.id", m_treeId);
		propertyTree.put(L"<xmlattr>.binding", m_inputFieldId);
		return propertyTree;
	}

	Link::Link(const boost::property_tree::wptree& propertyTree):
		m_name(propertyTree.get<std::wstring>(L"<xmlattr>.name")),
		m_shape(propertyTree.get<std::wstring>(L"Shape")),
		m_start(propertyTree.get_child(L"Begin")), 
		m_end(propertyTree.get_child(L"End")),
		//m_function(Function::CreateFunction(propertyTree.get_child(L"Function"))),
		m_function(propertyTree.get_child(L"Function")),
		m_color(propertyTree.get_child(L"Color"))
		
	{

	}

	Link::Link() /*: m_function(new MatchFunction)*/ {}

	Link::~Link() {

	}

	boost::property_tree::wptree& Link::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const  {
		boost::property_tree::wptree& rootPropertyTree = propertyTree.add(L"Link", L"");
		rootPropertyTree.put<std::wstring>(L"<xmlattr>.name", m_name);
		rootPropertyTree.add(L"Shape", m_shape);
		m_color.ExportToPropertyTree(rootPropertyTree.add(L"Color", L""));
		m_start.ExportToPropertyTree(rootPropertyTree.add(L"Begin", L""));
		m_end.ExportToPropertyTree(rootPropertyTree.add(L"End", L""));
		//m_function.ExportToPropertyTree(rootPropertyTree.add(L"Function", L""));
		m_function.ExportToPropertyTree(rootPropertyTree);
		return rootPropertyTree;
	}
}
