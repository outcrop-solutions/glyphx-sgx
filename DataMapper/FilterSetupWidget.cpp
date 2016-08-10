#include "FilterSetupWidget.h"
#include <QtWidgets/QHeaderView>
#include "DMGlobal.h"
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QUndoCommand>

FilterSetupWidget::FilterSetupWidget(QWidget *parent)
	: QTableView(parent)
{
	setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	verticalHeader()->hide();
	horizontalHeader()->setStretchLastSection(false);
	horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

	QObject::connect(horizontalHeader(), &QHeaderView::sectionClicked, this, &FilterSetupWidget::OnHeaderSectionClicked);
}

FilterSetupWidget::~FilterSetupWidget()
{

}

void FilterSetupWidget::OnHeaderSectionClicked(int index)
{
	if (index == 0){

		int row = model()->rowCount();

		auto command = new AddAliasCommand(m_aliasesTableWidget, GenerateUniqueName("A" + QString::number(row)));
		DMGlobal::Services()->GetUndoStack()->push(command);
	}

}