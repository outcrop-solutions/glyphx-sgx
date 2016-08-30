#include "tablesubsetproxymodel.h"

namespace SynGlyphX {

	TableSubsetProxyModel::TableSubsetProxyModel(QObject *parent)
		: QSortFilterProxyModel(parent)
	{
		setDynamicSortFilter(true);
		setSortRole(Qt::InitialSortOrderRole);
	}

	TableSubsetProxyModel::~TableSubsetProxyModel()
	{

	}

	void TableSubsetProxyModel::SetRowSubset(Subset rowSubset) {

		m_rowSubset = rowSubset;

		invalidateFilter();
	}

	void TableSubsetProxyModel::SetColumnSubset(Subset columnSubset) {

		m_columnSubset = columnSubset;

		invalidateFilter();
	}

	bool TableSubsetProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {

		if (source_parent.isValid()) {

			return false;
		}

		return (m_rowSubset.empty() || (m_rowSubset.count(source_row) > 0));
	}

	bool TableSubsetProxyModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const {

		if (source_parent.isValid()) {

			return false;
		}

		return (m_columnSubset.empty() || (m_columnSubset.count(source_column) > 0));
	}

} //namespace SynGlyphX