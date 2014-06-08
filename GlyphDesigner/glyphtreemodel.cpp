#include "glyphtreemodel.h"
#include "npdata.h"
#include "data/npmapfile.h"
#include "glyphmimedata.h"

GlyphTreeModel::GlyphTreeModel(QObject *parent)
    : QAbstractItemModel(parent),
    m_clipboardGlyph()
{
    m_antzData = static_cast<pData>(npInitData(0, NULL));
    CreateRootPinNode();

	QObject::connect(this, &GlyphTreeModel::NodeUpdated, this, &GlyphTreeModel::MarkDifferentNotifyModelUpdate);
	QObject::connect(this, &GlyphTreeModel::rowsInserted, this, &GlyphTreeModel::MarkDifferentNotifyModelUpdate);
	QObject::connect(this, &GlyphTreeModel::rowsMoved, this, &GlyphTreeModel::MarkDifferentNotifyModelUpdate);
	QObject::connect(this, &GlyphTreeModel::rowsRemoved, this, &GlyphTreeModel::MarkDifferentNotifyModelUpdate);
	QObject::connect(this, &GlyphTreeModel::modelReset, this, &GlyphTreeModel::NotifyModelUpdate);
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

	std::wstring displayedData = SynGlyphX::GlyphProperties::ShapeToString(static_cast<SynGlyphX::Geometry::Shape>(glyph->geometry / 2)) + L": ";
	displayedData += SynGlyphX::GlyphProperties::TopologyTypeToString(static_cast<SynGlyphX::Topology::Type>(glyph->topo));
    if (glyph == m_rootGlyph) {
		displayedData += L" (Root)";
    }
    
	return QString::fromStdWString(displayedData);
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

    return QAbstractItemModel::setData(index, value, role);
}*/

Qt::ItemFlags GlyphTreeModel::flags(const QModelIndex& index) const {
    
    if (!index.isValid()) {
        return 0;
    }

    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    if (index.parent().isValid()) {
        flags |= Qt::ItemIsDragEnabled;
    }
    
    return flags;
}

QVariant GlyphTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {

    //We don't need column headers in views so this gets rid of them
    return "";
}

Qt::DropActions GlyphTreeModel::supportedDropActions() const {

    return Qt::MoveAction;
}

pData GlyphTreeModel::GetANTzData() const {
    return m_antzData;
}

pNPnode GlyphTreeModel::GetRootGlyph() const {
    return m_rootGlyph;
}

bool GlyphTreeModel::LoadFromFile(const std::string& filename) {
    
    //Need to select so that npNodeDelete works properly.  ANTz assumes that the root pin being deleted is selected.  Easier to work around this for now
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
	m_isDifferentFromSavedFileOrDefaultGlyph = false;

    //Undo previous select node at the beginning of this function
    m_antzData->map.nodeRootIndex = 0;

    endResetModel();

    return success;
}

bool GlyphTreeModel::SaveToCSV(const std::string& filename, const QModelIndexList& selectedItems) {

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
		m_isDifferentFromSavedFileOrDefaultGlyph = false;
		NotifyModelUpdate();
	}
    
    return success;
}

void GlyphTreeModel::CreateNewTree(boost::shared_ptr<const SynGlyphX::Glyph> newGlyph) {

    //Need to select so that npNodeDelete works properly.  ANTz assumes that the root pin being deleted is selected.  Easier to work around this way
    m_antzData->map.nodeRootIndex = m_rootGlyph->id;

    beginResetModel();
    npNodeDelete(m_rootGlyph, m_antzData);

    CreateNodeFromTemplate(NULL, newGlyph);
    for (int i = 0; i < newGlyph->GetNumberOfChildren(); ++i) {

        CreateNewSubTree(m_rootGlyph, newGlyph->GetChild(i), false);
    }
	m_isDifferentFromSavedFileOrDefaultGlyph = true;

    endResetModel();

    //Undo previous select node at the beginning of this function
    m_antzData->map.nodeRootIndex = 0;
}

void GlyphTreeModel::CreateNewSubTree(pNPnode parent, boost::shared_ptr<const SynGlyphX::Glyph> newGlyph, bool needResetModelSignals) {

    if (parent == NULL) {
        return;
    }

    if (needResetModelSignals) {
        beginResetModel();
    }

    pNPnode child = CreateNodeFromTemplate(parent, newGlyph);
    for (int i = 0; i < newGlyph->GetNumberOfChildren(); ++i) {

        CreateNewSubTree(child, newGlyph->GetChild(i), false);
    }
	m_isDifferentFromSavedFileOrDefaultGlyph = true;

    if (needResetModelSignals) {
        endResetModel();
    }
}

pNPnode GlyphTreeModel::CreateNodeFromTemplate(pNPnode parent, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyphTemplate) {

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
	m_isDifferentFromSavedFileOrDefaultGlyph = false;
}

void GlyphTreeModel::UpdateNodes(const QModelIndexList& indexList, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph, PropertyUpdates updates) {

    for (int i = 0; i < indexList.length(); ++i) {
        
        UpdateNode(indexList[i], glyph, updates);
    }
}

void GlyphTreeModel::UpdateNode(const QModelIndex& index, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph, PropertyUpdates updates) {

    if (index.isValid()) {
        UpdateNode(static_cast<pNPnode>(index.internalPointer()), glyph, updates);
        emit NodeUpdated(index);
    }
}

void GlyphTreeModel::UpdateNode(pNPnode glyph, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyphTemplate, PropertyUpdates updates) {

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

void GlyphTreeModel::AppendChild(const QModelIndex& parent, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph, unsigned int numberOfChildren) {

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
/*
bool GlyphTreeModel::insertRows(int row, int count, const QModelIndex & parent) {

    return QAbstractItemModel::insertRows(row, count, parent);
}*/

bool GlyphTreeModel::removeRows(int row, int count, const QModelIndex& parent) {

    if ((parent.isValid()) && (hasIndex(parent.row(), 0, parent.parent())) && (count > 0)) {

        int lastRow = row + count - 1;
        pNPnode parentNode = static_cast<pNPnode>(parent.internalPointer());
        if (lastRow < parentNode->childCount) {
            
            beginRemoveRows(parent, row, lastRow);
            for (int i = lastRow; i >= row; --i) {
                npNodeRemove(true, parentNode->child[i], m_antzData);
            }
            endRemoveRows();
        }
        return true;
    }

    return false;
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

GlyphTreeModel::PropertyUpdates GlyphTreeModel::FindUpdates(boost::shared_ptr<const SynGlyphX::GlyphProperties> oldGlyph, boost::shared_ptr<const SynGlyphX::GlyphProperties> newGlyph) {

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

bool GlyphTreeModel::GreaterBranchLevel(const QModelIndex& left, const QModelIndex& right) {

    pNPnode leftNode = static_cast<pNPnode>(left.internalPointer());
    pNPnode rightNode = static_cast<pNPnode>(right.internalPointer());

    return (leftNode->branchLevel > rightNode->branchLevel);
}

QStringList GlyphTreeModel::mimeTypes() const {

    QStringList types;
    types.push_back(GlyphMimeData::Format);
    return types;
}

QMimeData* GlyphTreeModel::mimeData(const QModelIndexList& indexes) const {

    /*std::vector<boost::shared_ptr<SynGlyphX::Glyph>> glyphs;
    glyphs.reserve(indexes.length());
    for (int i = 0; i < indexes.length(); ++i) {
        boost::shared_ptr<SynGlyphX::Glyph> glyph(new SynGlyphX::Glyph(static_cast<pNPnode>(indexes[i].internalPointer())));
        glyphs.push_back(glyph);
    }

    GlyphMimeData* mimeData = new GlyphMimeData(glyphs);*/
    GlyphMimeData* mimeData = new GlyphMimeData(indexes);
    return mimeData;
}

bool GlyphTreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {

    const GlyphMimeData* glyphData = qobject_cast<const GlyphMimeData*>(data);
    
    if ((glyphData != NULL) && (row == -1)) {

		pNPnode newParentNode = static_cast<pNPnode>(parent.internalPointer());
        const QModelIndexList& indexes = glyphData->GetGlyphs();
		std::vector<boost::shared_ptr<SynGlyphX::Glyph>> glyphs;

		beginResetModel();
		for (int j = 0; j < indexes.length(); ++j) {
			
			pNPnode node = static_cast<pNPnode>(indexes[j].internalPointer());

			//If the node is being dropped on its own parent, do nothing
			if (node->parent != newParentNode) {
				boost::shared_ptr<SynGlyphX::Glyph> glyph(new SynGlyphX::Glyph(node));
				glyphs.push_back(glyph);
				npNodeRemove(true, node, m_antzData);
			}
		}

        for (int i = 0; i < glyphs.size(); ++i) {

			CreateNewSubTree(newParentNode, glyphs[i], false);
        }
        endResetModel();
        m_antzData->map.nodeRootIndex = 0;
        
        return true;
    }

    return false;
}

void GlyphTreeModel::NotifyModelUpdate() {

	emit ModelChanged(m_isDifferentFromSavedFileOrDefaultGlyph);
}

void GlyphTreeModel::MarkDifferentNotifyModelUpdate() {

	m_isDifferentFromSavedFileOrDefaultGlyph = true;
	emit ModelChanged(m_isDifferentFromSavedFileOrDefaultGlyph);
}