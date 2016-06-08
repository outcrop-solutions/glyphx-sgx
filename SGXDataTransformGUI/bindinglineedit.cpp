#include "bindinglineedit.h"
#include <QtGui/QDragEnterEvent>
#include <QtCore/QMimeData>
#include <QtWidgets/QMenu>
#include "inputfieldmimedata.h"
#include <QtWidgets/QHBoxLayout>
#include "datatransformmapping.h"

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

const SynGlyphX::InputField& BindingLineEdit::GetInputField() const {

	return m_inputField;
}

void BindingLineEdit::SetInputField(const SynGlyphX::InputField& inputfield) {

	m_inputField = inputfield;
	if (m_inputField.IsValid()) {

		SynGlyphX::Datasource::ConstSharedPtr datasource = m_model->GetDataTransformMapping()->GetDatasources().at(inputfield.GetDatasourceID());

		QString text = QString::fromStdWString(datasource->GetFormattedName());
		if (datasource->CanDatasourceHaveMultipleTables()) {
		
			text += ":" + QString::fromStdWString(inputfield.GetTable());
		}
		text += ":" + QString::fromStdWString(inputfield.GetField());
		m_lineEdit->setText(text);
	}
	else {

		m_lineEdit->clear();
	}

	m_clearAction->setEnabled(m_inputField.IsValid());
	//m_useInputFieldMinMaxActon->setEnabled(m_inputField.IsValid());
}

void BindingLineEdit::SetAcceptedInputTypes(SynGlyphX::MappingFunctionData::Input acceptedInputTypes) {

	m_acceptedInputTypes = acceptedInputTypes;

	//If input type has changed then clear input field if input field no longer matches input type
	if ((m_acceptedInputTypes == SynGlyphX::MappingFunctionData::Input::Numeric) && (m_inputField.IsValid()) && (!m_inputField.IsNumeric())) {

		Clear();
	}
	else if ((m_acceptedInputTypes == SynGlyphX::MappingFunctionData::Input::Text) && (m_inputField.IsValid()) && (m_inputField.IsNumeric())) {

		Clear();
	}
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

	if (m_inputField.IsValid()) {

		m_inputField = SynGlyphX::InputField();
		emit ValueChangedByUser(m_inputField);
	}
}
