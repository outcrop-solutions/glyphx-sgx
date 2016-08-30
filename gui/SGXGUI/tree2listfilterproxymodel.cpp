#include "tree2listfilterproxymodel.h"

namespace SynGlyphX {

	Tree2ListFilterProxyModel::Tree2ListFilterProxyModel(QObject *parent)
		: QSortFilterProxyModel(parent)
	{
		setDynamicSortFilter(true);
		setSortRole(Qt::InitialSortOrderRole);
	}

	Tree2ListFilterProxyModel::~Tree2ListFilterProxyModel()
	{

	}

	bool Tree2ListFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {

		QAbstractItemModel* model = sourceModel();

		if (model != nullptr) {

			if ((!m_parent.isValid()) && (!source_parent.isValid())) {

				return true;
			}
			else {

				return (m_parent == source_parent);
			}
		}
		else {

			return false;
		}
	}

	bool Tree2ListFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const {

		return true;
	}

	void Tree2ListFilterProxyModel::SetFilterParent(const QModelIndex& parent) {

		m_parent = parent;
		invalidateFilter();
	}

} //namespace SynGlyphX