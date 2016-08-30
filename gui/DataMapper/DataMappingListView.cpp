#include "DataMappingListView.h"

DataMappingListView::DataMappingListView(QWidget *parent)
	: QListView(parent)
{
	setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
}

DataMappingListView::~DataMappingListView()
{

}

bool DataMappingListView::edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) {

	if (trigger == QAbstractItemView::EditTrigger::DoubleClicked) {

		ShowEditDialog(index);
		return false;
	}
	else {

		return QListView::edit(index, trigger, event);
	}
}