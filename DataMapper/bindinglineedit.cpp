#include "bindinglineedit.h"
#include <QtGui/QDragEnterEvent>
#include <QtCore/QMimeData>
#include <QtWidgets/QMenu>
#include "inputfieldmimedata.h"
#include "databaseservices.h"

BindingLineEdit::BindingLineEdit(QWidget *parent)
	: QLineEdit(parent)
{
	setReadOnly(true);
	setAcceptDrops(true);

	m_clearAction = new QAction(tr("Clear Input Field"), this);
	m_clearAction->setEnabled(false);
	QObject::connect(m_clearAction, &QAction::triggered, this, &BindingLineEdit::Clear);

	//m_useInputFieldMinMaxActon = new QAction(tr("Set Min/Max To Input Field Min/Max"), this);
	//m_useInputFieldMinMaxActon->setEnabled(false);
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
	else {
		setText("");
	}

	m_clearAction->setEnabled(m_inputField.IsValid());
	//m_useInputFieldMinMaxActon->setEnabled(m_inputField.IsValid());
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
		emit ValueChangedByUser(mimeData->GetInputField());
	}
}

void BindingLineEdit::contextMenuEvent(QContextMenuEvent* event) {

	QMenu* menu = new QMenu(this);
	menu->addAction(m_clearAction);
	//menu->addSeparator();
	//menu->addAction(m_useInputFieldMinMaxActon);

	menu->exec(event->globalPos());

	delete menu;
}

void BindingLineEdit::Clear() {

	SynGlyphX::InputField emptyInputField;
	SetInputField(emptyInputField);
	emit ValueChangedByUser(emptyInputField);
}
