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

const SynGlyphX::InputField& BindingLineEdit::GetInputField() const {

	return m_inputField;
}

void BindingLineEdit::SetInputField(const SynGlyphX::InputField& inputField) {

	m_inputField = inputField;

	if (m_inputField.IsValid()) {

		setText(DatabaseServices::GetFormattedDBName(m_inputField.GetDatasourceID()) + ":" +
			QString::fromStdWString(m_inputField.GetTable()) + ":" +
			QString::fromStdWString(m_inputField.GetField()) + ":" +
			QString::number(m_inputField.GetMin()) + ":" +
			QString::number(m_inputField.GetMax()));
	}
}

void BindingLineEdit::dragEnterEvent(QDragEnterEvent *event) {

	if (event->mimeData()->hasFormat("application/datasource-field")) {

		event->acceptProposedAction();
	}
}

void BindingLineEdit::dropEvent(QDropEvent* event) {

	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData != nullptr) {

		SetInputField(mimeData->GetInputField());
	}
}
