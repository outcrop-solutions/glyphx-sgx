#include "bindinglineedit.h"
#include <QtGui/QDragEnterEvent>
#include <QtCore/QMimeData>
#include <QtWidgets/QMenu>
#include "inputfieldmimedata.h"
#include <QtWidgets/QHBoxLayout>
#include "datatransformmapping.h"
#include <QtWidgets/QUndoCommand>
#include <QtCore/QPointer>
#include "AppGlobal.h"
#include <QtWidgets/QUndoStack>

class BindingLineEditChangeCommand : public QUndoCommand {
public:
	BindingLineEditChangeCommand(BindingLineEdit* ble, const std::wstring& newInputField) : 
		m_ble(ble),
		m_newInputField(newInputField),
		m_oldInputField(ble->m_inputFieldId) // since it is called from ble, should not  not be null at time of construction
	{	
	}
	void undo() override {
		if (m_ble) {
			m_ble->m_inputFieldId = m_oldInputField;
			m_ble->ValueChangedByUser(m_ble->m_inputFieldId);
		}
	}
	void redo() override {
		if (m_ble) {
			m_ble->m_inputFieldId = m_newInputField;
			m_ble->ValueChangedByUser(m_ble->m_inputFieldId);
		}
	}
	QPointer<BindingLineEdit> m_ble;
	std::wstring m_newInputField;
	std::wstring m_oldInputField;
};

BindingLineEdit::BindingLineEdit(const GlyphRolesTableModel* model, QWidget *parent, SynGlyphX::MappingFunctionData::Input acceptedInputTypes)
	: QWidget(parent),
	m_model(model),
	m_acceptedInputTypes(acceptedInputTypes)
{
	m_lineEdit = new QLineEdit(this);
	m_lineEdit->setContextMenuPolicy(Qt::NoContextMenu);
	m_lineEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	m_lineEdit->setReadOnly(true);

	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(m_lineEdit);

	setLayout(mainLayout);

	setAcceptDrops(true);

	m_clearAction = new QAction(tr("Clear Input Field"), this);
	m_clearAction->setEnabled(false);
	QObject::connect(m_clearAction, &QAction::triggered, this, &BindingLineEdit::Clear);

	setFocusPolicy(Qt::StrongFocus);
	setFocusProxy(m_lineEdit);
}

BindingLineEdit::~BindingLineEdit()
{

}

const std::wstring& BindingLineEdit::GetInputField() const {

	return m_inputFieldId;
}

void BindingLineEdit::SetInputField(const std::wstring& inputfield) {

	m_inputFieldId = inputfield;
	//TODO implement correctly
	m_lineEdit->setText(QString::fromStdWString(inputfield));
	//if (m_inputFieldId[0] == '~')
	//if (m_inputFieldId.GetInputField().IsValid()) {

	//	SynGlyphX::Datasource::ConstSharedPtr datasource = m_model->GetDataTransformMapping()->GetDatasources().at(inputfield.GetInputField().GetDatasourceID());

	//	QString text = QString::fromStdWString(datasource->GetFormattedName());
	//	if (datasource->CanDatasourceHaveMultipleTables()) {
	//	
	//		text += ":" + QString::fromStdWString(inputfield.GetInputField().GetTable());
	//	}
	//	text += ":" + QString::fromStdWString(inputfield.GetInputField().GetField());
	//	m_lineEdit->setText(text);
	//}
	//else {

	//	m_lineEdit->clear();
	//}

	m_clearAction->setEnabled(true);
	//m_clearAction->setEnabled(m_inputFieldId.GetInputField().IsValid());
	//m_useInputFieldMinMaxActon->setEnabled(m_inputField.IsValid());
}

void BindingLineEdit::SetAcceptedInputTypes(SynGlyphX::MappingFunctionData::Input acceptedInputTypes) {

	m_acceptedInputTypes = acceptedInputTypes;

	//TODO: Impmlement correctly after refactoring
	//If input type has changed then clear input field if input field no longer matches input type
	//if ((m_acceptedInputTypes == SynGlyphX::MappingFunctionData::Input::Numeric) && (m_inputFieldId.GetInputField().IsValid()) && (!m_inputFieldId.GetInputField().IsNumeric())) {

	//	Clear();
	//}
	//else if ((m_acceptedInputTypes == SynGlyphX::MappingFunctionData::Input::Text) && (m_inputFieldId.GetInputField().IsValid()) && (m_inputFieldId.GetInputField().IsNumeric())) {

	//	Clear();
	//}
}

void BindingLineEdit::dragEnterEvent(QDragEnterEvent *event) {

	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData == nullptr) {

		return;
	}

	if ((m_acceptedInputTypes == SynGlyphX::MappingFunctionData::Input::Numeric) && (!mimeData->GetInputField().IsNumeric())) {
		
		return;
	}
	else if ((m_acceptedInputTypes == SynGlyphX::MappingFunctionData::Input::Text) && (mimeData->GetInputField().IsNumeric())) {

		return;
	}

	if (!m_model->IsInputFieldCompatible(mimeData->GetInputField())) {

		return;
	}
		
	event->acceptProposedAction();
}

void BindingLineEdit::dropEvent(QDropEvent* event) {

	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData != nullptr) {
		auto command = new BindingLineEditChangeCommand(this, L"Test" /*mimeData->GetInputField()*/);
		command->setText(tr("Change Binding"));
		SynGlyphX::AppGlobal::Services()->GetUndoStack()->push(command);
		//SetInputField(mimeData->GetInputField());
		//m_inputFieldId = mimeData->GetInputField();
		//emit ValueChangedByUser(m_inputFieldId);
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

	if (!m_inputFieldId.empty()) {

		m_inputFieldId.clear();
		emit ValueChangedByUser(m_inputFieldId);
	}
}
