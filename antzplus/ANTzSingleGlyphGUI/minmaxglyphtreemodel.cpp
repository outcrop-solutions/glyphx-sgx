#include "minmaxglyphtreemodel.h"
#include "glyphmimedata.h"
#include <stdexcept>
#include "antzcsvwriter.h"
#include "datamappingglyphfile.h"
#include "glyphnodeconverter.h"
#include <boost/lexical_cast.hpp>
#include <QtGui/QClipboard>
#include "glyphbuilderapplication.h"
#include "linklessgraphmimedata.h"

namespace SynGlyphXANTz {

	MinMaxGlyphTreeModel::MinMaxGlyphTreeModel(QObject *parent)
		: QAbstractItemModel(parent),
		m_minMaxGlyphTree(nullptr)
	{

	}

	MinMaxGlyphTreeModel::~MinMaxGlyphTreeModel()
	{

	}

	void MinMaxGlyphTreeModel::SetMinMaxGlyphTree(SynGlyphX::DataMappingGlyphGraph::SharedPtr minMaxGlyphTree) {

		beginResetModel();
		m_minMaxGlyphTree = std::make_shared<SynGlyphX::DataMappingGlyphGraph>(*minMaxGlyphTree); // create deep copy, otherwise memento will be modified
		endResetModel();
	}

	SynGlyphX::DataMappingGlyphGraph::ConstSharedPtr MinMaxGlyphTreeModel::GetMinMaxGlyphTree() const {

		return m_minMaxGlyphTree;
	}

	int MinMaxGlyphTreeModel::columnCount(const QModelIndex& parent) const {

		return 1;
	}

	QVariant MinMaxGlyphTreeModel::data(const QModelIndex& index, int role) const {

		if (index.isValid()) {
			
			if (role == Qt::DisplayRole) {

				return GetDisplayData(index);
			}
			else if (role == LabelRole) {

				return QVariant::fromValue<SynGlyphX::DataMappingGlyphGraph::Label>(GetLabel(index));
			}
		}

		return QVariant();
	}

	QVariant MinMaxGlyphTreeModel::GetDisplayData(const QModelIndex& index) const {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph = GetIteratorFromIndex(index);

		std::wstring displayedData = SynGlyphX::GlyphGeometryInfo::s_shapeNames.left.at(glyph->second.GetStructure().GetGeometryShape().GetValue()) + L": ";
		displayedData += SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.left.at(glyph->second.GetVirtualTopology().GetType().GetValue());
		if (IsRootGlyph(glyph)) {
			displayedData += L" (Root)";
		}

#ifdef _DEBUG
		displayedData += L" ; Label: " + boost::lexical_cast<std::wstring>(glyph->first);
#endif

		return QString::fromStdWString(displayedData);
	}

	SynGlyphX::DataMappingGlyphGraph::Label MinMaxGlyphTreeModel::GetLabel(const QModelIndex& index) const {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph = GetIteratorFromIndex(index);
		return glyph->first;
	}

	QModelIndex	MinMaxGlyphTreeModel::index(int row, int column, const QModelIndex& parent) const {

		if (!hasIndex(row, column, parent)) {
			return QModelIndex();
		}

		if (!parent.isValid()) {

			return createIndex(row, column, m_minMaxGlyphTree->GetRoot().node());
		}
		else {

			SynGlyphX::DataMappingGlyphGraph::GlyphIterator iT = GetIteratorFromIndex(parent);
			return createIndex(row, column, m_minMaxGlyphTree->GetChild(iT, row).node());
		}
	}

	QModelIndex	MinMaxGlyphTreeModel::parent(const QModelIndex& index) const {

		if (index.isValid()) {

			SynGlyphX::DataMappingGlyphGraph::GlyphIterator iT = GetIteratorFromIndex(index);
			if (!IsRootGlyph(iT)) {

				SynGlyphX::DataMappingGlyphGraph::GlyphIterator parentIterator = m_minMaxGlyphTree->GetParent(iT);
				if (IsRootGlyph(parentIterator)) {

					return createIndex(0, 0, parentIterator.node());
				}
				else {

					SynGlyphX::DataMappingGlyphGraph::GlyphIterator grandParentIterator = m_minMaxGlyphTree->GetParent(parentIterator);
					for (unsigned int i = 0; i < m_minMaxGlyphTree->ChildCount(grandParentIterator.constify()); ++i) {

						if (m_minMaxGlyphTree->GetChild(grandParentIterator, i).equal(parentIterator)) {

							return createIndex(i, 0, parentIterator.node());
						}
					}
				}
			}
		}

		return QModelIndex();
	}

	int	MinMaxGlyphTreeModel::rowCount(const QModelIndex& parent) const {

		if (m_minMaxGlyphTree == nullptr) {

			return 0;
		}

		if (parent.isValid()) {

			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator iT(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
			return m_minMaxGlyphTree->ChildCount(iT);
		}
		else {

			return 1;
		}
	}

	Qt::ItemFlags MinMaxGlyphTreeModel::flags(const QModelIndex& index) const {

		if (!index.isValid()) {
			return 0;
		}

		Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

		if (index.parent().isValid()) {
			flags |= Qt::ItemIsDragEnabled;
		}

		return flags;
	}

	QVariant MinMaxGlyphTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {

		return "";
	}

	bool MinMaxGlyphTreeModel::IsRootGlyph(const SynGlyphX::DataMappingGlyphGraph::GlyphIterator& glyph) const {

		return (glyph.equal(m_minMaxGlyphTree->GetRoot()));
	}

	Qt::DropActions MinMaxGlyphTreeModel::supportedDropActions() const {

		return Qt::MoveAction;
	}

	bool MinMaxGlyphTreeModel::removeRows(int row, int count, const QModelIndex& parent) {

		if (count > 0) {

			if (parent.isValid()) {

				int lastRow = row + count - 1;
				SynGlyphX::DataMappingGlyphGraph::GlyphIterator parentIterator = GetIteratorFromIndex(parent);
				beginRemoveRows(parent, row, lastRow);
				for (int i = lastRow; i >= row; --i) {

					m_minMaxGlyphTree->RemoveChild(parentIterator, i);
				}

				endRemoveRows();
				return true;
			}
			else {

				beginRemoveRows(parent, 0, 0);
				m_minMaxGlyphTree = nullptr;

				endRemoveRows();
			}
			return true;
		}

		return false;
	}

	void MinMaxGlyphTreeModel::AppendChild(const QModelIndex& parent, const SynGlyphX::DataMappingGlyph& glyph, unsigned int numberOfChildren) {

		if (!parent.isValid()) {

			throw std::invalid_argument("Can't append children to invalid parent");
		}

		if (numberOfChildren == 0) {

			throw std::invalid_argument("Can't append 0 children");
		}

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator parentGlyph = GetIteratorFromIndex(parent);
		unsigned int startingNumberOfChildren = m_minMaxGlyphTree->ChildCount(parentGlyph.constify());
		SynGlyphX::Vector3 newPosition = { { 15.0, 0.0, 0.0 } };
		if (startingNumberOfChildren > 0) {

			newPosition = m_minMaxGlyphTree->GetChild(parentGlyph, startingNumberOfChildren - 1)->second.GetMinGlyph().GetPosition();
		}

		beginInsertRows(parent, startingNumberOfChildren, startingNumberOfChildren + numberOfChildren - 1);
		for (unsigned int i = 0; i < numberOfChildren; ++i) {

			SynGlyphX::DataMappingGlyphGraph::GlyphIterator newChildGlyph = m_minMaxGlyphTree->AddChildGlyph(parentGlyph, glyph);

			//For now, update position to 15.0 less than the last x coordinate.  This follows what ANTz does
			newPosition[0] -= 15.0;
			newChildGlyph->second.GetPosition()[0].GetValue().SetMinDiff(newPosition[0], 0.0);
			newChildGlyph->second.GetPosition()[1].GetValue().SetMinDiff(newPosition[1], 0.0);
			newChildGlyph->second.GetPosition()[2].GetValue().SetMinDiff(newPosition[2], 0.0);
		}

		endInsertRows();
	}

	void MinMaxGlyphTreeModel::AppendChildGraph(const QModelIndex& parent, const SynGlyphX::DataMappingGlyphGraph& subgraph) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator newParentGlyph = GetIteratorFromIndex(parent);
		unsigned int numberOfChildren = m_minMaxGlyphTree->ChildCount(newParentGlyph.constify());

		beginInsertRows(parent, numberOfChildren, numberOfChildren);
		m_minMaxGlyphTree->AddChildGlyphGraph(newParentGlyph, subgraph);
		endInsertRows();
	}

	void MinMaxGlyphTreeModel::AppendChildGraphResetPosition(const QModelIndex& parent, const SynGlyphX::DataMappingGlyphGraph& subgraph) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator newParentGlyph = GetIteratorFromIndex(parent);
		unsigned int numberOfChildren = m_minMaxGlyphTree->ChildCount(newParentGlyph.constify());

		//Only do an insert here.  The MoveAction will take care of deleting the old object
		beginInsertRows(parent, numberOfChildren, numberOfChildren);
		SynGlyphX::Vector3 newPosition = { { 15.0, 0.0, 0.0 } };
		if (numberOfChildren > 0) {

			newPosition = m_minMaxGlyphTree->GetChild(newParentGlyph, numberOfChildren - 1)->second.GetMinGlyph().GetPosition();
		}

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator newGlyph = m_minMaxGlyphTree->AddChildGlyphGraph(newParentGlyph, subgraph);

		//For now, update position to 15.0 less than the last x coordinate.  This follows what ANTz does
		newPosition[0] -= 15.0;

		newGlyph->second.GetPosition()[0].GetValue().SetMinDiff(newPosition[0], 0.0);
		newGlyph->second.GetPosition()[1].GetValue().SetMinDiff(newPosition[1], 0.0);
		newGlyph->second.GetPosition()[2].GetValue().SetMinDiff(newPosition[2], 0.0);

		endInsertRows();
	}

	void MinMaxGlyphTreeModel::UpdateGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyph& glyph, SynGlyphX::PropertyUpdates updates) {

		if (index.isValid() && (updates != SynGlyphX::PropertyUpdate::UpdateNone)) {

			SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyphToUpdate = GetIteratorFromIndex(index);

			if (updates.testFlag(SynGlyphX::UpdateTorusRatio)) {

				glyphToUpdate->second.GetStructure().SetTorusRatio(glyph.GetStructure().GetTorusRatio());
			}

			if (updates.testFlag(SynGlyphX::UpdateScale)) {

				glyphToUpdate->second.GetScale()[0].GetValue() = glyph.GetScale()[0].GetValue();
				glyphToUpdate->second.GetScale()[1].GetValue() = glyph.GetScale()[1].GetValue();
				glyphToUpdate->second.GetScale()[2].GetValue() = glyph.GetScale()[2].GetValue();
			}

			if (updates.testFlag(SynGlyphX::UpdatePosition)) {

				glyphToUpdate->second.GetPosition()[0].GetValue() = glyph.GetPosition()[0].GetValue();
				glyphToUpdate->second.GetPosition()[1].GetValue() = glyph.GetPosition()[1].GetValue();
				glyphToUpdate->second.GetPosition()[2].GetValue() = glyph.GetPosition()[2].GetValue();
			}

			if (updates.testFlag(SynGlyphX::UpdateRotation)) {

				glyphToUpdate->second.GetRotation()[0].GetValue() = glyph.GetRotation()[0].GetValue();
				glyphToUpdate->second.GetRotation()[1].GetValue() = glyph.GetRotation()[1].GetValue();
				glyphToUpdate->second.GetRotation()[2].GetValue() = glyph.GetRotation()[2].GetValue();
			}

			if ((updates.testFlag(SynGlyphX::UpdateGeometry)) || (updates.testFlag(SynGlyphX::UpdateSurface))) {

				glyphToUpdate->second.GetStructure().SetGeometryShape(glyph.GetStructure().GetGeometryShape());
				glyphToUpdate->second.GetStructure().SetGeometrySurface(glyph.GetStructure().GetGeometrySurface());
			}

			if (updates.testFlag(SynGlyphX::UpdateColor)) {

				glyphToUpdate->second.GetColor().GetValue() = glyph.GetColor().GetValue();
				glyphToUpdate->second.GetTransparency().GetValue() = glyph.GetTransparency().GetValue();
			}

			if (updates.testFlag(SynGlyphX::UpdateTopology)) {

				glyphToUpdate->second.GetVirtualTopology().SetType(glyph.GetVirtualTopology().GetType());
			}

			if (updates.testFlag(SynGlyphX::UpdateRotationRate)) {

				glyphToUpdate->second.GetRotationRate()[0].GetValue() = glyph.GetRotationRate()[0].GetValue();
				glyphToUpdate->second.GetRotationRate()[1].GetValue() = glyph.GetRotationRate()[1].GetValue();
				glyphToUpdate->second.GetRotationRate()[2].GetValue() = glyph.GetRotationRate()[2].GetValue();
			}

			emit dataChanged(index, index);
		}
	}

	void MinMaxGlyphTreeModel::UpdateGlyphs(const QModelIndexList& indexList, const SynGlyphX::DataMappingGlyph& glyph, SynGlyphX::PropertyUpdates updates) {

		Q_FOREACH(const QModelIndex& index, indexList) {

			UpdateGlyph(index, glyph, updates);
		}
	}

	void MinMaxGlyphTreeModel::UpdateGlyph(const QModelIndex& index, GlyphType type, const SynGlyphX::Glyph& glyph, SynGlyphX::PropertyUpdates updates) {

		if (type == GlyphType::Max) {

			UpdateGlyphMax(index, glyph, updates);
		}
		else {

			UpdateGlyphMin(index, glyph, updates);
		}
	}

	void MinMaxGlyphTreeModel::UpdateGlyphs(const QModelIndexList& indexList, GlyphType type, const SynGlyphX::Glyph& glyph, SynGlyphX::PropertyUpdates updates) {

		Q_FOREACH(const QModelIndex& index, indexList) {

			UpdateGlyph(index, type, glyph, updates);
		}
	}

	void MinMaxGlyphTreeModel::UpdateGlyphMin(const QModelIndex& index, const SynGlyphX::Glyph& glyph, SynGlyphX::PropertyUpdates updates) {

		if (index.isValid() && (updates != SynGlyphX::PropertyUpdate::UpdateNone)) {

			SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyphToUpdate = GetIteratorFromIndex(index);

			if (updates.testFlag(SynGlyphX::UpdateScale)) {

				glyphToUpdate->second.GetScale()[0].GetValue().SetMinWithCurrentMax(glyph.GetScale()[0]);
				glyphToUpdate->second.GetScale()[1].GetValue().SetMinWithCurrentMax(glyph.GetScale()[1]);
				glyphToUpdate->second.GetScale()[2].GetValue().SetMinWithCurrentMax(glyph.GetScale()[2]);
			}

			if (updates.testFlag(SynGlyphX::UpdatePosition)) {

				glyphToUpdate->second.GetPosition()[0].GetValue().SetMinWithCurrentMax(glyph.GetPosition()[0]);
				glyphToUpdate->second.GetPosition()[1].GetValue().SetMinWithCurrentMax(glyph.GetPosition()[1]);
				glyphToUpdate->second.GetPosition()[2].GetValue().SetMinWithCurrentMax(glyph.GetPosition()[2]);
			}

			if (updates.testFlag(SynGlyphX::UpdateRotation)) {

				glyphToUpdate->second.GetRotation()[0].GetValue().SetMinWithCurrentMax(glyph.GetRotation()[0]);
				glyphToUpdate->second.GetRotation()[1].GetValue().SetMinWithCurrentMax(glyph.GetRotation()[1]);
				glyphToUpdate->second.GetRotation()[2].GetValue().SetMinWithCurrentMax(glyph.GetRotation()[2]);
			}

			if ((updates.testFlag(SynGlyphX::UpdateGeometry)) || (updates.testFlag(SynGlyphX::UpdateSurface))) {

				glyphToUpdate->second.GetStructure().SetGeometryShape(glyph.GetStructure().GetGeometryShape());
				glyphToUpdate->second.GetStructure().SetGeometrySurface(glyph.GetStructure().GetGeometrySurface());
			}

			if (updates.testFlag(SynGlyphX::UpdateColor)) {

				glyphToUpdate->second.GetColor().GetValue().SetMinWithCurrentMax(glyph.GetColor());

				glyphToUpdate->second.GetTransparency().GetValue().SetMinWithCurrentMax(glyph.GetTransparency());
			}

			if (updates.testFlag(SynGlyphX::UpdateTopology)) {

				glyphToUpdate->second.GetVirtualTopology().SetType(glyph.GetVirtualTopology().GetType());
			}

			if (updates.testFlag(SynGlyphX::UpdateRotationRate)) {

				glyphToUpdate->second.GetRotationRate()[0].GetValue().SetMinWithCurrentMax(glyph.GetRotationRate()[0]);
				glyphToUpdate->second.GetRotationRate()[1].GetValue().SetMinWithCurrentMax(glyph.GetRotationRate()[1]);
				glyphToUpdate->second.GetRotationRate()[2].GetValue().SetMinWithCurrentMax(glyph.GetRotationRate()[2]);
			}

			if (updates.testFlag(SynGlyphX::UpdateTorusRatio)) {

				glyphToUpdate->second.GetStructure().SetTorusRatio(glyph.GetStructure().GetTorusRatio());
			}

			emit dataChanged(index, index);
		}
	}

	void MinMaxGlyphTreeModel::UpdateGlyphMax(const QModelIndex& index, const SynGlyphX::Glyph& glyph, SynGlyphX::PropertyUpdates updates) {

		if (index.isValid() && (updates != SynGlyphX::PropertyUpdate::UpdateNone)) {

			SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyphToUpdate = GetIteratorFromIndex(index);;

			if (updates.testFlag(SynGlyphX::UpdateScale)) {

				glyphToUpdate->second.GetScale()[0].GetValue().SetMaxWithCurrentMin(glyph.GetScale()[0]);
				glyphToUpdate->second.GetScale()[1].GetValue().SetMaxWithCurrentMin(glyph.GetScale()[1]);
				glyphToUpdate->second.GetScale()[2].GetValue().SetMaxWithCurrentMin(glyph.GetScale()[2]);
			}

			if (updates.testFlag(SynGlyphX::UpdatePosition)) {

				glyphToUpdate->second.GetPosition()[0].GetValue().SetMaxWithCurrentMin(glyph.GetPosition()[0]);
				glyphToUpdate->second.GetPosition()[1].GetValue().SetMaxWithCurrentMin(glyph.GetPosition()[1]);
				glyphToUpdate->second.GetPosition()[2].GetValue().SetMaxWithCurrentMin(glyph.GetPosition()[2]);
			}

			if (updates.testFlag(SynGlyphX::UpdateRotation)) {

				glyphToUpdate->second.GetRotation()[0].GetValue().SetMaxWithCurrentMin(glyph.GetRotation()[0]);
				glyphToUpdate->second.GetRotation()[1].GetValue().SetMaxWithCurrentMin(glyph.GetRotation()[1]);
				glyphToUpdate->second.GetRotation()[2].GetValue().SetMaxWithCurrentMin(glyph.GetRotation()[2]);
			}

			if ((updates.testFlag(SynGlyphX::UpdateGeometry)) || (updates.testFlag(SynGlyphX::UpdateSurface))) {

				glyphToUpdate->second.GetStructure().SetGeometryShape(glyph.GetStructure().GetGeometryShape());
				glyphToUpdate->second.GetStructure().SetGeometrySurface(glyph.GetStructure().GetGeometrySurface());
			}

			if (updates.testFlag(SynGlyphX::UpdateColor)) {

				glyphToUpdate->second.GetColor().GetValue().SetMaxWithCurrentMin(glyph.GetColor());

				glyphToUpdate->second.GetTransparency().GetValue().SetMaxWithCurrentMin(glyph.GetTransparency());
			}

			if (updates.testFlag(SynGlyphX::UpdateTopology)) {

				glyphToUpdate->second.GetVirtualTopology().SetType(glyph.GetVirtualTopology().GetType());
			}

			if (updates.testFlag(SynGlyphX::UpdateRotationRate)) {

				glyphToUpdate->second.GetRotationRate()[0].GetValue().SetMaxWithCurrentMin(glyph.GetRotationRate()[0]);
				glyphToUpdate->second.GetRotationRate()[1].GetValue().SetMaxWithCurrentMin(glyph.GetRotationRate()[1]);
				glyphToUpdate->second.GetRotationRate()[2].GetValue().SetMaxWithCurrentMin(glyph.GetRotationRate()[2]);
			}

			if (updates.testFlag(SynGlyphX::UpdateTorusRatio)) {

				glyphToUpdate->second.GetStructure().SetTorusRatio(glyph.GetStructure().GetTorusRatio());
			}

			emit dataChanged(index, index);
		}
	}

	QStringList MinMaxGlyphTreeModel::mimeTypes() const {

		QStringList types;
		types.push_back(SynGlyphX::GlyphMimeData::s_format);
		return types;
	}

	QMimeData* MinMaxGlyphTreeModel::mimeData(const QModelIndexList& indexes) const {

		if (indexes.empty()) {

			return nullptr;
		}
		else {

			SynGlyphX::GlyphMimeData* mimeData = new SynGlyphX::GlyphMimeData(indexes);
			return mimeData;
		}
	}

	bool MinMaxGlyphTreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {

		const SynGlyphX::GlyphMimeData* glyphData = qobject_cast<const SynGlyphX::GlyphMimeData*>(data);

		if ((glyphData != NULL) && (row == -1)) {

			const QModelIndexList& indexes = glyphData->GetGlyphs();

			bool glyphsMoved = false;

			for (int j = 0; j < indexes.length(); ++j) {

				//Only drop if parents are different
				if (indexes[j].parent() != parent) {

					SynGlyphX::DataMappingGlyphGraph oldGlyphSubtree = m_minMaxGlyphTree->GetSubgraph(GetIteratorFromIndex(indexes[j]), true);
					AppendChildGraphResetPosition(parent, oldGlyphSubtree);
					glyphsMoved = true;
				}
			}

			return glyphsMoved;
		}

		return false;
	}

	bool MinMaxGlyphTreeModel::LoadFromFile(const QString& filename) {

		SynGlyphX::DataMappingGlyphGraph::SharedPtr newGraph;

		try {

			QString extension = filename.right(4);
			if (extension == ".sgt") {

				SynGlyphX::DataMappingGlyphFile fileReader;
				fileReader.ReadFromFile(filename.toStdString());
				newGraph = fileReader.GetDataMappingGlyphGraph();
			}
			else if (extension == ".csv") {

				newGraph = std::make_shared<SynGlyphX::DataMappingGlyphGraph>(*SynGlyphXANTz::GlyphNodeConverter::CreateGlyphGraphFromCSV(filename.toStdString()).get());
			}
			else {

				throw std::runtime_error("File type not supported");
			}
		}
		catch (const std::exception&) {

			throw;
		}

		beginResetModel();
		m_minMaxGlyphTree = newGraph;
		endResetModel();

		return true;
	}

	void MinMaxGlyphTreeModel::SaveToTemplateFile(const QString& filename) const {

		SynGlyphX::DataMappingGlyphFile glyphFile(m_minMaxGlyphTree);
		glyphFile.WriteToFile(filename.toStdString());
	}

	bool MinMaxGlyphTreeModel::SaveToCSV(const QString& filename, bool writeMaxGlyph) {

		SynGlyphX::GlyphGraph::ConstSharedVector trees;
		if (writeMaxGlyph) {

			trees.push_back(m_minMaxGlyphTree->GetMaxGlyphTree());
		}
		else {

			trees.push_back(m_minMaxGlyphTree->GetMinGlyphTree());
		}

		ANTzCSVWriter& csvWriter = ANTzCSVWriter::GetInstance();
		csvWriter.Write({ { filename.toStdString() } }, trees, std::vector<ANTzGrid>(), SynGlyphXANTz::ANTzCSVWriter::OutputPlatform::Windows);

		return true;
	}

	SynGlyphX::DataMappingGlyphGraph MinMaxGlyphTreeModel::GetSubgraph(const QModelIndex& index, bool includeChildren) {

		if (!index.isValid()) {

			throw std::invalid_argument("Index given to MinMaxGlyphTreeModel::GetSubgraph is invalid");
		}

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator vertex = GetIteratorFromIndex(index);
		return m_minMaxGlyphTree->GetSubgraph(vertex, includeChildren);
	}

	void MinMaxGlyphTreeModel::OverwriteGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph& subgraph) {

		if (!index.isValid()) {

			throw std::invalid_argument("Index given to MinMaxGlyphTreeModel::GetSubgraph is invalid");
		}

		SynGlyphX::DataMappingGlyphGraph& nonConstGraph = const_cast<SynGlyphX::DataMappingGlyphGraph&>(subgraph);

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator pasteGlyph = GetIteratorFromIndex(index);
		UpdateGlyph(index, nonConstGraph.GetRoot()->second, SynGlyphX::PropertyUpdate::UpdateAllExceptPosition);

		unsigned int numberOfChildrenOfRootInSubgraph = nonConstGraph.ChildCount(nonConstGraph.GetRoot().constify());
		if (numberOfChildrenOfRootInSubgraph > 0) {

			unsigned int numberOfChildrenInNewParent = m_minMaxGlyphTree->ChildCount(pasteGlyph.constify());
			beginInsertRows(index, numberOfChildrenInNewParent, numberOfChildrenInNewParent + numberOfChildrenOfRootInSubgraph - 1);

			for (unsigned int i = 0; i < numberOfChildrenOfRootInSubgraph; ++i) {

				m_minMaxGlyphTree->AddChildGlyphGraph(pasteGlyph, nonConstGraph.GetSubgraph(nonConstGraph.GetChild(nonConstGraph.GetRoot(), i), true));
			}

			endInsertRows();
		}
	}

	void MinMaxGlyphTreeModel::RepaceModelWithDefaultGlyphTree() {

		beginResetModel();
		m_minMaxGlyphTree = SynGlyphX::DataMappingGlyphGraph::CreateDefault();
		endResetModel();
	}

	bool MinMaxGlyphTreeModel::GreaterBranchLevel(const QModelIndex& left, const QModelIndex& right) {

		const MinMaxGlyphTreeModel* model = dynamic_cast<const MinMaxGlyphTreeModel*>(left.model());

		return (model->GetBranchLevel(left) > model->GetBranchLevel(right));
	}

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator MinMaxGlyphTreeModel::GetIteratorFromIndex(const QModelIndex& index) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator iT(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
		return iT;
	}

	unsigned int MinMaxGlyphTreeModel::GetBranchLevel(const QModelIndex& index) const {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator node = GetIteratorFromIndex(index);
		return m_minMaxGlyphTree->GetDepth(node.constify());
	}

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator MinMaxGlyphTreeModel::GetMinMaxGlyph(const QModelIndex& index) const {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator iT = GetIteratorFromIndex(index);
		return iT.constify();
	}

	void MinMaxGlyphTreeModel::ResetRootMinMaxPositionXY() {

		m_minMaxGlyphTree->ResetRootMinMaxPositionXY();
	}

	QModelIndex MinMaxGlyphTreeModel::GetIndexFromLabel(SynGlyphX::DataMappingGlyphGraph::Label label) const {

		return m_labelToIndexMap.at(label);
	}

} //namespace SynGlyphXANTz