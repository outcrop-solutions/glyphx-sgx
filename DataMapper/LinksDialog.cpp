#include "LinksDialog.h"
#include "Link.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGroupBox>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include "roledatafilterproxymodel.h"
#include "colorbutton.h"
#include "glyphtreesview.h"
#include "inputfieldmimedata.h"
#include "inputfield.h"
#include "datatransformmodel.h"

static void print_tree(boost::property_tree::wptree const& pt)
{
#ifdef WIN32
	using boost::property_tree::wptree;
	wptree::const_iterator end = pt.end();
	for (wptree::const_iterator it = pt.begin(); it != end; ++it) {
		std::wstringstream ss;
		ss << it->first << ": " << it->second.get_value<std::wstring>() << std::endl;
		OutputDebugStringW(ss.str().c_str());
		print_tree(it->second);
	}
#endif
}

class LinkLineEdit : public QLineEdit
{
public:
	LinkLineEdit(SynGlyphX::DataTransformModel* dataTransformModel, QWidget *parent = 0);
	virtual ~LinkLineEdit() {}
	const SynGlyphX::InputField& GetInputField() const { return m_inputField; }
	void SetInputField(const SynGlyphX::InputField& inputField);
protected:
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);
private:
	SynGlyphX::InputField m_inputField;
	SynGlyphX::DataTransformModel* m_dataTransformModel;
};


class FunctionDialog : public QDialog {
	Q_OBJECT
	enum class Type //matches combobox order
	{
		MatchValue = 0, //should not launch this dialog
		KeyToValue = 1,
		KeyToRange = 2,
		//////////
		NFunctions
	};
public:
	FunctionDialog(const boost::property_tree::wptree& propertyTree, LinksDialog *parent) :
		QDialog(parent),
		m_table(nullptr)
	{
		//print_tree(propertyTree);
		m_type = (Type)parent->m_functionComboBox->currentIndex();

		setWindowTitle(tr("Edit Function Properties"));
		//QHBoxLayout* defaultLayout = new QHBoxLayout(this);
		m_table = new QTableWidget(this);
		if (m_type == Type::KeyToValue)
			m_table->setColumnCount(2);
		else if (m_type == Type::KeyToRange)
			m_table->setColumnCount(3);

		QVBoxLayout* layout = new QVBoxLayout(this);
		//layout->addLayout(defaultLayout);
		QStringList headers;
		if (m_type == Type::KeyToValue)
			headers << "Key" << "Value";
		else if (m_type == Type::KeyToRange)
			headers << "Key" << "Min" << "Max";
		m_table->setHorizontalHeaderLabels(headers);
		m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
		m_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
		m_table->setSelectionMode(QAbstractItemView::SingleSelection);
		m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_table->sortByColumn(0, Qt::SortOrder::AscendingOrder);
		m_table->setSortingEnabled(true);
		QObject::connect(m_table, &QTableWidget::itemSelectionChanged, this, &FunctionDialog::OnTableSelectionChanged);
		layout->addWidget(m_table);

		QHBoxLayout* buttonLayout = new QHBoxLayout(this);

		m_removeEntryButton = new QPushButton(tr("Remove"), this);
		QObject::connect(m_removeEntryButton, &QPushButton::clicked, this, &FunctionDialog::OnRemoveEntry);
		buttonLayout->addWidget(m_removeEntryButton);

		//QPushButton* clearAllButton = new QPushButton(tr("ClearAll"), this);
		//QObject::connect(clearAllButton, &QPushButton::clicked, this, &FunctionDialog::OnClearAllKeyValues);
		//buttonLayout->addWidget(clearAllButton);
		buttonLayout->addStretch(1);
		layout->addLayout(buttonLayout);

		QString groupBoxName;
		if (m_type == Type::KeyToValue) {

			groupBoxName = tr("New Key to Value entry");
		}
		else {

			groupBoxName = tr("New Key to Range entry");
		}
		QGroupBox* inputGroupBox = new QGroupBox(groupBoxName, this);
		QHBoxLayout* inputLayout = new QHBoxLayout(inputGroupBox);

		inputLayout->addWidget(new QLabel(tr("Key:")));
		m_keyEdit = new QLineEdit(this);
		inputLayout->addWidget(m_keyEdit);
		m_valueEdit = new QLineEdit(this);
		if (m_type == Type::KeyToValue) {
			inputLayout->addWidget(new QLabel(tr("Value:")));
			inputLayout->addWidget(m_valueEdit);
		}
		else {
			inputLayout->addWidget(new QLabel(tr("Range:")));
			inputLayout->addWidget(m_valueEdit);
			m_rangeEdit = new QLineEdit(this);
			inputLayout->addWidget(m_rangeEdit);
		}

		QPushButton* addEntryButton = new QPushButton(tr("Add"), this);
		QObject::connect(addEntryButton, &QPushButton::clicked, this, &FunctionDialog::OnAddEntry);
		inputLayout->addWidget(addEntryButton);

		inputGroupBox->setLayout(inputLayout);
		layout->addWidget(inputGroupBox);

		QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
		layout->addWidget(dialogButtonBox);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &FunctionDialog::accept);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &FunctionDialog::reject);

		setLayout(layout);
		auto type = propertyTree.get_optional<std::wstring>(L"<xmlattr>.type");
		if (type.is_initialized() && type == parent->m_functionComboBox->currentText().toStdWString())
			(PopulateFromTree(propertyTree));
	}

	Type m_type;
	QPushButton*  m_removeEntryButton;
	QLineEdit* m_keyEdit;
	QLineEdit* m_valueEdit;
	QLineEdit* m_rangeEdit;
	QTableWidget* m_table;
signals:
	public slots :
		void OnTableSelectionChanged() {

	}
	void OnRemoveEntry() {
		int row = m_table->currentRow();
		m_table->removeRow(row);
	}
	void OnAddEntry() {
		AddRow();
		int row = m_table->rowCount() - 1;
		dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 0))->setText(m_keyEdit->text());
		dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 1))->setText(m_valueEdit->text());
		if (m_type == Type::KeyToRange)
			dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 2))->setText(m_rangeEdit->text());
	}
	void AddRow() {

		int row = m_table->rowCount();
		m_table->insertRow(row);
		for (int i = 0; i < m_table->columnCount(); ++i){
			QWidget* inputWidget = new QLineEdit(this);

			inputWidget->setContentsMargins(0, 0, 0, 0);
			m_table->setCellWidget(row, i, inputWidget);
		}
	}

	void GetPropertyTree(boost::property_tree::wptree* tree) {
		tree->clear();
		for (int row = 0; row < m_table->rowCount(); ++row) {
			QString key = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 0))->text();
			QString value = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 1))->text();
			boost::property_tree::wptree& entry = tree->add(L"Pair", L"");
			entry.put(L"<xmlattr>.key", key.toStdWString());
			if (m_type == Type::KeyToValue)
				entry.put(L"<xmlattr>.value", value.toStdWString());
			else if (m_type == Type::KeyToRange) {
				entry.put(L"<xmlattr>.min", value.toStdWString());
				QString range = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 2))->text();
				entry.put(L"<xmlattr>.max", range.toStdWString());
			}
		}
	}
	void PopulateFromTree(const boost::property_tree::wptree& propertyTree) {
		//print_tree(propertyTree);
		for (const auto& func : propertyTree) {
			if (func.first == L"Pair") {

				AddRow();
				int row = m_table->rowCount() - 1;

				dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 0))->setText(QString::fromStdWString(func.second.get<std::wstring>(L"<xmlattr>.key")));

				if (m_type == Type::KeyToValue) {
					dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 1))->setText(QString::fromStdWString(func.second.get<std::wstring>(L"<xmlattr>.value")));
				}
				else if (m_type == Type::KeyToRange) {
					dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 1))->setText(QString::fromStdWString(func.second.get<std::wstring>(L"<xmlattr>.min")));
					dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 2))->setText(QString::fromStdWString(func.second.get<std::wstring>(L"<xmlattr>.max")));
				}
			}
		}
	}
	bool Validate() {
		if (m_table->rowCount() == 0) {
			QMessageBox::warning(this, tr("Function properties Error"), tr("Function must have at least one Key"));
			return false;
		}
		if (m_type == Type::KeyToRange) {

			for (int row = 0; row < m_table->rowCount(); ++row) {
				bool okMin, okMax;
				double min = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 1))->text().toDouble(&okMin);
				double max = dynamic_cast<QLineEdit*>(m_table->cellWidget(row, 2))->text().toDouble(&okMax);
				if (!(okMin && okMax && min <= max)) {
					QMessageBox::warning(this, tr("Row %n Error", 0, row), tr("Min and max must be numbers, with min less or equal max"));
					return false;
				}
			}
		}

		return true;

	}
	virtual void accept() {
		if (Validate())
			return QDialog::accept();
	}
};

LinkLineEdit::LinkLineEdit(SynGlyphX::DataTransformModel* dataTransformModel, QWidget *parent) : QLineEdit(parent),
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

LinksDialog::LinksDialog(SynGlyphX::DataTransformModel* dataTransformModel, QWidget *parent)
	: QDialog(parent),
	m_dataTransformModel(dataTransformModel),
	m_link(new SynGlyphX::Link),
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

	QGroupBox* functionGroupBox = new QGroupBox(tr("Function"), this);
	QHBoxLayout* functionLayout = new QHBoxLayout(functionGroupBox);
	
	m_functionComboBox = new QComboBox(this);

	//TODO get available types from Link class rather then hardcode strings
	QStringList functions;
	functions << "Match Value" << "Key to Value" << "Key to Range";
	m_functionComboBox->addItems(functions);

	m_functionPushButton = new QPushButton(tr("Properties"), this);
	m_functionPushButton->setDisabled(true);
	functionLayout->addWidget(m_functionComboBox);
	functionLayout->addWidget(m_functionPushButton);

	QObject::connect(m_functionComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
		if (index == 0)
			m_functionPushButton->setDisabled(true);
		else
			m_functionPushButton->setDisabled(false);
		// clear function properties on change. Maybe we need worning dialog here, but it will be annoying 
		m_link->m_function.m_propertyTree.clear();
	});
	QObject::connect(m_functionPushButton, &QPushButton::clicked, this, &LinksDialog::OnFunctionProperties);

	mainLayout->addWidget(functionGroupBox);


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
	//m_link->m_function = SynGlyphX::LinkFunction::MatchValue;
	m_link->m_start = GetNode(m_fromGlyphTree, m_fromLineEdit);
	m_link->m_end = GetNode(m_toGlyphTree, m_toLineEdit);
	m_link->m_name = m_nameLineEdit->text().toStdWString();
	QColor c = m_colorButton->GetColor();
	m_link->m_color.SetRGB(c.red(), c.green(), c.blue());
	m_link->m_color.m_alpha = m_transparensySpinBox->value();
	m_link->m_color.m_inheritfromParent = m_inheritColorCheckBox->isChecked();
	m_link->m_function.m_propertyTree.put(L"<xmlattr>.type", m_functionComboBox->currentText().toStdWString());
	return *m_link;
}

void LinksDialog::SetLink(const SynGlyphX::Link& link) {

	m_nameLineEdit->setText(QString::fromStdWString(link.m_name));
	m_colorButton->SetColor(QColor(link.m_color.m_r, link.m_color.m_g, link.m_color.m_b));
	m_transparensySpinBox->setValue(link.m_color.m_alpha);
	m_inheritColorCheckBox->setChecked(m_link->m_color.m_inheritfromParent);
	SetNode(link.m_start, m_fromGlyphTree, m_fromLineEdit);
	SetNode(link.m_end, m_toGlyphTree, m_toLineEdit);	
	const auto& type = link.m_function.m_propertyTree.get_optional<std::wstring>(L"<xmlattr>.type");
	if (type.is_initialized()) {
		QString typeStr = QString::fromStdWString(type.get());
		m_functionComboBox->setCurrentText(QString::fromStdWString(type.get()));
	}
	m_link->m_function = link.m_function;
	//print_tree(link.m_function.m_propertyTree);

}

void LinksDialog::Clear(){
	m_nameLineEdit->clear();
	m_fromLineEdit->clear();
	m_toLineEdit->clear();

}

SynGlyphX::LinkNode LinksDialog::GetNode(GlyphTreesView* treeView, LinkLineEdit* lineEdit) {

	QModelIndexList selectedItems = treeView->selectionModel()->selectedIndexes();
	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(treeView->model());

	QModelIndex sourceIndex = filterModel->mapToSource(selectedItems[0]);
	SynGlyphX::DataMappingGlyphGraph::Node* treeNode = static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(sourceIndex.internalPointer());

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator fromGlyph(treeNode);
	SynGlyphX::LinkNode node(m_dataTransformModel->GetTreeId(sourceIndex), fromGlyph->first, lineEdit->GetInputField().GetHashID());
	return node;
}


void LinksDialog::SelectGlyph(const QModelIndex &parent, GlyphTreesView* treeView, const SynGlyphX::LinkNode& node){

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

void LinksDialog::SetNode(const SynGlyphX::LinkNode& node, GlyphTreesView* treeView, LinkLineEdit* lineEdit) {

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

void LinksDialog::OnFunctionProperties() {
	//print_tree(m_link->m_function.m_propertyTree);
	m_link->m_function.m_propertyTree.put(L"<xmlattr>.type", m_functionComboBox->currentText().toStdWString());
	FunctionDialog dialog(m_link->m_function.m_propertyTree, this);
	if (dialog.exec() == QDialog::Accepted) {
		dialog.GetPropertyTree(&m_link->m_function.m_propertyTree);
	}

}

LinksDialog::~LinksDialog() {
	delete m_fromGlyphTree;
	delete m_toGlyphTree;
	delete m_link;
}

void LinksDialog::accept() {
	if (Validate())
		QDialog::accept();
}

#include "linksdialog.moc" //this is nesseasary to because there is no .h file for FunctionDialog
