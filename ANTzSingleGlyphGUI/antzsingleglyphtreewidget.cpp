#include "antzsingleglyphtreewidget.h"
#include "npdata.h"
#include "data/npmapfile.h"
#include "glyphmimedata.h"
#include <QtCore/QFile>
#include <QtGui/QMouseEvent>
#include <stack>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtWidgets/QMenu>
#include "glyphnodeconverter.h"
#include "glyphbuilderapplication.h"
#include "defaultbaseimageproperties.h"

namespace SynGlyphXANTz {

	ANTzSingleGlyphTreeWidget::ANTzSingleGlyphTreeWidget(MinMaxGlyphTreeModel::GlyphType glyphTreeType, QWidget *parent)
		: ANTzWidget(parent),
		m_glyphTreeType(glyphTreeType),
		m_clipboardGlyph(),
		m_rootGlyph(nullptr),
		m_model(nullptr),
		m_selectionModel(nullptr),
		m_editingMode(Move),
		m_selectionEdited(false),
		m_allowMultiSelection(false),
		m_baseImageTextureID(0),
		m_animationEnabled(true),
		m_lockZPositionToZero(true),
		m_updateCameraAfterDraw(false)
	{

		setContextMenuPolicy(Qt::NoContextMenu);
	}

	ANTzSingleGlyphTreeWidget::~ANTzSingleGlyphTreeWidget()
	{
		DeleteRootGlyphNode();

		if (m_baseImageTextureID != 0) {

			delete (m_baseImageTextureID);
			m_baseImageTextureID = 0;
		}
	}

	void ANTzSingleGlyphTreeWidget::initializeGL() {

		ANTzWidget::initializeGL();

		SetBaseImage(SynGlyphX::DefaultBaseImageProperties::Type::World);

		ResetCamera();
	}

	void ANTzSingleGlyphTreeWidget::SetAllowMultiSelection(bool allowMultiSelection) {

		m_allowMultiSelection = allowMultiSelection;
	}

	void ANTzSingleGlyphTreeWidget::SetModel(MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel) {

		m_model = model;

		if (m_model != nullptr) {

			QObject::disconnect(m_selectionConnection);
			m_selectionModel = selectionModel;
			if (selectionModel != nullptr) {

				m_selectionConnection = QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ANTzSingleGlyphTreeWidget::UpdateSelection);
			}

			ConnectDataChangedSignal();
			QObject::connect(m_model, &MinMaxGlyphTreeModel::rowsInserted, this, &ANTzSingleGlyphTreeWidget::OnModelRowsInserted);
			QObject::connect(m_model, &MinMaxGlyphTreeModel::rowsMoved, this, &ANTzSingleGlyphTreeWidget::OnModelRowsMoved);
			QObject::connect(m_model, &MinMaxGlyphTreeModel::rowsRemoved, this, &ANTzSingleGlyphTreeWidget::OnModelRowsRemoved);
			QObject::connect(m_model, &MinMaxGlyphTreeModel::modelReset, this, &ANTzSingleGlyphTreeWidget::OnModelReset);

			OnModelReset();
		}
	}

	void ANTzSingleGlyphTreeWidget::BeforeDrawScene() {


	}

	void ANTzSingleGlyphTreeWidget::AfterDrawScene() {

		//since changes from editing don't happen until drawing emit the ObjectEdited signal here
		if (m_selectionEdited) {
			const QModelIndexList& selected = m_selectionModel->selectedIndexes();
			if (!selected.isEmpty() && (m_editingMode != EditingMode::None)) {

				SynGlyphX::PropertyUpdates propertyUpdate = SynGlyphX::PropertyUpdate::UpdatePosition;
				if (m_editingMode == EditingMode::Rotate) {

					propertyUpdate = SynGlyphX::PropertyUpdate::UpdateRotation;
				}
				else if (m_editingMode == EditingMode::Size) {

					propertyUpdate = SynGlyphX::PropertyUpdate::UpdateScale;
				}

				SynGlyphX::Glyph glyph = GlyphNodeConverter::CreateGlyphFromNode(GetGlyphFromModelIndex(selected.back()));
				DisconnectDataChangedSignal();
				m_model->UpdateGlyph(selected.back(), m_glyphTreeType, glyph, propertyUpdate);
				ConnectDataChangedSignal();
			}
			m_selectionEdited = false;
		}

		if (m_updateCameraAfterDraw) {

			m_updateCameraAfterDraw = false;

			//Need to update camera here because ANTz can't update properly
			const QModelIndexList& currentSelection = m_selectionModel->selectedIndexes();
			if (!currentSelection.isEmpty())  {

				const QModelIndex& last = currentSelection.back();
				if (last.isValid()) {

					CenterCameraOnNode(GetGlyphFromModelIndex(last));
				}
			}
		}
	}

	void ANTzSingleGlyphTreeWidget::RebuildTree() {

		if (m_model != nullptr) {

			DeleteRootGlyphNode();
			SynGlyphX::DataMappingGlyphGraph::ConstSharedPtr minMaxGlyphTree = m_model->GetMinMaxGlyphTree();
			if (minMaxGlyphTree) {

				CreateNewSubTree(nullptr, minMaxGlyphTree->GetRoot());

				for (auto& link : minMaxGlyphTree->GetLinks()) {

					pNPnode sourceNode = static_cast<pNPnode>(m_antzData->map.nodeID[m_labelToANTzNodeMap.left.at(link.first.first)]);
					pNPnode destinationNode = static_cast<pNPnode>(m_antzData->map.nodeID[m_labelToANTzNodeMap.left.at(link.first.second)]);
					pNPnode linkNode = npNodeNewLink(sourceNode, destinationNode, m_antzData);

					linkNode->selected = 0;

					UpdateGlyphProperties(linkNode, link.second);
				}
			}

			//Undo previous select node at the beginning of this function
			m_antzData->map.nodeRootIndex = 0;
		}
	}

	void ANTzSingleGlyphTreeWidget::CreateNewSubTree(pNPnode parent, const SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator& minMaxGlyph) {

		pNPnode childNode = CreateNodeFromTemplate(parent, minMaxGlyph);
		m_labelToANTzNodeMap.insert(boost::bimap<SynGlyphX::DataMappingGlyphGraph::Label, int>::value_type(minMaxGlyph->first, childNode->id));

		for (int i = 0; i < m_model->GetMinMaxGlyphTree()->ChildCount(minMaxGlyph); ++i) {

			CreateNewSubTree(childNode, m_model->GetMinMaxGlyphTree()->GetChild(minMaxGlyph, i));
		}
	}

	pNPnode ANTzSingleGlyphTreeWidget::CreateNodeFromTemplate(pNPnode parent, const SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator& minMaxGlyph) {

		pNPnode glyph = npNodeNew(kNodePin, parent, m_antzData);
		if (parent == nullptr) {

			m_rootGlyph = glyph;
		}

		glyph->selected = 0;

		UpdateGlyphProperties(glyph, minMaxGlyph->second);

		return glyph;
	}

	void ANTzSingleGlyphTreeWidget::OnModelReset() {

		RebuildTree();
		EnableBasedOnModelRowCount();
	}

	void ANTzSingleGlyphTreeWidget::OnModelRowsInserted(const QModelIndex& parent, int first, int last) {

		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator minMaxGlyphParent = SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
		pNPnode parentGlyph = GetGlyphFromModelIndex(parent);
		for (int i = first; i <= last; ++i) {

			CreateNewSubTree(parentGlyph, m_model->GetMinMaxGlyphTree()->GetChild(minMaxGlyphParent, i));
		}
		EnableBasedOnModelRowCount();
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

		if (parent.isValid()) {

			pNPnode parentGlyph = GetGlyphFromModelIndex(parent);
			DeleteChildren(parentGlyph, first, last - first + 1);
		}
		else {

			DeleteRootGlyphNode();
		}
		EnableBasedOnModelRowCount();
	}

	void ANTzSingleGlyphTreeWidget::DeleteNode(pNPnode node) {

		RemoveNodeFromIDMap(node);
		ANTzWidget::DeleteNode(node);
	}

	void ANTzSingleGlyphTreeWidget::RemoveNodeFromIDMap(pNPnode node) {

		for (unsigned int i = 0; i < node->childCount; ++i) {

			RemoveNodeFromIDMap(node->child[i]);
		}

		m_labelToANTzNodeMap.right.erase(node->id);
	}

	pNPnode ANTzSingleGlyphTreeWidget::GetGlyphFromModelIndex(const QModelIndex& index) const {

		if (!index.isValid()) {

			return nullptr;
		}

		SynGlyphX::DataMappingGlyphGraph::Label label = m_model->GetLabel(index);

		return static_cast<pNPnode>(m_antzData->map.nodeID[m_labelToANTzNodeMap.left.at(label)]);
	}

	void ANTzSingleGlyphTreeWidget::UpdateData(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

		for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {

			QModelIndex index = m_model->index(i, 0, topLeft.parent());
			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator minMaxGlyph = SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
			pNPnode glyph = GetGlyphFromModelIndex(index);

			UpdateGlyphProperties(glyph, minMaxGlyph->second);
		}
	}

	void ANTzSingleGlyphTreeWidget::UpdateGlyphProperties(pNPnode glyph, const SynGlyphX::DataMappingGlyph& minMaxGlyph) {

		if (m_glyphTreeType == MinMaxGlyphTreeModel::GlyphType::Min) {

			UpdateGlyphProperties(glyph, minMaxGlyph.GetMinGlyph());
		}
		else {

			UpdateGlyphProperties(glyph, minMaxGlyph.GetMaxGlyph());
		}
	}

	void ANTzSingleGlyphTreeWidget::UpdateGlyphProperties(pNPnode glyph, const SynGlyphX::Glyph& glyphTemplate) {

		SynGlyphX::Vector3 scale = glyphTemplate.GetScale();
		glyph->scale.x = scale[0];
		glyph->scale.y = scale[1];
		glyph->scale.z = scale[2];

		SynGlyphX::Vector3 translate = glyphTemplate.GetPosition();

		if ((translate[0] != glyph->translate.x) || (translate[1] != glyph->translate.y) || (translate[2] != glyph->translate.z)) {

			RecenterCameraAfterPaint();
		}

		if (m_rootGlyph == glyph) {

			//Since we're only showing either a single min glyph or a max glyph, we want the root glyph position to be a the center of the 3D world
			glyph->translate.x = 0.0;
			glyph->translate.y = 0.0;
		}
		else {

			glyph->translate.x = translate[0];
			glyph->translate.y = translate[1];
		}

		if ((m_rootGlyph == glyph) && m_lockZPositionToZero) {

			glyph->translate.z = 0.0;
		}
		else {

			glyph->translate.z = translate[2];
		}

		SynGlyphX::Vector3 rotation = glyphTemplate.GetRotation();
		glyph->rotate.x = rotation[0];
		glyph->rotate.y = rotation[1];
		glyph->rotate.z = rotation[2];

		glyph->geometry = 2 * glyphTemplate.GetStructure().GetGeometryShape();

		//This is necessary because ANTz screwed up the enum for geometries
		if (glyphTemplate.GetStructure().GetGeometryShape() == SynGlyphX::GlyphGeometryInfo::Shape::Pin) {
			glyph->geometry += (1 - glyphTemplate.GetStructure().GetGeometrySurface());
		}
		else {
			glyph->geometry += glyphTemplate.GetStructure().GetGeometrySurface();
		}

		SynGlyphX::GlyphColor color = glyphTemplate.GetColor();
		glyph->color.r = color[0];
		glyph->color.g = color[1];
		glyph->color.b = color[2];
		glyph->color.a = glyphTemplate.GetTransparency();

		glyph->topo = glyphTemplate.GetVirtualTopology().GetType();

		UpdateAnimationValuesFromGlyph(glyph, glyphTemplate);

		glyph->ratio = glyphTemplate.GetStructure().GetTorusRatio();
	}

	void ANTzSingleGlyphTreeWidget::UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected) {

		//unselect all nodes that are no longer selected
		const QModelIndexList& deselectedIndicies = deselected.indexes();
		for (int i = 0; i < deselectedIndicies.length(); ++i) {
			if (deselectedIndicies[i].isValid()) {
				pNPnode node = GetGlyphFromModelIndex(deselectedIndicies[i]);
				node->selected = false;
			}
		}

		//select all newly selected nodes
		const QModelIndexList& selectedIndicies = selected.indexes();
		for (int i = 0; i < selectedIndicies.length(); ++i) {
			if (selectedIndicies[i].isValid()) {
				pNPnode node = GetGlyphFromModelIndex(selectedIndicies[i]);
				node->selected = true;
			}
		}

		//Set the cam target if there is only one object selected
		const QModelIndexList& currentSelection = m_selectionModel->selectedIndexes();

		int nodeRootIndex = 0;
		if (currentSelection.length() > 0)  {
			const QModelIndex& last = currentSelection.back();
			if (last.isValid()) {
				CenterCameraOnNode(GetGlyphFromModelIndex(last));
				nodeRootIndex = m_rootGlyph->id;
			}
		}
		
		if ((nodeRootIndex == 0) && (m_rootGlyph != nullptr)) {

			CenterCameraOnNode(m_rootGlyph);
			nodeRootIndex = m_rootGlyph->id;
		}

		m_antzData->map.nodeRootIndex = nodeRootIndex;
	}

	QModelIndex ANTzSingleGlyphTreeWidget::IndexFromANTzID(int id) const {

		pNPnode node = static_cast<pNPnode>(m_antzData->map.nodeID[id]);

		std::stack<unsigned int> childIndexes;
		while (node != m_rootGlyph) {

			childIndexes.push(GetChildIndexFromParentNoLinks(node));
			node = node->parent;
		}

		QModelIndex indexForID = m_model->index(0);
		while (!childIndexes.empty()) {

			indexForID = m_model->index(childIndexes.top(), 0, indexForID);
			childIndexes.pop();
		}

		return indexForID;
	}

	int ANTzSingleGlyphTreeWidget::GetChildIndexFromParentNoLinks(pNPnode node) const {

		if (node->type != kNodePin) {

			throw std::invalid_argument("GetChildIndexFromParentNoLinks can only work on ANTz nodes of type pin");
		}

		int linkCount = 0;
		pNPnode parent = node->parent;

		int i = 0;
		for (; i < parent->childCount; ++i) {
			
			pNPnode currentChild = parent->child[i];
			if (currentChild->type != kNodePin) {

				++linkCount;
				continue;
			}
			if (currentChild == node) {
			
				break;
			}
		}

		return i - linkCount;
	}

	void ANTzSingleGlyphTreeWidget::ShowGlyph(bool show) {

		if (m_rootGlyph != nullptr) {
			m_rootGlyph->hide = !show;
		}
	}

	void ANTzSingleGlyphTreeWidget::DeleteRootGlyphNode() {

		if (m_rootGlyph != nullptr) {

			m_antzData->map.nodeRootIndex = kNPnodeRootPin;
			DeleteNode(m_rootGlyph);
			m_antzData->map.nodeRootIndex = 0;
			m_rootGlyph = nullptr;
			m_labelToANTzNodeMap.clear();
		}
	}

	bool ANTzSingleGlyphTreeWidget::IsRootNodeSelected() const {

		const QModelIndexList& selectedIndexes = m_selectionModel->selectedIndexes();
		if (!selectedIndexes.empty()) {

			return (!selectedIndexes.back().parent().isValid());
		}

		return false;
	}

	void ANTzSingleGlyphTreeWidget::mousePressEvent(QMouseEvent* event) {

		int pickID = PickPinAtPoint(event->pos());

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

		//Make sure that ANTz combo of left & right button does not cause the context menu to appear
		if (event->buttons() == Qt::RightButton) {

			setContextMenuPolicy(Qt::ActionsContextMenu);
		}
		else {

			setContextMenuPolicy(Qt::NoContextMenu);
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

		if ((event->modifiers() == Qt::ShiftModifier) && (event->buttons() != Qt::NoButton)) {

			if ((m_editingMode == EditingMode::Move) && IsRootNodeSelected() && m_lockZPositionToZero) {

				//lock move mode if root node is selected
				m_antzData->io.mouse.tool = kNPtoolNull;
			}
			else {

				m_antzData->io.mouse.tool = kNPtoolMove + m_editingMode - 1;

				if ((event->buttons() & Qt::RightButton) || IsRootNodeSelected()) {

					m_antzData->io.mouse.mode = kNPmouseModeDragXZ;
					m_antzData->io.mouse.buttonR = true;
					m_selectionEdited = true;
				}
				else if (event->buttons() & Qt::LeftButton) {

					m_antzData->io.mouse.mode = kNPmouseModeDragXY;
					m_antzData->io.mouse.buttonR = false;
					m_selectionEdited = true;
				}

				RecenterCameraAfterPaint();
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
			else if (event->buttons() & Qt::MidButton) {

				m_antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
			}
		}

		m_lastMousePosition = event->pos();
	}

	void ANTzSingleGlyphTreeWidget::wheelEvent(QWheelEvent* event) {

		m_antzData->io.mouse.mode = kNPmouseModeCamExamXZ;
		//antzData->io.mouse.delta.x = event->x() - m_lastMousePosition.x();
		m_antzData->io.mouse.delta.y = event->delta() / 10;
		event->accept();
	}

	void ANTzSingleGlyphTreeWidget::SetEditingMode(EditingMode mode) {

		m_editingMode = mode;
	}

	void ANTzSingleGlyphTreeWidget::EnableBasedOnModelRowCount() {

		setEnabled(m_model->rowCount() > 0);
	}

	void ANTzSingleGlyphTreeWidget::ConnectDataChangedSignal() {

		m_dataChangedConnection = QObject::connect(m_model, &MinMaxGlyphTreeModel::dataChanged, this, &ANTzSingleGlyphTreeWidget::UpdateData);
	}

	void ANTzSingleGlyphTreeWidget::DisconnectDataChangedSignal() {

		QObject::disconnect(m_dataChangedConnection);
	}

	void ANTzSingleGlyphTreeWidget::EnableAnimation(bool enable) {

		m_animationEnabled = enable;

		SynGlyphX::DataMappingGlyphGraph::ConstSharedPtr minMaxGlyphTree = m_model->GetMinMaxGlyphTree();

		if (minMaxGlyphTree && (m_rootGlyph != nullptr)) {

			if (m_animationEnabled) {

				ResetAnimationValuesInTree(m_rootGlyph, minMaxGlyphTree->GetRoot());
			}
			else {

				TurnOffAnimationInNodeTree(m_rootGlyph);
			}
		}
	}

	void ANTzSingleGlyphTreeWidget::ResetAnimationValuesInTree(pNPnode node, const SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator& minMaxGlyph) {

		if (m_glyphTreeType == MinMaxGlyphTreeModel::GlyphType::Min) {

			UpdateAnimationValuesFromGlyph(node, minMaxGlyph->second.GetMinGlyph());
		}
		else {

			UpdateAnimationValuesFromGlyph(node, minMaxGlyph->second.GetMaxGlyph());
		}

		for (int i = 0; i < m_model->GetMinMaxGlyphTree()->ChildCount(minMaxGlyph); ++i) {

			ResetAnimationValuesInTree(node->child[i], m_model->GetMinMaxGlyphTree()->GetChild(minMaxGlyph, i));
		}
	}

	void ANTzSingleGlyphTreeWidget::UpdateAnimationValuesFromGlyph(pNPnode node, const SynGlyphX::Glyph& glyph) {

		if (m_animationEnabled) {

			node->rotateRate.x = glyph.GetRotationRate()[0];
			node->rotateRate.y = glyph.GetRotationRate()[1];
			node->rotateRate.z = glyph.GetRotationRate()[2];
		}
		else {

			node->rotateRate.x = 0.0;
			node->rotateRate.y = 0.0;
			node->rotateRate.z = 0.0;
		}
	}

	void ANTzSingleGlyphTreeWidget::TurnOffAnimationInNodeTree(pNPnode node) {

		node->rotateRate.x = 0.0;
		node->rotateRate.y = 0.0;
		node->rotateRate.z = 0.0;

		for (int i = 0; i < node->childCount; ++i) {

			TurnOffAnimationInNodeTree(node->child[i]);
		}
	}

	void ANTzSingleGlyphTreeWidget::SetBaseImage(SynGlyphX::DefaultBaseImageProperties::Type baseImage) {

		if ((baseImage != m_baseImage) || (m_baseImageTextureID == 0)) {

			QOpenGLTexture* newTextureID = BindTextureInFile(SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation() + QString::fromStdWString(SynGlyphX::DefaultBaseImageProperties::GetBasefilename(baseImage)));
			pNPnode rootGrid = static_cast<pNPnode>(m_antzData->map.node[kNPnodeRootGrid]);
			rootGrid->textureID = newTextureID->textureId();

			if (m_baseImageTextureID != 0) {

				delete (m_baseImageTextureID);
			}

			m_baseImageTextureID = newTextureID;
			m_baseImage = baseImage;
		}
	}

	SynGlyphX::DefaultBaseImageProperties::Type ANTzSingleGlyphTreeWidget::GetBaseImage() const {

		return m_baseImage;
	}

	void ANTzSingleGlyphTreeWidget::SetLockZPositionToZero(bool lock) {

		if (m_lockZPositionToZero != lock) {

			m_lockZPositionToZero = lock;
			UpdateGlyphProperties(m_rootGlyph, m_model->GetMinMaxGlyphTree()->GetRoot()->second);
			RecenterCameraAfterPaint();
		}
	}

	bool ANTzSingleGlyphTreeWidget::GetLockZPositionToZero() const {

		return m_lockZPositionToZero;
	}

	void ANTzSingleGlyphTreeWidget::RecenterCameraAfterPaint() {

		m_updateCameraAfterDraw = true;
	}

} //namespace SynGlyphX
