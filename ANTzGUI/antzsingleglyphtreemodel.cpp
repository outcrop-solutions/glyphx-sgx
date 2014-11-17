#include "antzsingleglyphtreemodel.h"
#include "npdata.h"
#include "data/npmapfile.h"
#include "glyphmimedata.h"
#include "minmaxglyphtree.h"
#include <QtCore/QFile>
#include <QtGUI/QMouseEvent>
#include <stack>
#include "io/npgl.h"

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
		QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ANTzSingleGlyphTreeWidget::UpdateSelection);

		QObject::connect(m_model, &MinMaxGlyphTreeModel::dataChanged, this, &ANTzSingleGlyphTreeWidget::UpdateData);
		QObject::connect(m_model, &MinMaxGlyphTreeModel::rowsInserted, this, &ANTzSingleGlyphTreeWidget::OnModelRowsInserted);
		QObject::connect(m_model, &MinMaxGlyphTreeModel::rowsMoved, this, &ANTzSingleGlyphTreeWidget::OnModelRowsMoved);
		QObject::connect(m_model, &MinMaxGlyphTreeModel::rowsRemoved, this, &ANTzSingleGlyphTreeWidget::OnModelRowsRemoved);
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

void ANTzSingleGlyphTreeWidget::OnModelRowsInserted(const QModelIndex& parent, int first, int last) {

	SynGlyphX::MinMaxGlyphTree::const_iterator minMaxGlyphParent = SynGlyphX::MinMaxGlyphTree::const_iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(parent.internalPointer()));
	pNPnode parentGlyph = GetGlyphFromModelIndex(parent);
	for (int i = first; i <= last; ++i) {

		CreateNewSubTree(parentGlyph, m_model->GetMinMaxGlyphTree()->child(minMaxGlyphParent, i));
	}
}

void ANTzSingleGlyphTreeWidget::OnModelRowsMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow) {

	pNPnode sourceParentGlyph = GetGlyphFromModelIndex(sourceParent);
	pNPnode destinationParentGlyph = GetGlyphFromModelIndex(destinationParent);
	
	unsigned int numberOfRowsMoved = sourceEnd - sourceStart + 1;
	DeleteChildren(sourceParentGlyph, sourceStart, numberOfRowsMoved);

	unsigned int numberOfDestinationGlyphsMoved = destinationParentGlyph->childCount - destinationRow;
	if (numberOfDestinationGlyphsMoved > 0) {

		DeleteChildren(destinationParentGlyph, destinationRow, numberOfDestinationGlyphsMoved);
	}

	OnModelRowsInserted(destinationParent, destinationRow, destinationRow + numberOfRowsMoved + numberOfDestinationGlyphsMoved - 1);
}

void ANTzSingleGlyphTreeWidget::OnModelRowsRemoved(const QModelIndex& parent, int first, int last) {

	pNPnode parentGlyph = GetGlyphFromModelIndex(parent);
	DeleteChildren(parentGlyph, first, last - first + 1);
}

void ANTzSingleGlyphTreeWidget::DeleteChildren(pNPnode parent, unsigned int first, unsigned int count) {

	if (count > 0) {
	
		for (int i = first + count -1; i >= first; ++i) {

			npNodeRemove(true, parent->child[i], m_antzData);
		}
	}
}

pNPnode ANTzSingleGlyphTreeWidget::GetGlyphFromModelIndex(const QModelIndex& index) const {

	if (!index.isValid()) {

		return nullptr;
	}

	std::stack<unsigned int> childIDs;
	QModelIndex parent = index;
	while (parent.isValid()) {

		childIDs.push(parent.row());
		parent = parent.parent();
	}
	childIDs.pop();

	pNPnode glyph = m_rootGlyph;
	while (!childIDs.empty()) {

		glyph = glyph->child[childIDs.top()];
		childIDs.pop();
	}

	return glyph;
}

void ANTzSingleGlyphTreeWidget::UpdateData(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
	
		QModelIndex index = m_model->index(i, 0, topLeft.parent());
		SynGlyphX::MinMaxGlyphTree::const_iterator minMaxGlyph = SynGlyphX::MinMaxGlyphTree::const_iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(index.internalPointer()));
		pNPnode glyph = GetGlyphFromModelIndex(index);
		UpdateGlyphProperties(glyph, minMaxGlyph);
	}
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

	if (m_rootGlyph == glyph) {

		//Since we're only showing either a single min glyph or a max glyph, we want the root glyph position to be a the center of the 3D world
		glyph->translate.x = 0.0;
		glyph->translate.y = 0.0;
		glyph->translate.z = 0.0;
	}
	else {

		SynGlyphX::Vector3 translate = glyphTemplate.GetPosition();
		glyph->translate.x = translate[0];
		glyph->translate.y = translate[1];
		glyph->translate.z = translate[2];
	}
	
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

void ANTzSingleGlyphTreeWidget::UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected) {

	//unselect all nodes that are no longer selected
	const QModelIndexList& deselectedIndicies = deselected.indexes();
	for (int i = 0; i < deselectedIndicies.length(); ++i) {
		if (deselectedIndicies[i].isValid()) {
			pNPnode node = static_cast<pNPnode>(deselectedIndicies[i].internalPointer());
			node->selected = false;
		}
	}

	//select all newly selected nodes
	const QModelIndexList& selectedIndicies = selected.indexes();
	for (int i = 0; i < selectedIndicies.length(); ++i) {
		if (selectedIndicies[i].isValid()) {
			pNPnode node = static_cast<pNPnode>(selectedIndicies[i].internalPointer());
			node->selected = true;
		}
	}

	//Set the cam target if there is only one object selected
	const QModelIndexList& currentSelection = m_selectionModel->selectedIndexes();

	int nodeRootIndex = 0;
	if (currentSelection.length() > 0)  {
		const QModelIndex& last = currentSelection.back();
		if (last.isValid()) {
			CenterCameraOnNode(static_cast<pNPnode>(last.internalPointer()));
			nodeRootIndex = m_rootGlyph->id;
		}
	}

	m_antzData->map.nodeRootIndex = nodeRootIndex;
}

QModelIndex ANTzSingleGlyphTreeWidget::IndexFromANTzID(int id) const {

    pNPnode node = static_cast<pNPnode>(m_antzData->map.nodeID[id]);

	std::stack<unsigned int> childIndexes;
    while (node != m_rootGlyph) {
        
		childIndexes.push(GetChildIndexFromParent(node));
		node = node->parent;
    }

	QModelIndex indexForID = m_model->index(0);
	while (!childIndexes.empty()) {

		indexForID = m_model->index(childIndexes.top(), 0, indexForID);
		childIndexes.pop();
	}

	return indexForID;
}

int ANTzSingleGlyphTreeWidget::GetChildIndexFromParent(pNPnode node) const {
    
    pNPnode parent = node->parent;
    int i = 0;
    for (; i < parent->childCount; ++i) {
        if (parent->child[i] == node) {
            break;
        }
    }
    
    return i;
}
/*
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

void ANTzSingleGlyphTreeWidget::OnNodeDeleted(const QModelIndex& parent, int start, int end) {

	if (!m_selectionModel->hasSelection()) {

		if (parent.isValid()) {

			pNPnode parentNode = static_cast<pNPnode>(parent.internalPointer());
			CenterCameraOnNode(parentNode);
		}
		else {
			CenterCameraOnNode(m_model->GetRootGlyph());
		}

		pData antzData = m_model->GetANTzData();
		antzData->map.nodeRootIndex = 0;
	}
}*/

void ANTzSingleGlyphTreeWidget::ShowGlyph(bool show) {

	if (m_rootGlyph != nullptr) {
		m_rootGlyph->hide = !show;
	}
}

bool ANTzSingleGlyphTreeWidget::IsANTzCSVFile(const QString& filename) const {

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

bool ANTzSingleGlyphTreeWidget::IsRootNodeSelected() const {

	const QModelIndexList& selectedIndexes = m_selectionModel->selectedIndexes();
	if (!selectedIndexes.empty()) {

		return (!selectedIndexes.back().parent().isValid());
	}

	return false;
}

void ANTzSingleGlyphTreeWidget::mousePressEvent(QMouseEvent* event) {

	int pickID = npPickPin(event->x(), m_antzData->io.gl.height - event->y(), m_antzData);

	if (pickID != 0) {

		if ((event->modifiers() == Qt::ControlModifier) && m_allowMultiSelection) {

			if (event->button() == Qt::LeftButton) {

				m_selectionModel->select(IndexFromANTzID(pickID), QItemSelectionModel::Toggle);
			}
		}
		else {

			m_selectionModel->select(IndexFromANTzID(pickID), QItemSelectionModel::ClearAndSelect);
		}
	}

	m_lastMousePosition = event->pos();
}

void ANTzSingleGlyphTreeWidget::mouseReleaseEvent(QMouseEvent* event) {

	//Reset ANTz mouse values back to the original values
	m_antzData->io.mouse.mode = kNPmouseModeNull;
	m_antzData->io.mouse.tool = kNPtoolNull;
	m_antzData->io.mouse.buttonR = false;
}

void ANTzSingleGlyphTreeWidget::mouseMoveEvent(QMouseEvent* event) {

	m_antzData->io.mouse.previous.x = m_lastMousePosition.x();
	m_antzData->io.mouse.previous.y = m_lastMousePosition.y();
	m_antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.x();
	m_antzData->io.mouse.delta.y = event->y() - m_lastMousePosition.y();
	m_antzData->io.mouse.x = event->x();
	m_antzData->io.mouse.y = event->y();

	if (event->modifiers() == Qt::ShiftModifier) {

		if ((m_editingMode == EditingMode::Move) && (IsRootNodeSelected())) {

			//lock move mode if root node is selected
			m_antzData->io.mouse.tool = kNPtoolNull;
		}
		else {

			m_antzData->io.mouse.tool = kNPtoolMove + m_editingMode;
			if (event->buttons() & Qt::RightButton) {
				m_antzData->io.mouse.mode = kNPmouseModeDragXZ;
				m_antzData->io.mouse.buttonR = true;
				m_selectionEdited = true;
			}
			else if (event->buttons() & Qt::LeftButton) {
				m_antzData->io.mouse.mode = kNPmouseModeDragXY;
				m_antzData->io.mouse.buttonR = false;
				m_selectionEdited = true;
			}
		}
	}
	else {
		m_antzData->io.mouse.tool = kNPtoolNull;
		if (event->buttons() & Qt::LeftButton) {
			if (event->buttons() & Qt::RightButton) {
				m_antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
			}
			else {
				m_antzData->io.mouse.mode = kNPmouseModeCamExamXY;
			}
		}
	}

	m_lastMousePosition = event->pos();
}

void ANTzSingleGlyphTreeWidget::SetEditingMode(EditingMode mode) {

	m_editingMode = mode;
}

void ANTzSingleGlyphTreeWidget::OnModelChanged() {

	setEnabled(m_model->rowCount() > 0);
}