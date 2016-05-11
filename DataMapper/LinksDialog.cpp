#include "LinksDialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGroupBox>
#include <QtGui/QDragEnterEvent>
#include "roledatafilterproxymodel.h"

LinkLineEdit::LinkLineEdit(DataTransformModel* dataTransformModel, QWidget *parent) : QLineEdit(parent),
m_dataTransformModel(dataTransformModel)
{
	setContextMenuPolicy(Qt::NoContextMenu);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setReadOnly(true);	
}

void LinkLineEdit::SetInputField(const SynGlyphX::InputField& inputField) {
	m_inputField = inputField;
	if (m_inputField.IsValid()) {

		SynGlyphX::Datasource::ConstSharedPtr datasource = m_dataTransformModel->GetDataMapping()->GetDatasources().at(inputField.GetDatasourceID());

		QString text = QString::fromStdWString(datasource->GetFormattedName());
		if (datasource->CanDatasourceHaveMultipleTables()) {

			text += ":" + QString::fromStdWString(inputField.GetTable());
		}
		text += ":" + QString::fromStdWString(inputField.GetField());
		setText(text);
	}
	else {

		clear();
	}
}
void LinkLineEdit::dragEnterEvent(QDragEnterEvent *event) {

	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData == nullptr) {

		return;
	}

	event->acceptProposedAction();
}

void LinkLineEdit::dropEvent(QDropEvent* event) {
	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData != nullptr) {
		SetInputField(mimeData->GetInputField());
	}
}

LinksDialog::LinksDialog(DataTransformModel* dataTransformModel, GlyphRolesTableModel* glyphRolesTableModel, QWidget *parent)
	: QDialog(parent),
	m_dataTransformModel(dataTransformModel)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	//mainLayout->addStretch(1);

	QGroupBox* glyphGroupBox = new QGroupBox(tr("Link Glyphs"), this);
	//QHBoxLayout* glyphLayout = new QHBoxLayout(glyphGroupBox);
	QGridLayout* glyphLayout = new QGridLayout(glyphGroupBox);
	glyphLayout->addWidget(new QLabel(tr("From:"),this), 0, 0);
	glyphLayout->addWidget(new QLabel(tr("To:"), this), 0, 1);

	

	m_fromGlyphTree = new GlyphTreesView(m_dataTransformModel, glyphGroupBox);
	m_toGlyphTree = new GlyphTreesView(m_dataTransformModel, glyphGroupBox);

	glyphLayout->addWidget(m_fromGlyphTree, 1, 0);
	glyphLayout->addWidget(m_toGlyphTree, 1, 1);

	glyphLayout->addWidget(new QLabel(tr("Input:"), this), 2, 0);
	glyphLayout->addWidget(new QLabel(tr("Input:"), this), 2, 1);
	m_fromLineEdit = new LinkLineEdit(m_dataTransformModel, this);
	glyphLayout->addWidget(m_fromLineEdit, 3, 0);

	m_toLineEdit = new LinkLineEdit(m_dataTransformModel, this);
	glyphLayout->addWidget(m_toLineEdit, 3, 1);

	mainLayout->addWidget(glyphGroupBox);

	QGroupBox* colorGroupBox = new QGroupBox(tr("Link color"), this);
	QHBoxLayout* colorLayout = new QHBoxLayout(colorGroupBox);

	m_colorButton = new SynGlyphX::ColorButton(this);
	m_colorButton->SetColor(Qt::blue);
	colorLayout->addWidget(m_colorButton);
	colorLayout->addSpacing(20);
	QLabel* tl = new QLabel(tr("Transparency:"), this);
	tl->setFixedWidth(75);
	colorLayout->addWidget(tl);
	colorLayout->setSpacing(1);
	m_transparensySpinBox = new QSpinBox(this);
	m_transparensySpinBox->setRange(0, 255);
	m_transparensySpinBox->setValue(128);
	m_transparensySpinBox->setFixedWidth(50);
	colorLayout->addWidget(m_transparensySpinBox);
	colorLayout->addSpacing(20);
	m_inheritColorCheckBox = new QCheckBox(tr("Inherit from parent"), this);
	colorLayout->addWidget(m_inheritColorCheckBox);


	mainLayout->addWidget(colorGroupBox);
	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);	
	mainLayout->addWidget(dialogButtonBox);

	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &LinksDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &LinksDialog::reject);

	setLayout(mainLayout);
}

const SynGlyphX::Link& LinksDialog::GetLink() {
	// 
	//m_link.m_function = SynGlyphX::LinkFunction::MatchValue;
	m_link.m_start = GetNode(m_fromGlyphTree, m_fromLineEdit);
	m_link.m_end = GetNode(m_toGlyphTree, m_toLineEdit);
	return m_link;
}

SynGlyphX::Link::Node LinksDialog::GetNode(GlyphTreesView* treeView, LinkLineEdit* lineEdit) {

	QModelIndexList selectedItems = treeView->selectionModel()->selectedIndexes();
	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(treeView->model());

	QModelIndex sourceIndex = filterModel->mapToSource(selectedItems[0]);
	SynGlyphX::DataMappingGlyphGraph::Node* treeNode = static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(sourceIndex.internalPointer());

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator fromGlyph(treeNode);
	SynGlyphX::Link::Node node(m_dataTransformModel->GetTreeId(sourceIndex), fromGlyph->first, lineEdit->GetInputField());
	return node;
}

LinksDialog::~LinksDialog() {
	delete m_fromGlyphTree;
	delete m_toGlyphTree;
}

void LinksDialog::accept() {

	QDialog::accept();
}

