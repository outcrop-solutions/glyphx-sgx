#include "sourcedataselectionmodel.h"

SourceDataSelectionModel::SourceDataSelectionModel()
{
}

SourceDataSelectionModel::~SourceDataSelectionModel()
{
}

SynGlyphX::IndexSet SourceDataSelectionModel::GetRootRows(const QModelIndexList& indexList) {

	SynGlyphX::IndexSet rows;
	Q_FOREACH(const QModelIndex& index, indexList) {

		rows.insert(GetRootRow(index));
	}

	return rows;
}

unsigned long SourceDataSelectionModel::GetRootRow(const QModelIndex& index) {

	QModelIndex ancestor = index;
	while (ancestor.parent().isValid()) {

		ancestor = ancestor.parent();
	}

	return ancestor.row();
}