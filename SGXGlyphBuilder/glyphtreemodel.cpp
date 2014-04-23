#include "glyphtreemodel.h"
#include "npdata.h"
#include "data/npmapfile.h"
#include <Windows.h>

GlyphTreeModel::GlyphTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_antzData = static_cast<pData>(npInitData(0, NULL));
    CreateRootPinNode();
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
    
    beginResetModel();
    npNodeDelete(m_rootGlyph, m_antzData);
    bool success = (npFileOpenMap(filename.c_str(), 1, filename.length(), m_antzData) != 0);
    if (success) {
        m_rootGlyph = static_cast<pNPnode>(m_antzData->map.node[m_antzData->map.nodeRootCount - 1]);
    }
    else {
        CreateRootPinNode();
    }
    endResetModel();

    return success;
}

bool GlyphTreeModel::SaveToCSV(const std::string& filename) const {

    //need to unselect for saving since selection will get saved with the CSV
    pNPnode selectedNode = m_antzData->map.selectedPinNode;
    npSelectNode(NULL, m_antzData);

    bool success = (npFileSaveMap(filename.c_str(), 1, filename.length(), m_antzData) != 0);

    //reselect the previously selected node
    npSelectNode(selectedNode, m_antzData);
    return success;
}

void GlyphTreeModel::CreateFromTemplates(boost::shared_ptr<const SynGlyphX::Glyph> newGlyphTemplates) {

    beginResetModel();
    npNodeDelete(m_rootGlyph, m_antzData);
    CreateNodeFromTemplate(NULL, newGlyphTemplates);
    endResetModel();
}

void GlyphTreeModel::CreateNodeFromTemplate(pNPnode parent, boost::shared_ptr<const SynGlyphX::Glyph> glyphTemplate) {

    pNPnode glyph = npNodeNew(kNodePin, parent, m_antzData);
    if (parent == NULL) {
        m_rootGlyph = glyph;
    }

    glyph->selected = 0;

    SynGlyphX::Vector3 scale = glyphTemplate->GetScale();
    glyph->scale.x = scale[0];
    glyph->scale.y = scale[1];
    glyph->scale.z = scale[2];

    SynGlyphX::Vector3 translate = glyphTemplate->GetTranslate();
    glyph->translate.x = translate[0];
    glyph->translate.y = translate[1];
    glyph->translate.z = translate[2];

    SynGlyphX::Vector3 rotation = glyphTemplate->GetRotate();
    glyph->rotate.x = rotation[0];
    glyph->rotate.y = rotation[1];
    glyph->rotate.z = rotation[2];

    glyph->geometry = 2 * glyphTemplate->GetShape() + glyphTemplate->GetSurface();

    SynGlyphX::Color color = glyphTemplate->GetColor();
    glyph->color.r = color[0];
    glyph->color.g = color[1];
    glyph->color.b = color[2];
    glyph->color.a = color[3];

    glyph->topo = glyphTemplate->GetTopology();

    if (glyphTemplate->GetNumberOfChildren() > 0) {
        CreateNodeFromTemplate(glyph, glyphTemplate->GetChild(0));
    }
}

void GlyphTreeModel::CreateRootPinNode() {
    m_rootGlyph = npNodeNew(kNodePin, NULL, m_antzData);
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