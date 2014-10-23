#ifndef ANTZDATA_H
#define ANTZDATA_H

#include "ANTzBridge.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "data/nptypes.h"

namespace ANTzPlus {

	class ANTZBRIDGE_API ANTzData : public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<ANTzData> SharedPtr;

		ANTzData();
		~ANTzData();

		pData GetData() const;

	private:
		pData m_data;
	};

} //namespace ANTz

#endif //ANTZDATA_H