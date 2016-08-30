#include "datamappingvirtualtopology.h"
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	DataMappingVirtualTopology::DataMappingVirtualTopology() :
		VirtualTopologyTemplate < VirtualTopologyMappingProperty >() {
	
	}

	DataMappingVirtualTopology::DataMappingVirtualTopology(const DataMappingVirtualTopology& virtualTopology) :
		VirtualTopologyTemplate < VirtualTopologyMappingProperty >(virtualTopology) {

	}

	DataMappingVirtualTopology::DataMappingVirtualTopology(const VirtualTopology& virtualTopology) :
		VirtualTopologyTemplate < VirtualTopologyMappingProperty >(VirtualTopologyMappingProperty(virtualTopology.GetType())) {

	}

	DataMappingVirtualTopology::DataMappingVirtualTopology(const boost::property_tree::wptree& propertyTree, bool useOldPropertyTree) :
		VirtualTopologyTemplate < VirtualTopologyMappingProperty >() {

		if (useOldPropertyTree) {

			m_data = propertyTree.get<VirtualTopologyInfo::Type>(L"<xmlattr>.Topology");
		}
		else {

			m_data = VirtualTopologyMappingProperty(propertyTree.get_child(L"Type"));
		}
	}

	DataMappingVirtualTopology::~DataMappingVirtualTopology()
	{
	}

	DataMappingVirtualTopology& DataMappingVirtualTopology::operator=(const DataMappingVirtualTopology& virtualTopology) {

		VirtualTopologyTemplate < VirtualTopologyMappingProperty >::operator=(virtualTopology);

		return *this;
	}

	bool DataMappingVirtualTopology::operator==(const DataMappingVirtualTopology& virtualTopology) const {

		return VirtualTopologyTemplate < VirtualTopologyMappingProperty >::operator==(virtualTopology);
	}

	bool DataMappingVirtualTopology::operator!=(const DataMappingVirtualTopology& virtualTopology) const {

		return !operator==(virtualTopology);
	}

	boost::property_tree::wptree& DataMappingVirtualTopology::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& topologyPropertyTree = propertyTree.add(L"VirtualTopology", L"");

		boost::property_tree::wptree& typePropertyTree = topologyPropertyTree.add(L"Type", L"");
		m_data.ExportToPropertyTree(typePropertyTree);

		return topologyPropertyTree;
	}

	VirtualTopology DataMappingVirtualTopology::ExportVirtualTopology() const {

		VirtualTopology virtualTopology;
		virtualTopology.SetType(m_data.GetValue());

		return virtualTopology;
	}

} //namespace SynGlyphX