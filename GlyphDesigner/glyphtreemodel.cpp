#include "glyphtreemodel.h"
#include "npdata.h"
#include "data/npmapfile.h"
#include <Windows.h>

GlyphTreeModel::GlyphTreeModel(QObject *parent)
    : QAbstractItemModel(parent),
    m_clipboardGlyph()
{
    m_antzData = static_cast<pData>(npInitData(0, NULL));
    CreateRootPinNode();

    QObject::connect(this, &GlyphTreeModel::NodeUpdated, this, &GlyphTreeModel::ModelChanged);
    QObject::connect(this, &GlyphTreeModel::rowsInserted, this, &GlyphTreeModel::ModelChanged);
    QObject::connect(this, &GlyphTreeModel::rowsMoved, this, &GlyphTreeModel::ModelChanged);
    QObject::connect(this, &GlyphTreeModel::rowsRemoved, this, &GlyphTreeModel::ModelChanged);
}

GlyphTreeModel::~GlyphTreeModel()
{
    npCloseData();
}

int GlyphTreeModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QVariant GlyphTreeModel::data(const QModelIndex& index, int role) const {

    if (!index.isValid()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    pNPnode glyph = static_cast<pNPnode>(index.internalPointer());

    if (glyph == m_rootGlyph) {
        return "ID: " + QString::number(glyph->id) + " (Root Pin)";
    }
    else {
        return "ID: " + QString::number(glyph->id);
    }
}

QModelIndex	GlyphTreeModel::index(int row, int column, const QModelIndex& parent) const {

    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    
    pNPnode parentGlyph;
    if (!parent.isValid()) {
        return createIndex(row, column, m_rootGlyph);
    }
    else {
        parentGlyph = static_cast<pNPnode>(parent.internalPointer());
    }

    if (row < parentGlyph->childCount) {
        return createIndex(row, column, parentGlyph->child[row]);
    }
    else {
        return QModelIndex();
    }
}

QModelIndex	GlyphTreeModel::parent(const QModelIndex& index) const {

    if (!index.isValid()) {
        return QModelIndex();
    }
    
    pNPnode glyph = static_cast<pNPnode>(index.internalPointer());

    if (glyph == m_rootGlyph) {
        return QModelIndex();
    }
    else {
        pNPnode parent = glyph->parent;
        if (parent == m_rootGlyph) {
            return createIndex(0, 0, parent);
        }
        else {
            return createIndex(GetChildIndexFromParent(parent), 0, parent);
        }
    }
}

int	GlyphTreeModel::rowCount(const QModelIndex& parent) const {
    
    pNPnode glyph;
    if (!parent.isValid()) {
        return 1;
    }
    else {
        glyph = static_cast<pNPnode>(parent.internalPointer());
    }
    return glyph->childCount;
}
/*
bool GlyphTreeModel::setData(const QModelIndex& index, const QVariant& value, int role) {

    return true;
}*/

Qt::ItemFlags GlyphTreeModel::flags(const QModelIndex& index) const {
    
    if (!index.isValid()) {
        return 0;
    }
    
    //return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant GlyphTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {

    //We don't need column headers in views so this gets rid of them
    return "";
}

pData GlyphTreeModel::GetANTzData() const {
    return m_antzData;
}

pNPnode GlyphTreeModel::GetRootGlyph() const {
    return m_rootGlyph;
}

bool GlyphTreeModel::LoadFromFile(const std::string& filename) {
    
    //Need to select so that npNodeDelete works properly.  ANTz assumes that the root pin being deleted is selected.  Easier to work around this way
    m_antzData->map.nodeRootIndex = m_rootGlyph->id;

    beginResetModel();
    npNodeDelete(m_rootGlyph, m_antzData);
    bool success = (npFileOpenMap(filename.c_str(), 1, filename.length(), m_antzData) != 0);
    if (success) {
        m_rootGlyph = static_cast<pNPnode>(m_antzData->map.node[m_antzData->map.nodeRootCount - 1]);
    }
    else {
        CreateRootPinNode();
    }

    //Undo previous select node at the beginning of this function
    m_antzData->map.nodeRootIndex = 0;

    endResetModel();

    return success;
}

bool GlyphTreeModel::SaveToCSV(const std::string& filename, const QModelIndexList& selectedItems) const {

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
    
    return success;
}

void GlyphTreeModel::CreateNewTree(const std::vector<boost::shared_ptr<SynGlyphX::Glyph>>& newGlyphTemplates) {

    //Need to select so that npNodeDelete works properly.  ANTz assumes that the root pin being deleted is selected.  Easier to work around this way
    m_antzData->map.nodeRootIndex = m_rootGlyph->id;

    beginResetModel();
    npNodeDelete(m_rootGlyph, m_antzData);

    std::vector<pNPnode> parents;
    parents.push_back(CreateNodeFromTemplate(NULL, newGlyphTemplates[0]));
    for (int i = 1; i < newGlyphTemplates.size(); ++i) {
        std::vector<pNPnode> children;
        for (int j = 0; j < parents.size(); ++j) {
            for (int k = 0; k < newGlyphTemplates[i-1]->GetNumberOfChildren(); ++k) {
                pNPnode glyph = CreateNodeFromTemplate(parents[j], newGlyphTemplates[i]);
                if (glyph != NULL) {
                    children.push_back(glyph);
                }
                else{
                    //throw exception
                }
            }
        }
        parents.swap(children);
    }

    endResetModel();

    //Undo previous select node at the beginning of this function
    m_antzData->map.nodeRootIndex = 0;
}

pNPnode GlyphTreeModel::CreateNodeFromTemplate(pNPnode parent, boost::shared_ptr<const SynGlyphX::Glyph> glyphTemplate) {

    pNPnode glyph = npNodeNew(kNodePin, parent, m_antzData);
    if (parent == NULL) {
        m_rootGlyph = glyph;
    }

    glyph->selected = 0;

    PropertyUpdates updates = UpdateAll ^ UpdatePosition;
    UpdateNode(glyph, glyphTemplate, updates);

    return glyph;
}

void GlyphTreeModel::CreateRootPinNode() {
    
    m_rootGlyph = npNodeNew(kNodePin, NULL, m_antzData);
    npNodeNew(kNodePin, m_rootGlyph, m_antzData);
}

void GlyphTreeModel::UpdateNodes(const QModelIndexList& indexList, boost::shared_ptr<const SynGlyphX::Glyph> glyph, PropertyUpdates updates) {

    for (int i = 0; i < indexList.length(); ++i) {
        
        UpdateNode(indexList[i], glyph, updates);
    }
}

void GlyphTreeModel::UpdateNode(const QModelIndex& index, boost::shared_ptr<const SynGlyphX::Glyph> glyph, PropertyUpdates updates) {

    if (index.isValid()) {
        UpdateNode(static_cast<pNPnode>(index.internalPointer()), glyph, updates);
        emit NodeUpdated(index);
    }
}

void GlyphTreeModel::UpdateNode(pNPnode glyph, boost::shared_ptr<const SynGlyphX::Glyph> glyphTemplate, PropertyUpdates updates) {

    if (updates.testFlag(UpdateScale)) {
        SynGlyphX::Vector3 scale = glyphTemplate->GetScale();
        glyph->scale.x = scale[0];
        glyph->scale.y = scale[1];
        glyph->scale.z = scale[2];
    }

    if (updates.testFlag(UpdatePosition)) {
        SynGlyphX::Vector3 translate = glyphTemplate->GetTranslate();
        glyph->translate.x = translate[0];
        glyph->translate.y = translate[1];
        glyph->translate.z = translate[2];
    }

    if (updates.testFlag(UpdateRotation)) {
        SynGlyphX::Vector3 rotation = glyphTemplate->GetRotate();
        glyph->rotate.x = rotation[0];
        glyph->rotate.y = rotation[1];
        glyph->rotate.z = rotation[2];
    }

    if ((updates.testFlag(UpdateGeometry)) || (updates.testFlag(UpdateSurface))) {
        glyph->geometry = 2 * glyphTemplate->GetShape();

        //This is necessary because ANTz screwed up the enum for geometries
        if (glyphTemplate->GetShape() == SynGlyphX::Geometry::Pin) {
            glyph->geometry += (1 - glyphTemplate->GetSurface());
        }
        else {
            glyph->geometry += glyphTemplate->GetSurface();
        }
    }

    if (updates.testFlag(UpdateColor)) {
        SynGlyphX::Color color = glyphTemplate->GetColor();
        glyph->color.r = color[0];
        glyph->color.g = color[1];
        glyph->color.b = color[2];
        glyph->color.a = color[3];
    }

    if (updates.testFlag(UpdateTopology)) {
        glyph->topo = glyphTemplate->GetTopology();
    }
}

QModelIndex GlyphTreeModel::IndexFromANTzID(int id) {

    pNPnode node = static_cast<pNPnode>(m_antzData->map.nodeID[id]);

    if (node == m_rootGlyph) {
        return createIndex(0, 0, node);
    }
    else {
        return createIndex(GetChildIndexFromParent(node), 0, node);
    }
}

int GlyphTreeModel::GetChildIndexFromParent(pNPnode node) const {
    
    pNPnode parent = node->parent;
    int i = 0;
    for (; i < parent->childCount; ++i) {
        if (parent->child[i] == node) {
            break;
        }
    }
    
    return i;
}

void GlyphTreeModel::AppendChild(const QModelIndex& parent, boost::shared_ptr<const SynGlyphX::Glyph> glyph, unsigned int numberOfChildren) {

    if (parent.isValid()) {
        pNPnode parentNode = static_cast<pNPnode>(parent.internalPointer());
        beginInsertRows(parent, parentNode->childCount, parentNode->childCount + numberOfChildren - 1);
        for (int i = 0; i < numberOfChildren; ++i) {
            CreateNodeFromTemplate(parentNode, glyph);
        }
        endInsertRows();
        emit NodeUpdated(parent);
    }
}

void GlyphTreeModel::DeleteNode(const QModelIndex& index) {

    if ((index.isValid()) && (index.parent().isValid())) {
        int row = index.row();
        beginRemoveRows(index.parent(), row, row);
        npNodeDelete(static_cast<pNPnode>(index.internalPointer()), m_antzData);
        endRemoveRows();
    }
}

void GlyphTreeModel::DeleteChildren(const QModelIndex& parent) {

    if (parent.isValid()) {
        pNPnode parentNode = static_cast<pNPnode>(parent.internalPointer());
        beginRemoveRows(parent, 0, parentNode->childCount);
        for (int i = parentNode->childCount - 1; i >= 0; --i) {
            npNodeDelete(parentNode->child[i], m_antzData);
        }
        endRemoveRows();
    }
}

bool GlyphTreeModel::IsClipboardEmpty() const {

    return (m_clipboardGlyph.get() == NULL);
}

boost::shared_ptr<const SynGlyphX::Glyph> GlyphTreeModel::GetClipboardGlyph() const {

    return m_clipboardGlyph;
}

void GlyphTreeModel::CopyToClipboard(const QModelIndex& index, bool removeFromTree) {

    //m_clipboardGlyph.reset(new SynGlyphX::Glyph(index.))
}

GlyphTreeModel::PropertyUpdates GlyphTreeModel::FindUpdates(boost::shared_ptr<const SynGlyphX::Glyph> oldGlyph, boost::shared_ptr<const SynGlyphX::Glyph> newGlyph) {

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

    if (oldGlyph->GetTranslate() != newGlyph->GetTranslate()) {
        updates |= UpdatePosition;
    }

    if (oldGlyph->GetRotate() != newGlyph->GetRotate()) {
        updates |= UpdateRotation;
    }

    if (oldGlyph->GetScale() != newGlyph->GetScale()) {
        updates |= UpdateScale;
    }

    return updates;
}