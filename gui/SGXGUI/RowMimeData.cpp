#include "RowMimeData.h"

namespace SynGlyphX {

	const QString RowMimeData::s_format("application/x-sgx-rows");

	RowMimeData::RowMimeData(const std::set<unsigned int>& rowList)
		: QMimeData(),
		m_rowList(rowList)
	{

	}

	RowMimeData::~RowMimeData()
	{

	}

} //namespace SynGlyphX