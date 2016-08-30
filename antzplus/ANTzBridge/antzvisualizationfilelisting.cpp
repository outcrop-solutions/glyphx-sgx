#include "antzvisualizationfilelisting.h"
#include <boost/uuid/uuid_io.hpp>

namespace SynGlyphXANTz {

	ANTzVisualizationFileListing::ANTzVisualizationFileListing() :
		SynGlyphX::XMLPropertyTreeFile(),
		m_id(SynGlyphX::UUIDGenerator::GetNewRandomUUID())
	{
		
	}
	
	ANTzVisualizationFileListing::ANTzVisualizationFileListing(const std::wstring& nodeFilename,
		const std::wstring& tagFilename,
		const std::wstring& channelFilename,
		const std::wstring& channelMapFilename,
		const std::wstring& baseImageFilename) :
		SynGlyphX::XMLPropertyTreeFile(),
		m_nodeFilename(nodeFilename),
		m_tagFilename(tagFilename),
		m_channelFilename(channelFilename),
		m_channelMapFilename(channelMapFilename),
		m_baseImageFilename(baseImageFilename),
		m_id(SynGlyphX::UUIDGenerator::GetNewRandomUUID()) {


	}

	ANTzVisualizationFileListing::ANTzVisualizationFileListing(const ANTzVisualizationFileListing& fileListing) :
		m_nodeFilename(fileListing.m_nodeFilename),
		m_tagFilename(fileListing.m_tagFilename),
		m_channelFilename(fileListing.m_channelFilename),
		m_channelMapFilename(fileListing.m_channelMapFilename),
		m_baseImageFilename(fileListing.m_baseImageFilename),
		m_id(fileListing.m_id) {


	}

	ANTzVisualizationFileListing::~ANTzVisualizationFileListing()
	{
	}

	ANTzVisualizationFileListing& ANTzVisualizationFileListing::operator=(const ANTzVisualizationFileListing& fileListing) {

		m_nodeFilename = fileListing.m_nodeFilename;
		m_tagFilename = fileListing.m_tagFilename;
		m_channelFilename = fileListing.m_channelFilename;
		m_channelMapFilename = fileListing.m_channelMapFilename;
		m_baseImageFilename = fileListing.m_baseImageFilename;
		m_id = fileListing.m_id;

		return *this;
	}
	
	bool ANTzVisualizationFileListing::UsesDefaultImage() const {

		return (m_baseImageFilename.empty());
	}

	bool ANTzVisualizationFileListing::IsValid() const {

		return ((!m_nodeFilename.empty()) && (m_channelFilename.empty() == m_channelMapFilename.empty()));
	}

	void ANTzVisualizationFileListing::ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) {

		const boost::property_tree::wptree& antzPropertyTree = filePropertyTree.get_child(L"ANTz");

		m_id = antzPropertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id");
		m_nodeFilename = antzPropertyTree.get<std::wstring>(L"Node");
		m_tagFilename = antzPropertyTree.get<std::wstring>(L"Tag", L"");
		m_channelFilename = antzPropertyTree.get<std::wstring>(L"Channel", L"");
		m_channelMapFilename = antzPropertyTree.get<std::wstring>(L"ChannelMap", L"");
		m_baseImageFilename = antzPropertyTree.get<std::wstring>(L"BaseImage", L"");
	}

	void ANTzVisualizationFileListing::ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const {

		boost::property_tree::wptree& antzPropertyTreeRoot = filePropertyTree.add(L"ANTz", L"");
		antzPropertyTreeRoot.put(L"<xmlattr>.id", m_id);
		antzPropertyTreeRoot.put(L"Node", m_nodeFilename);
		if (!m_tagFilename.empty()) {

			antzPropertyTreeRoot.put(L"Tag", m_tagFilename);
		}
		if (!m_channelFilename.empty()) {

			antzPropertyTreeRoot.put(L"Channel", m_channelFilename);
		}
		if (!m_channelMapFilename.empty()) {

			antzPropertyTreeRoot.put(L"ChannelMap", m_channelMapFilename);
		}
		if (!m_baseImageFilename.empty()) {

			antzPropertyTreeRoot.put(L"BaseImage", m_baseImageFilename);
		}
	}

	void ANTzVisualizationFileListing::Clear() {

		m_nodeFilename = L"";
		m_tagFilename = L"";
		m_channelFilename = L"";
		m_channelMapFilename = L"";
		m_baseImageFilename = L"";
		m_id = SynGlyphX::UUIDGenerator::GetNewRandomUUID();
	}

	const std::wstring& ANTzVisualizationFileListing::GetNodeFilename() const {

		return m_nodeFilename;
	}

	const std::wstring& ANTzVisualizationFileListing::GetTagFilename() const {

		return m_tagFilename;
	}

	const std::wstring& ANTzVisualizationFileListing::GetChannelFilename() const {

		return m_channelFilename;
	}

	const std::wstring& ANTzVisualizationFileListing::GetChannelMapFilename() const {

		return m_channelMapFilename;
	}

	const std::wstring& ANTzVisualizationFileListing::GetBaseImageFilename() const {

		return m_baseImageFilename;
	}

	const boost::uuids::uuid& ANTzVisualizationFileListing::GetID() const {

		return m_id;
	}

} //namespace SynGlyphXANTz
