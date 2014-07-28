#include "bindinglineedit.h"
#include <QtGui/QDragEnterEvent>
#include <QtCore/QMimeData>
#include "inputfieldmimedata.h"
#include "databaseservices.h"

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

	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData != nullptr) {

		const SynGlyphX::InputField& inputfield = mimeData->GetInputField();
		setText(DatabaseServices::GetFormattedDBName(inputfield.GetDatasourceID()) + ":" + 
			QString::fromStdWString(inputfield.GetTable()) + ":" + 
			QString::fromStdWString(inputfield.GetField()) + ":" +
			QString::number(inputfield.GetMin()) + ":" + 
			QString::number(inputfield.GetMax()));
	}
}
