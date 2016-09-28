#include "SourceDataTableModel.h"

SourceDataTableModel::SourceDataTableModel(bool doesEmptyFilterShowAll, QObject *parent)
	: QSortFilterProxyModel(parent),
	m_doesEmptyFilterShowAll(doesEmptyFilterShowAll)
{
	setDynamicSortFilter(true);
	setSortRole(Qt::InitialSortOrderRole);
}

SourceDataTableModel::~SourceDataTableModel()
{

}

void SourceDataTableModel::SetFilters(const SynGlyphX::IndexSet& rowSubset) {

	beginResetModel();
	m_rowSubset = rowSubset;
	endResetModel();
}

bool SourceDataTableModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {

	if (m_rowSubset.empty()) {

		return m_doesEmptyFilterShowAll;
	}
	else {

		return (m_rowSubset.count(source_row) > 0);
	}	
}

bool SourceDataTableModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const {

	return true;
}