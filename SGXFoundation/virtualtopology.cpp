#include "virtualtopology.h"

namespace SynGlyphX {

	template<typename DataType>
	VirtualTopologyTemplate<DataType>::VirtualTopologyTemplate(const DataType& data) :
		m_data(data) {


	}

	template<typename DataType>
	VirtualTopologyTemplate<DataType>::VirtualTopologyTemplate(const VirtualTopologyTemplate& virtualTopology) :
		m_data(virtualTopology.m_data) {


	}

	template<typename DataType>
	VirtualTopologyTemplate<DataType>::~VirtualTopologyTemplate() {


	}

	template<typename DataType>
	VirtualTopologyTemplate<DataType>& VirtualTopologyTemplate<DataType>::operator=(const VirtualTopologyTemplate& virtualTopology) {

		m_data = virtualTopology.m_data;

		return *this;
	}

	template<typename DataType>
	bool VirtualTopologyTemplate<DataType>::operator==(const VirtualTopologyTemplate& virtualTopology) const {

		return (m_data == virtualTopology.m_data);
	}

	template<typename DataType>
	bool VirtualTopologyTemplate<DataType>::operator!=(const VirtualTopologyTemplate& virtualTopology) const {
	
		return !operator==(virtualTopology);
	}

	template<typename DataType>
	const DataType& VirtualTopologyTemplate<DataType>::GetType() const {

		return m_data;
	}

	template<typename DataType>
	void VirtualTopologyTemplate<DataType>::SetType(const DataType& data) {

		m_data = data;
	}

	template class VirtualTopologyTemplate < VirtualTopologyInfo::Type >;
}