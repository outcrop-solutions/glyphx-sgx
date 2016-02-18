#include "tree2listfilterproxymodel.h"

namespace SynGlyphX {

	Tree2ListFilterProxyModel::Tree2ListFilterProxyModel(QObject *parent)
		: QSortFilterProxyModel(parent)
	{

	}

	Tree2ListFilterProxyModel::~Tree2ListFilterProxyModel()
	{

	}

	bool Tree2ListFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {

		QAbstractItemModel* model = sourceModel();

		if (model != nullptr) {

			return (m_parent == source_parent);
		}
		else {

			return false;
		}
	}

	void Tree2ListFilterProxyModel::SetFilterParent(const QModelIndex& parent) {

		m_parent = parent;
		invalidateFilter();
	}

} //namespace SynGlyphX