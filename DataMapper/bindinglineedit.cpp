#include "bindinglineedit.h"
#include <QtGui/QDragEnterEvent>
#include <QtCore/QMimeData>
#include <QtWidgets/QMenu>
#include "inputfieldmimedata.h"
#include "databaseservices.h"
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>

BindingLineEdit::BindingLineEdit(QWidget *parent, bool onlyAcceptsNumericFields)
	: QLineEdit(parent),
	m_onlyAcceptsNumericFields(onlyAcceptsNumericFields)
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

void BindingLineEdit::SetInputField(const SynGlyphX::InputField& inputfield) {

	m_inputField = inputfield;
	if (m_inputField.IsValid()) {

		QString text = SynGlyphX::DatabaseServices::GetFormattedDBName(inputfield.GetDatasourceID()) + ":" + QString::fromStdWString(inputfield.GetTable()) + ":" + QString::fromStdWString(inputfield.GetField());
		setText(text);
	}
	else {

		setText("");
	}

	m_clearAction->setEnabled(m_inputField.IsValid());
	//m_useInputFieldMinMaxActon->setEnabled(m_inputField.IsValid());
}

void BindingLineEdit::dragEnterEvent(QDragEnterEvent *event) {

	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData == nullptr) {

		return;
	}

	if (m_onlyAcceptsNumericFields) {
		
		if (!mimeData->GetInputField().IsNumeric()) {
			return;
		}
	}
		
	event->acceptProposedAction();
}

void BindingLineEdit::dropEvent(QDropEvent* event) {

	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData != nullptr) {

		//SetInputField(mimeData->GetInputField());
		m_inputField = mimeData->GetInputField();
		emit ValueChangedByUser(m_inputField);
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

	m_inputField = SynGlyphX::InputField();
	emit ValueChangedByUser(m_inputField);
}
