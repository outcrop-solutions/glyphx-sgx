#include "antzsingleglyphtreemodel.h"
#include "npdata.h"
#include "data/npmapfile.h"
#include "glyphmimedata.h"
#include "minmaxglyphtree.h"
#include <QtCore/QFile>

ANTzSingleGlyphTreeWidget::ANTzSingleGlyphTreeWidget(MinMaxGlyphTreeType glyphTreeType, QWidget *parent)
    : ANTzWidget(parent),
	m_glyphTreeType(glyphTreeType),
    m_clipboardGlyph(),
	m_rootGlyph(nullptr),
	m_model(nullptr),
	m_selectionModel(nullptr),
	m_editingMode(Move),
	m_selectionEdited(false),
	m_allowMultiSelection(false)
{
    
    
}

ANTzSingleGlyphTreeWidget::~ANTzSingleGlyphTreeWidget()
{
    npCloseData();
}

void ANTzSingleGlyphTreeWidget::SetAllowMultiSelection(bool allowMultiSelection) {

	m_allowMultiSelection = allowMultiSelection;
}

void ANTzSingleGlyphTreeWidget::SetModel(MinMaxGlyphTreeModel* model) {

	m_model = model;

	if (m_model != nullptr) {

		m_selectionModel = new QItemSelectionModel(m_model, this);

		QObject::connect(m_model, &MinMaxGlyphTreeModel::GlyphPropertiesUpdated, this, &ANTzSingleGlyphTreeWidget::MarkDifferentNotifyModelUpdate);
		QObject::connect(m_model, &MinMaxGlyphTreeModel::rowsInserted, this, &ANTzSingleGlyphTreeWidget::MarkDifferentNotifyModelUpdate);
		QObject::connect(m_model, &MinMaxGlyphTreeModel::rowsMoved, this, &ANTzSingleGlyphTreeWidget::MarkDifferentNotifyModelUpdate);
		QObject::connect(m_model, &MinMaxGlyphTreeModel::rowsRemoved, this, &ANTzSingleGlyphTreeWidget::MarkDifferentNotifyModelUpdate);
		QObject::connect(m_model, &MinMaxGlyphTreeModel::modelReset, this, &ANTzSingleGlyphTreeWidget::OnModelReset);
	}
}

void ANTzSingleGlyphTreeWidget::BeforeDrawScene() {


}

void ANTzSingleGlyphTreeWidget::AfterDrawScene() {

	//since changes from editing don't happen until drawing emit the ObjectEdited signal here
	if (m_selectionEdited) {
		const QModelIndexList& selected = m_selectionModel->selectedIndexes();
		if (!selected.isEmpty()) {

			emit ObjectEdited(selected.back());
		}
		m_selectionEdited = false;
	}
}

bool ANTzSingleGlyphTreeWidget::LoadFromFile(const QString& filename) {
    
    beginResetModel();
	DeleteGlyphRootNode();

	bool success = false;
	if (IsANTzCSVFile(filename)) {

		success = (npFileOpenMap(filename.toStdString().c_str(), 1, filename.length(), m_antzData) != 0);
	} 
	else {

		SynGlyphX::MinMaxGlyphTree::SharedPtr newGlyphTree(new SynGlyphX::MinMaxGlyphTree());
		newGlyphTree->ReadFromFile(filename.toStdString());
		SynGlyphX::GlyphTree::ConstSharedPtr maxTree = newGlyphTree->GetMaxGlyphTree();
		CreateNewSubTree(NULL, maxTree, maxTree->root(), true);
		success = true;
	}

    if (success) {

        m_rootGlyph = static_cast<pNPnode>(m_antzData->map.node[m_antzData->map.nodeRootCount - 1]);
		m_rootGlyph->translate.x = 0;
		m_rootGlyph->translate.y = 0;
		m_rootGlyph->translate.z = 0;
    }
    else {
        CreateDefaultGlyph(false);
    }
	m_isDifferentFromSavedFileOrDefaultGlyph = false;

    //Undo previous select node at the beginning of this function
    m_antzData->map.nodeRootIndex = 0;

    endResetModel();

    return success;
}

void ANTzSingleGlyphTreeWidget::SaveToTemplateFile(const QString& filename) const {

	SynGlyphX::GlyphTree glyphTree(m_rootGlyph);
	SynGlyphX::MinMaxGlyphTree minMaxGlyphTree(glyphTree);
	SynGlyphX::MinMaxGlyphTree::iterator iT = minMaxGlyphTree.root();
	SynGlyphX::GlyphProperties minGlyph = iT->GetMinGlyph();
	SynGlyphX::GlyphNumericMappableProperties difference = iT->GetDifference();
	minGlyph.SetPosition({ { -180.0, -90.0, 0.0 } });
	difference.SetPosition({ { 360.0, 180.0, 50.0 } });
	iT->SetMinGlyphProperties(minGlyph);
	iT->SetDifference(difference);

	minMaxGlyphTree.WriteToFile(filename.toStdString());
}

bool ANTzSingleGlyphTreeWidget::SaveToCSV(const std::string& filename, const QModelIndexList& selectedItems) {

    //need to unselect node(s) for saving since selection will get saved with the CSV
    for (int i = 0; i < selectedItems.length(); ++i) {
        pNPnode selectedNode = static_cast<pNPnode>(selectedItems[i].internalPointer());
        selectedNode->selected = false;
    }

    bool success = (npFileSaveMap(filename.c_str(), 1, filename.length(), m_antzData) != 0);

    //reselect the previously selected node(s)
    for (int i = 0; i < selectedItems.length(); ++i) {
        pNPnode selectedNode = static_cast<pNPnode>(selectedItems[i].internalPointer());
        selectedNode->selected = true;
    }
	
	if (success) {
		NotifyModelUpdate();
	}
    
    return success;
}

void ANTzSingleGlyphTreeWidget::RebuildTree() {

	if (m_model != nullptr) {

		DeleteGlyphRootNode();
		CreateNewSubTree(nullptr, m_model->GetMinMaxGlyphTree()->root());

		//Undo previous select node at the beginning of this function
		m_antzData->map.nodeRootIndex = 0;
	}
}

void ANTzSingleGlyphTreeWidget::CreateNewSubTree(pNPnode parent, const SynGlyphX::MinMaxGlyphTree::const_iterator& minMaxGlyph) {

	pNPnode childNode = CreateNodeFromTemplate(parent, minMaxGlyph);

	for (int i = 0; i < m_model->GetMinMaxGlyphTree()->children(minMaxGlyph); ++i) {

		CreateNewSubTree(childNode, m_model->GetMinMaxGlyphTree()->child(minMaxGlyph, i));
    }
}

pNPnode ANTzSingleGlyphTreeWidget::CreateNodeFromTemplate(pNPnode parent, const SynGlyphX::MinMaxGlyphTree::const_iterator& minMaxGlyph) {

    pNPnode glyph = npNodeNew(kNodePin, parent, m_antzData);
    if (parent == nullptr) {

        m_rootGlyph = glyph;
    }

    glyph->selected = 0;

	UpdateGlyphProperties(glyph, minMaxGlyph);

    return glyph;
}

void ANTzSingleGlyphTreeWidget::OnModelReset() {

	DeleteGlyphRootNode();
	RebuildTree();

	//Undo previous select node at the beginning of this function
	m_antzData->map.nodeRootIndex = 0;
}

void ANTzSingleGlyphTreeWidget::UpdateGlyphProperties(pNPnode glyph, const SynGlyphX::MinMaxGlyphTree::const_iterator& minMaxGlyph) {

	if (m_glyphTreeType == MinMaxGlyphTreeType::Min) {

		UpdateGlyphProperties(glyph, minMaxGlyph->GetMinGlyph());
	}
	else {

		UpdateGlyphProperties(glyph, minMaxGlyph->GetMaxGlyph());
	}
}

void ANTzSingleGlyphTreeWidget::UpdateGlyphProperties(pNPnode glyph, const SynGlyphX::GlyphProperties& glyphTemplate) {

    SynGlyphX::Vector3 scale = glyphTemplate.GetScale();
    glyph->scale.x = scale[0];
    glyph->scale.y = scale[1];
    glyph->scale.z = scale[2];

    glyph->ratio = glyphTemplate.GetRatio();

	SynGlyphX::Vector3 translate = glyphTemplate.GetPosition();
    glyph->translate.x = translate[0];
    glyph->translate.y = translate[1];
    glyph->translate.z = translate[2];

	SynGlyphX::Vector3 rotation = glyphTemplate.GetRotation();
    glyph->rotate.x = rotation[0];
    glyph->rotate.y = rotation[1];
    glyph->rotate.z = rotation[2];

	glyph->geometry = 2 * glyphTemplate.GetShape();

    //This is necessary because ANTz screwed up the enum for geometries
	if (glyphTemplate.GetShape() == SynGlyphX::GlyphProperties::Shape::Pin) {
        glyph->geometry += (1 - glyphTemplate.GetSurface());
    }
    else {
        glyph->geometry += glyphTemplate.GetSurface();
    }

    SynGlyphX::Color color = glyphTemplate.GetColor();
    glyph->color.r = color[0];
    glyph->color.g = color[1];
    glyph->color.b = color[2];
    glyph->color.a = color[3];

    glyph->topo = glyphTemplate.GetTopology();
}

QModelIndex ANTzSingleGlyphTreeModel::IndexFromANTzID(int id) {

    pNPnode node = static_cast<pNPnode>(m_antzData->map.nodeID[id]);

    if (node == m_rootGlyph) {
        return createIndex(0, 0, node);
    }
    else {
        return createIndex(GetChildIndexFromParent(node), 0, node);
    }
}

int ANTzSingleGlyphTreeModel::GetChildIndexFromParent(pNPnode node) const {
    
    pNPnode parent = node->parent;
    int i = 0;
    for (; i < parent->childCount; ++i) {
        if (parent->child[i] == node) {
            break;
        }
    }
    
    return i;
}

void ANTzSingleGlyphTreeModel::AppendChild(const QModelIndex& parent, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph, unsigned int numberOfChildren) {

    if (parent.isValid()) {
        pNPnode parentNode = static_cast<pNPnode>(parent.internalPointer());
        beginInsertRows(parent, parentNode->childCount, parentNode->childCount + numberOfChildren - 1);
        for (int i = 0; i < numberOfChildren; ++i) {
            CreateNodeFromTemplate(parentNode, *glyph, false);
        }
        endInsertRows();
        emit NodeUpdated(parent);
    }
}

ANTzSingleGlyphTreeModel::PropertyUpdates ANTzSingleGlyphTreeModel::FindUpdates(boost::shared_ptr<const SynGlyphX::GlyphProperties> oldGlyph, boost::shared_ptr<const SynGlyphX::GlyphProperties> newGlyph) {

    PropertyUpdates updates = UpdateNone;

    if (oldGlyph->GetColor() != newGlyph->GetColor()) {
        updates |= UpdateColor;
    }

    if (oldGlyph->GetShape() != newGlyph->GetShape()) {
        updates |= UpdateGeometry;
    }

    if (oldGlyph->GetTopology() != newGlyph->GetTopology()) {
        updates |= UpdateTopology;
    }

    if (oldGlyph->GetSurface() != newGlyph->GetSurface()) {
        updates |= UpdateSurface;
    }

    if (oldGlyph->GetPosition() != newGlyph->GetPosition()) {
        updates |= UpdatePosition;
    }

	if (oldGlyph->GetRotation() != newGlyph->GetRotation()) {
        updates |= UpdateRotation;
    }

    if ((oldGlyph->GetScale() != newGlyph->GetScale()) || (oldGlyph->GetRatio() != newGlyph->GetRatio())) {
        updates |= UpdateScale;
    }

    return updates;
}

bool ANTzSingleGlyphTreeModel::GreaterBranchLevel(const QModelIndex& left, const QModelIndex& right) {

    pNPnode leftNode = static_cast<pNPnode>(left.internalPointer());
    pNPnode rightNode = static_cast<pNPnode>(right.internalPointer());

    return (leftNode->branchLevel > rightNode->branchLevel);
}

void ANTzSingleGlyphTreeModel::NotifyModelUpdate() {

	emit ModelChanged(m_isDifferentFromSavedFileOrDefaultGlyph);
}

void ANTzSingleGlyphTreeModel::MarkDifferentNotifyModelUpdate() {

	m_isDifferentFromSavedFileOrDefaultGlyph = true;
	emit ModelChanged(m_isDifferentFromSavedFileOrDefaultGlyph);
}

void ANTzSingleGlyphTreeModel::ShowGlyph(bool show) {

	if (m_rootGlyph != nullptr) {
		m_rootGlyph->hide = !show;
	}
}

bool ANTzSingleGlyphTreeModel::IsANTzCSVFile(const QString& filename) const {

	if (filename.right(4).toLower() == ".csv") {

		return true;
	}

	QFile glyphFile(filename);
	const char* firstField = "id,";
	if (glyphFile.open(QIODevice::ReadOnly)) {

		uchar* first3bytes = glyphFile.map(0, 3);
		glyphFile.close();

		if (memcmp(first3bytes, firstField, 3) == 0) {

			return true;
		}
	}

	return false;
}