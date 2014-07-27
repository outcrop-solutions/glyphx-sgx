#include "bindinglineedit.h"
#include <QtGui/QDragEnterEvent>
#include <QtCore/QMimeData>

BindingLineEdit::BindingLineEdit(QWidget *parent)
	: QLineEdit(parent)
{
	setReadOnly(true);
	setAcceptDrops(true);
}

BindingLineEdit::~BindingLineEdit()
{

}

void BindingLineEdit::dragEnterEvent(QDragEnterEvent *event) {

	if (event->mimeData()->hasFormat("application/datasource-field")) {

		event->acceptProposedAction();
	}
}

void BindingLineEdit::dropEvent(QDropEvent* event) {

	setText(event->mimeData()->data("application/datasource-field"));
}
