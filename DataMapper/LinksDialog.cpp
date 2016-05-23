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

LinksDialog::LinksDialog(DataTransformModel* dataTransformModel, QWidget *parent)
	: QDialog(parent),
	m_dataTransformModel(dataTransformModel),
	m_row(-1)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	//mainLayout->addStretch(1);
	m_nameLineEdit = new QLineEdit(this);
	//m_nameLineEdit->setFixedWidth(100);
	QHBoxLayout* nameLayout = new QHBoxLayout;
	nameLayout->addWidget(new QLabel(tr("Link name:"), this));
	nameLayout->addWidget(m_nameLineEdit);
	mainLayout->addLayout(nameLayout);
	//nameLayout->setAlignment(m_nameLineEdit, Qt::AlignLeft);

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

void LinksDialog::SetEditRow(int row) {

	m_row = row;
	if (m_row >= 0)
		SetLink(m_dataTransformModel->GetDataMapping()->GetLinks()[row]);
}

const SynGlyphX::Link& LinksDialog::GetLink() {
	// 
	//m_link.m_function = SynGlyphX::LinkFunction::MatchValue;
	m_link.m_start = GetNode(m_fromGlyphTree, m_fromLineEdit);
	m_link.m_end = GetNode(m_toGlyphTree, m_toLineEdit);
	m_link.m_name = m_nameLineEdit->text().toStdWString();
	QColor c = m_colorButton->GetColor();
	m_link.m_color.SetRGB(c.red(), c.green(), c.blue());
	m_link.m_color.m_alpha = m_transparensySpinBox->value();
	m_link.m_color.m_inheritfromParent = m_inheritColorCheckBox->isChecked();
	return m_link;
}

void LinksDialog::SetLink(const SynGlyphX::Link& link) {
	
	m_nameLineEdit->setText(QString::fromStdWString(link.m_name));
	m_colorButton->SetColor(QColor(link.m_color.m_r, link.m_color.m_g, link.m_color.m_b));
	m_transparensySpinBox->setValue(link.m_color.m_alpha);
	m_inheritColorCheckBox->setChecked(m_link.m_color.m_inheritfromParent);
	SetNode(link.m_start, m_fromGlyphTree, m_fromLineEdit);
	SetNode(link.m_end, m_toGlyphTree, m_toLineEdit);

}

SynGlyphX::Link::Node LinksDialog::GetNode(GlyphTreesView* treeView, LinkLineEdit* lineEdit) {

	QModelIndexList selectedItems = treeView->selectionModel()->selectedIndexes();
	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(treeView->model());

	QModelIndex sourceIndex = filterModel->mapToSource(selectedItems[0]);
	SynGlyphX::DataMappingGlyphGraph::Node* treeNode = static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(sourceIndex.internalPointer());

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator fromGlyph(treeNode);
	SynGlyphX::Link::Node node(m_dataTransformModel->GetTreeId(sourceIndex), fromGlyph->first, lineEdit->GetInputField().GetHashID());
	return node;
}

void LinksDialog::Clear(){
	m_nameLineEdit->clear();
	m_fromLineEdit->clear();
	m_toLineEdit->clear();

}

void LinksDialog::SelectGlyph(const QModelIndex &parent, GlyphTreesView* treeView, const SynGlyphX::Link::Node& node){

	int rowCount = m_dataTransformModel->rowCount(parent);
	for (int i = 0; i < rowCount; ++i) {
		QModelIndex idx = m_dataTransformModel->index(i, 0, parent);

		if (idx.isValid()) {
			SynGlyphX::DataMappingGlyphGraph::Node* treeNode = static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(idx.internalPointer());
			if (!treeNode)
				break;
			SynGlyphX::DataMappingGlyphGraph::GlyphIterator fromGlyph(treeNode);
			if (m_dataTransformModel->GetTreeId(idx) == node.m_treeId && fromGlyph->first == node.m_label) {
				SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(treeView->model());
				treeView->selectionModel()->select(filterModel->mapFromSource(idx), QItemSelectionModel::Select);
				break;
			}
			else {
				SelectGlyph(idx, treeView, node);
			}
		}
	}
}

void LinksDialog::SetNode(const SynGlyphX::Link::Node& node, GlyphTreesView* treeView, LinkLineEdit* lineEdit) {

	const auto& glyphGraph = m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().at(node.m_treeId);
	const SynGlyphX::DataMappingGlyphGraph::InputFieldMap& inputFields = glyphGraph->GetInputFields();
	for (const auto& inputField : inputFields) {
		if (inputField.second.GetHashID() == node.m_inputFieldId) {
			lineEdit->SetInputField(inputField.second);
			break;
		}
	}
	int nRows = m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().size();
	
	SelectGlyph(QModelIndex(), treeView, node);
	//for (const auto& glyph : glyphGraphs) {
	//	if (glyph.second-> == node.m_label) 

	//}
	//	selectionModel()->select(model()->index(m_sourceModel->GetDataMapping()->GetGlyphGraphs().size() - 1, 0), QItemSelectionModel::ClearAndSelect);
	//treeView->selectionModel()->select
}

bool LinksDialog::Validate() {

	if (m_nameLineEdit->text().isEmpty()){
		QMessageBox::warning(this, tr("Add Link Error"), tr("Link name is empty. Please enter link name"));
		return false;
	}
	QModelIndexList s1 = m_fromGlyphTree->selectionModel()->selectedIndexes();
	QModelIndexList s2 = m_toGlyphTree->selectionModel()->selectedIndexes();
	if (s1.size() != 1 || s2.size() != 1) {
		QMessageBox::warning(this, tr("Add Link Error"), tr("One glyph must be slected for each side of the link"));
		return false;
	}
	if (m_fromLineEdit->text().isEmpty() || m_fromLineEdit->text().isEmpty()) {
		QMessageBox::warning(this, tr("Add Link Error"), tr("Input field is empty. Please drag and drop input field from the source data"));
		return false;
	}

	return true;
}

LinksDialog::~LinksDialog() {
	delete m_fromGlyphTree;
	delete m_toGlyphTree;
}

void LinksDialog::accept() {
	if (Validate())
		QDialog::accept();
}
