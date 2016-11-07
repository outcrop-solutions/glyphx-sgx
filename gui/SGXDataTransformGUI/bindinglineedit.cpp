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
#include "datatransformmodel.h" //refactor to exclude this
#include "GlyphTreesViewMementoBase.h"

class BindingLineEditChangeCommand : public QUndoCommand {
public:
	BindingLineEditChangeCommand(BindingLineEdit* ble, const QString& newInputField) : 
		m_ble(ble),
		m_newInputField(newInputField),
		m_oldInputField(ble->m_inputFieldId), // since it is called from ble, should not  not be null at time of construction
		m_firstCall(true)
	{
		// this will not work after undoing tree operation, will need to somehow store selection in a model-independent way
		//m_selection = SynGlyphX::AppGlobal::Services()->GetTreeViewSelectionModel()->selection();
		m_selection = SynGlyphX::AppGlobal::Services()->CreateTreeSelection();
	}
	void undo() override {
		SynGlyphX::AppGlobal::Services()->ApplyTreeSelection(*m_selection);
		if (m_ble) {
			m_ble->m_inputFieldId = m_oldInputField;
			m_ble->SetInputField(m_ble->m_inputFieldId); //call explicitely for consistency
			m_ble->ValueChangedByUser(m_ble->m_inputFieldId);
		}
	}
	void redo() override {
		if (!m_firstCall)
			SynGlyphX::AppGlobal::Services()->ApplyTreeSelection(*m_selection);
		else
			m_firstCall = false;

		if (m_ble) {
			m_ble->m_inputFieldId = m_newInputField;
			m_ble->SetInputField(m_ble->m_inputFieldId); //call explicitely for consistency
			m_ble->ValueChangedByUser(m_ble->m_inputFieldId);
		}
	}
	virtual ~BindingLineEditChangeCommand()
	{
		if (!m_selection)
			delete m_selection;
	}
	QPointer<BindingLineEdit> m_ble;
	QString m_newInputField;
	QString m_oldInputField;
	//QItemSelection m_selection;
	bool m_firstCall;
	TreeSelection* m_selection;

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

const QString& BindingLineEdit::GetInputField() const {

	return m_inputFieldId;
}

void BindingLineEdit::SetInputField(const QString& inputFieldID) {

	auto ifm = m_model->GetSourceModel()->GetInputFieldManager();
	m_inputFieldId = inputFieldID;
	
	if (m_inputFieldId.isEmpty()) {

		m_lineEdit->clear();
	}
	else {

		auto inputField = ifm->GetInputField(m_inputFieldId.toStdWString());
		SynGlyphX::Datasource::ConstSharedPtr datasource = m_model->GetDataTransformMapping()->GetDatasources().at(inputField.GetDatasourceID());

		QString text = QString::fromStdWString(datasource->GetFormattedName());

		if (datasource->CanDatasourceHaveMultipleTables()) {

			text += ":" + QString::fromStdWString(inputField.GetTable());
		}

		if (m_inputFieldId[0] == '~')
		{
			m_lineEdit->setText(text + ":" + QString::fromStdWString(inputField.GetField()));
		}
		else
		{
			m_lineEdit->setText(text + ":" + inputFieldID);
		}
	}

	m_clearAction->setEnabled(!m_inputFieldId.isEmpty());
	//m_clearAction->setEnabled(m_inputFieldId.GetInputField().IsValid());
	//m_useInputFieldMinMaxActon->setEnabled(!m_inputFieldId.isEmpty());
}

void BindingLineEdit::SetAcceptedInputTypes(SynGlyphX::MappingFunctionData::Input acceptedInputTypes) {

	m_acceptedInputTypes = acceptedInputTypes;

	//If input type has changed then clear input field if input field no longer matches input type
	auto ifm = m_model->GetSourceModel()->GetInputFieldManager();
	auto inputField = ifm->GetInputField(m_inputFieldId.toStdWString());
	if ((m_acceptedInputTypes == SynGlyphX::MappingFunctionData::Input::Numeric) && (inputField.IsValid()) && (!inputField.IsNumeric())) {

		Clear();
	}
	else if ((m_acceptedInputTypes == SynGlyphX::MappingFunctionData::Input::Text) && (inputField.IsValid()) && (inputField.IsNumeric())) {

		Clear();
	}
}

void BindingLineEdit::dragEnterEvent(QDragEnterEvent *event) {

	auto ifm = m_model->GetSourceModel()->GetInputFieldManager();
	SynGlyphX::InputField field;
	if (event->mimeData()->hasText())
	{
		QString fieldID = event->mimeData()->text();
		field = ifm->GetInputField(fieldID.toStdWString());
	}
	else
	{
		const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
		if (mimeData == nullptr) 
			return;
		field = mimeData->GetInputField();
	}


	if ((m_acceptedInputTypes == SynGlyphX::MappingFunctionData::Input::Numeric) && (!field.IsNumeric())) {
		
		return;
	}
	else if ((m_acceptedInputTypes == SynGlyphX::MappingFunctionData::Input::Text) && (field.IsNumeric())) {

		return;
	}

	if (!m_model->IsInputFieldCompatible(field)) {

		return;
	}
		
	event->acceptProposedAction();
}

void BindingLineEdit::dropEvent(QDropEvent* event) {

	auto ifm = m_model->GetSourceModel()->GetInputFieldManager();
	if (event->mimeData()->hasText())
	{
		QString fieldID = event->mimeData()->text();
		ifm->GetInputField(fieldID.toStdWString());
		auto command = new BindingLineEditChangeCommand(this, fieldID);
		command->setText(tr("Change Binding"));
		SynGlyphX::AppGlobal::Services()->GetUndoStack()->push(command);
		return;
	}
	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	
	std::wstring fieldID = ifm->GenerateInputFieldID(mimeData->GetInputField());
	ifm->SetInputField(fieldID, mimeData->GetInputField());
	if (mimeData != nullptr) {
		auto command = new BindingLineEditChangeCommand(this, QString::fromStdWString(fieldID)/*mimeData->GetInputField()*/);
		command->setText(tr("Change Binding"));
		SynGlyphX::AppGlobal::Services()->GetUndoStack()->push(command);
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

	if (!m_inputFieldId.isEmpty()) {

		auto command = new BindingLineEditChangeCommand(this, QString());
		command->setText(tr("Clear Binding"));
		SynGlyphX::AppGlobal::Services()->GetUndoStack()->push(command);		
	}
}
