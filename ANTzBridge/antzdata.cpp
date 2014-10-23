#include "antzdata.h"
#include "npdata.h"
#include "io/npfile.h"
#include "io/npch.h"
#include "npctrl.h"

namespace ANTzPlus {

	ANTzData::ANTzData() :
		m_data(static_cast<pData>(npInitData(0, NULL)))
	{
		npInitFile(m_data);
		npInitCh(m_data);
		npInitCtrl(m_data);
	}


	ANTzData::~ANTzData()
	{
		npCloseCtrl(m_data);
		npCloseFile(m_data);
		npCloseCh(m_data);
		npCloseData();
	}

	pData ANTzData::GetData() const {

		return m_data;
	}

} //namespace ANTzPlus