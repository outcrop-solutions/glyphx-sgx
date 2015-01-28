#include "minmaxglyphtreemodel.h"
#include "glyphmimedata.h"
#include <stdexcept>
#include "antzcsvwriter.h"
#include "datamappingglyphfile.h"
#include "glyphnodeconverter.h"

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
		m_minMaxGlyphTree = minMaxGlyphTree;
		endResetModel();
	}

	SynGlyphX::DataMappingGlyphGraph::ConstSharedPtr MinMaxGlyphTreeModel::GetMinMaxGlyphTree() const {

		return m_minMaxGlyphTree;
	}

	int MinMaxGlyphTreeModel::columnCount(const QModelIndex& parent) const {

		return 1;
	}

	QVariant MinMaxGlyphTreeModel::data(const QModelIndex& index, int role) const {

		if (!index.isValid()) {
			return QVariant();
		}

		if (role != Qt::DisplayRole) {
			return QVariant();
		}

		SynGlyphX::DataMappingGlyphGraph::iterator glyph = GetIteratorFromIndex(index);

		std::wstring displayedData = SynGlyphX::GlyphStructuralProperties::s_shapeNames.left.at(glyph->GetStructure().GetGeometryShape()) + L": ";
		displayedData += SynGlyphX::GlyphStructuralProperties::s_virtualTopologyNames.left.at(glyph->GetStructure().GetVirtualTopology());
		if (IsRootGlyph(glyph)) {
			displayedData += L" (Root)";
		}

		return QString::fromStdWString(displayedData);
	}

	QModelIndex	MinMaxGlyphTreeModel::index(int row, int column, const QModelIndex& parent) const {

		if (!hasIndex(row, column, parent)) {
			return QModelIndex();
		}

		if (!parent.isValid()) {

			return createIndex(row, column, m_minMaxGlyphTree->root().node());
		}
		else {

			SynGlyphX::DataMappingGlyphGraph::iterator iT = GetIteratorFromIndex(parent);
			return createIndex(row, column, m_minMaxGlyphTree->child(iT, row).node());
		}
	}

	QModelIndex	MinMaxGlyphTreeModel::parent(const QModelIndex& index) const {

		if (index.isValid()) {

			SynGlyphX::DataMappingGlyphGraph::iterator iT = GetIteratorFromIndex(index);
			if (!IsRootGlyph(iT)) {

				SynGlyphX::DataMappingGlyphGraph::iterator parentIterator = m_minMaxGlyphTree->parent(iT);
				if (IsRootGlyph(parentIterator)) {

					return createIndex(0, 0, parentIterator.node());
				}
				else {

					SynGlyphX::DataMappingGlyphGraph::iterator grandParentIterator = m_minMaxGlyphTree->parent(parentIterator);
					for (int i = 0; i < m_minMaxGlyphTree->children(grandParentIterator); ++i) {

						if (m_minMaxGlyphTree->child(grandParentIterator, i).equal(parentIterator)) {

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

			SynGlyphX::DataMappingGlyphGraph::const_iterator iT(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
			return m_minMaxGlyphTree->children(iT);
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

	bool MinMaxGlyphTreeModel::IsRootGlyph(const SynGlyphX::DataMappingGlyphGraph::iterator& glyph) const {

		return (glyph.equal(m_minMaxGlyphTree->root()));
	}

	Qt::DropActions MinMaxGlyphTreeModel::supportedDropActions() const {

		return Qt::MoveAction;
	}

	bool MinMaxGlyphTreeModel::removeRows(int row, int count, const QModelIndex& parent) {

		if (count > 0) {

			if (parent.isValid()) {

				int lastRow = row + count - 1;
				SynGlyphX::DataMappingGlyphGraph::iterator parentIterator = GetIteratorFromIndex(parent);
				beginRemoveRows(parent, row, lastRow);
				for (int i = lastRow; i >= row; --i) {

					m_minMaxGlyphTree->erase_child(parentIterator, i);
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

		SynGlyphX::DataMappingGlyphGraph::iterator parentGlyph = GetIteratorFromIndex(parent);
		unsigned int startingNumberOfChildren = m_minMaxGlyphTree->children(parentGlyph);
		SynGlyphX::Vector3 newPosition = { { 15.0, 0.0, 0.0 } };
		if (startingNumberOfChildren > 0) {

			newPosition = m_minMaxGlyphTree->child(parentGlyph, startingNumberOfChildren - 1)->GetMinGlyph().GetPosition();
		}

		beginInsertRows(parent, startingNumberOfChildren, startingNumberOfChildren + numberOfChildren - 1);
		for (int i = 0; i < numberOfChildren; ++i) {

			SynGlyphX::DataMappingGlyphGraph::iterator newChildGlyph = m_minMaxGlyphTree->append(parentGlyph, glyph);

			//For now, update position to 15.0 less than the last x coordinate.  This follows what ANTz does
			newPosition[0] -= 15.0;
			newChildGlyph->GetPosition()[0].GetValue().first = newPosition[0];
			newChildGlyph->GetPosition()[1].GetValue().first = newPosition[1];
			newChildGlyph->GetPosition()[2].GetValue().first = newPosition[2];
		}

		endInsertRows();
	}

	void MinMaxGlyphTreeModel::UpdateGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyph& glyph, PropertyUpdates updates) {

		if (index.isValid() && (updates != PropertyUpdate::UpdateNone)) {

			SynGlyphX::DataMappingGlyphGraph::iterator glyphToUpdate = GetIteratorFromIndex(index);

			if (updates.testFlag(UpdateScale)) {

				glyphToUpdate->GetScale()[0].GetValue() = glyph.GetScale()[0].GetValue();
				glyphToUpdate->GetScale()[1].GetValue() = glyph.GetScale()[1].GetValue();
				glyphToUpdate->GetScale()[2].GetValue() = glyph.GetScale()[2].GetValue();
			}

			if (updates.testFlag(UpdatePosition)) {

				glyphToUpdate->GetPosition()[0].GetValue() = glyph.GetPosition()[0].GetValue();
				glyphToUpdate->GetPosition()[1].GetValue() = glyph.GetPosition()[1].GetValue();
				glyphToUpdate->GetPosition()[2].GetValue() = glyph.GetPosition()[2].GetValue();
			}

			if (updates.testFlag(UpdateRotation)) {

				glyphToUpdate->GetRotation()[0].GetValue() = glyph.GetRotation()[0].GetValue();
				glyphToUpdate->GetRotation()[1].GetValue() = glyph.GetRotation()[1].GetValue();
				glyphToUpdate->GetRotation()[2].GetValue() = glyph.GetRotation()[2].GetValue();
			}

			if ((updates.testFlag(UpdateGeometry)) || (updates.testFlag(UpdateSurface))) {

				glyphToUpdate->GetStructure().SetGeometryShape(glyph.GetStructure().GetGeometryShape());
				glyphToUpdate->GetStructure().SetGeometrySurface(glyph.GetStructure().GetGeometrySurface());
			}

			if (updates.testFlag(UpdateColor)) {

				glyphToUpdate->GetColor().GetValue() = glyph.GetColor().GetValue();
				glyphToUpdate->GetTransparency().GetValue() = glyph.GetTransparency().GetValue();
			}

			if (updates.testFlag(UpdateTopology)) {

				glyphToUpdate->GetStructure().SetVirtualTopology(glyph.GetStructure().GetVirtualTopology());
			}

			if (updates.testFlag(UpdateRotationRate)) {

				glyphToUpdate->GetRotationRate()[0].GetValue() = glyph.GetRotationRate()[0].GetValue();
				glyphToUpdate->GetRotationRate()[1].GetValue() = glyph.GetRotationRate()[1].GetValue();
				glyphToUpdate->GetRotationRate()[2].GetValue() = glyph.GetRotationRate()[2].GetValue();
			}

			emit dataChanged(index, index);
		}
	}

	void MinMaxGlyphTreeModel::UpdateGlyphs(const QModelIndexList& indexList, const SynGlyphX::DataMappingGlyph& glyph, PropertyUpdates updates) {

		Q_FOREACH(const QModelIndex& index, indexList) {

			UpdateGlyph(index, glyph, updates);
		}
	}

	void MinMaxGlyphTreeModel::UpdateGlyph(const QModelIndex& index, GlyphType type, const SynGlyphX::Glyph& glyph, PropertyUpdates updates) {

		if (type == GlyphType::Max) {

			UpdateGlyphMax(index, glyph, updates);
		}
		else {

			UpdateGlyphMin(index, glyph, updates);
		}
	}

	void MinMaxGlyphTreeModel::UpdateGlyphs(const QModelIndexList& indexList, GlyphType type, const SynGlyphX::Glyph& glyph, PropertyUpdates updates) {

		Q_FOREACH(const QModelIndex& index, indexList) {

			UpdateGlyph(index, type, glyph, updates);
		}
	}

	void MinMaxGlyphTreeModel::UpdateGlyphMin(const QModelIndex& index, const SynGlyphX::Glyph& glyph, PropertyUpdates updates) {

		if (index.isValid() && (updates != PropertyUpdate::UpdateNone)) {

			SynGlyphX::DataMappingGlyphGraph::iterator glyphToUpdate = GetIteratorFromIndex(index);

			if (updates.testFlag(UpdateScale)) {

				glyphToUpdate->GetScale()[0].GetValue().second -= glyph.GetScale()[0] - glyphToUpdate->GetScale()[0].GetValue().first;
				glyphToUpdate->GetScale()[1].GetValue().second -= glyph.GetScale()[1] - glyphToUpdate->GetScale()[1].GetValue().first;
				glyphToUpdate->GetScale()[2].GetValue().second -= glyph.GetScale()[2] - glyphToUpdate->GetScale()[2].GetValue().first;

				glyphToUpdate->GetScale()[0].GetValue().first = glyph.GetScale()[0];
				glyphToUpdate->GetScale()[1].GetValue().first = glyph.GetScale()[1];
				glyphToUpdate->GetScale()[2].GetValue().first = glyph.GetScale()[2];
			}

			if (updates.testFlag(UpdatePosition)) {

				glyphToUpdate->GetPosition()[0].GetValue().second -= glyph.GetPosition()[0] - glyphToUpdate->GetPosition()[0].GetValue().first;
				glyphToUpdate->GetPosition()[1].GetValue().second -= glyph.GetPosition()[1] - glyphToUpdate->GetPosition()[1].GetValue().first;
				glyphToUpdate->GetPosition()[2].GetValue().second -= glyph.GetPosition()[2] - glyphToUpdate->GetPosition()[2].GetValue().first;

				glyphToUpdate->GetPosition()[0].GetValue().first = glyph.GetPosition()[0];
				glyphToUpdate->GetPosition()[1].GetValue().first = glyph.GetPosition()[1];
				glyphToUpdate->GetPosition()[2].GetValue().first = glyph.GetPosition()[2];
			}

			if (updates.testFlag(UpdateRotation)) {

				glyphToUpdate->GetRotation()[0].GetValue().second -= glyph.GetRotation()[0] - glyphToUpdate->GetRotation()[0].GetValue().first;
				glyphToUpdate->GetRotation()[1].GetValue().second -= glyph.GetRotation()[1] - glyphToUpdate->GetRotation()[1].GetValue().first;
				glyphToUpdate->GetRotation()[2].GetValue().second -= glyph.GetRotation()[2] - glyphToUpdate->GetRotation()[2].GetValue().first;

				glyphToUpdate->GetRotation()[0].GetValue().first = glyph.GetRotation()[0];
				glyphToUpdate->GetRotation()[1].GetValue().first = glyph.GetRotation()[1];
				glyphToUpdate->GetRotation()[2].GetValue().first = glyph.GetRotation()[2];
			}

			if ((updates.testFlag(UpdateGeometry)) || (updates.testFlag(UpdateSurface))) {

				glyphToUpdate->GetStructure().SetGeometryShape(glyph.GetStructure().GetGeometryShape());
				glyphToUpdate->GetStructure().SetGeometrySurface(glyph.GetStructure().GetGeometrySurface());
			}

			if (updates.testFlag(UpdateColor)) {

				glyphToUpdate->GetColor().GetValue().second.Set(0, glyph.GetColor()[0] - glyphToUpdate->GetColor().GetValue().first[0]);
				glyphToUpdate->GetColor().GetValue().second.Set(1, glyph.GetColor()[1] - glyphToUpdate->GetColor().GetValue().first[1]);
				glyphToUpdate->GetColor().GetValue().second.Set(2, glyph.GetColor()[2] - glyphToUpdate->GetColor().GetValue().first[2]);
				glyphToUpdate->GetColor().GetValue().first = glyph.GetColor();

				glyphToUpdate->GetTransparency().GetValue().second = glyph.GetTransparency() - glyphToUpdate->GetTransparency().GetValue().first;
				glyphToUpdate->GetTransparency().GetValue().first = glyph.GetTransparency();
			}

			if (updates.testFlag(UpdateTopology)) {

				glyphToUpdate->GetStructure().SetVirtualTopology(glyph.GetStructure().GetVirtualTopology());
			}

			if (updates.testFlag(UpdateRotationRate)) {

				glyphToUpdate->GetRotationRate()[0].GetValue().second -= glyph.GetRotationRate()[0] - glyphToUpdate->GetRotationRate()[0].GetValue().first;
				glyphToUpdate->GetRotationRate()[1].GetValue().second -= glyph.GetRotationRate()[1] - glyphToUpdate->GetRotationRate()[1].GetValue().first;
				glyphToUpdate->GetRotationRate()[2].GetValue().second -= glyph.GetRotationRate()[2] - glyphToUpdate->GetRotationRate()[2].GetValue().first;

				glyphToUpdate->GetRotationRate()[0].GetValue().first = glyph.GetRotationRate()[0];
				glyphToUpdate->GetRotationRate()[1].GetValue().first = glyph.GetRotationRate()[1];
				glyphToUpdate->GetRotationRate()[2].GetValue().first = glyph.GetRotationRate()[2];
			}

			if (updates.testFlag(UpdateTorusRatio)) {

				glyphToUpdate->GetStructure().SetTorusRatio(glyph.GetStructure().GetTorusRatio());
			}

			emit dataChanged(index, index);
		}
	}

	void MinMaxGlyphTreeModel::UpdateGlyphMax(const QModelIndex& index, const SynGlyphX::Glyph& glyph, PropertyUpdates updates) {

		if (index.isValid() && (updates != PropertyUpdate::UpdateNone)) {

			SynGlyphX::DataMappingGlyphGraph::iterator glyphToUpdate = GetIteratorFromIndex(index);;

			if (updates.testFlag(UpdateScale)) {

				glyphToUpdate->GetScale()[0].GetValue().second = glyph.GetScale()[0] - glyphToUpdate->GetScale()[0].GetValue().first;
				glyphToUpdate->GetScale()[1].GetValue().second = glyph.GetScale()[1] - glyphToUpdate->GetScale()[1].GetValue().first;
				glyphToUpdate->GetScale()[2].GetValue().second = glyph.GetScale()[2] - glyphToUpdate->GetScale()[2].GetValue().first;
			}

			if (updates.testFlag(UpdatePosition)) {

				glyphToUpdate->GetPosition()[0].GetValue().second = glyph.GetPosition()[0] - glyphToUpdate->GetPosition()[0].GetValue().first;
				glyphToUpdate->GetPosition()[1].GetValue().second = glyph.GetPosition()[1] - glyphToUpdate->GetPosition()[1].GetValue().first;
				glyphToUpdate->GetPosition()[2].GetValue().second = glyph.GetPosition()[2] - glyphToUpdate->GetPosition()[2].GetValue().first;
			}

			if (updates.testFlag(UpdateRotation)) {

				glyphToUpdate->GetRotation()[0].GetValue().second = glyph.GetRotation()[0] - glyphToUpdate->GetRotation()[0].GetValue().first;
				glyphToUpdate->GetRotation()[1].GetValue().second = glyph.GetRotation()[1] - glyphToUpdate->GetRotation()[1].GetValue().first;
				glyphToUpdate->GetRotation()[2].GetValue().second = glyph.GetRotation()[2] - glyphToUpdate->GetRotation()[2].GetValue().first;
			}

			if ((updates.testFlag(UpdateGeometry)) || (updates.testFlag(UpdateSurface))) {

				glyphToUpdate->GetStructure().SetGeometryShape(glyph.GetStructure().GetGeometryShape());
				glyphToUpdate->GetStructure().SetGeometrySurface(glyph.GetStructure().GetGeometrySurface());
			}

			if (updates.testFlag(UpdateColor)) {

				glyphToUpdate->GetColor().GetValue().second.Set(0, glyph.GetColor()[0] - glyphToUpdate->GetColor().GetValue().first[0]);
				glyphToUpdate->GetColor().GetValue().second.Set(1, glyph.GetColor()[1] - glyphToUpdate->GetColor().GetValue().first[1]);
				glyphToUpdate->GetColor().GetValue().second.Set(2, glyph.GetColor()[2] - glyphToUpdate->GetColor().GetValue().first[2]);

				glyphToUpdate->GetTransparency().GetValue().second = glyph.GetTransparency() - glyphToUpdate->GetTransparency().GetValue().first;
			}

			if (updates.testFlag(UpdateTopology)) {

				glyphToUpdate->GetStructure().SetVirtualTopology(glyph.GetStructure().GetVirtualTopology());
			}

			if (updates.testFlag(UpdateRotationRate)) {

				glyphToUpdate->GetRotationRate()[0].GetValue().second = glyph.GetRotationRate()[0] - glyphToUpdate->GetRotationRate()[0].GetValue().first;
				glyphToUpdate->GetRotationRate()[1].GetValue().second = glyph.GetRotationRate()[1] - glyphToUpdate->GetRotationRate()[1].GetValue().first;
				glyphToUpdate->GetRotationRate()[2].GetValue().second = glyph.GetRotationRate()[2] - glyphToUpdate->GetRotationRate()[2].GetValue().first;
			}

			if (updates.testFlag(UpdateTorusRatio)) {

				glyphToUpdate->GetStructure().SetTorusRatio(glyph.GetStructure().GetTorusRatio());
			}

			emit dataChanged(index, index);
		}
	}

	QStringList MinMaxGlyphTreeModel::mimeTypes() const {

		QStringList types;
		types.push_back(GlyphMimeData::Format);
		return types;
	}

	QMimeData* MinMaxGlyphTreeModel::mimeData(const QModelIndexList& indexes) const {

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

	bool MinMaxGlyphTreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {

		const GlyphMimeData* glyphData = qobject_cast<const GlyphMimeData*>(data);

		if ((glyphData != NULL) && (row == -1)) {

			SynGlyphX::DataMappingGlyphGraph::iterator newParentGlyph = GetIteratorFromIndex(parent);
			const QModelIndexList& indexes = glyphData->GetGlyphs();

			for (int j = 0; j < indexes.length(); ++j) {

				SynGlyphX::DataMappingGlyphGraph::iterator oldGlyph = GetIteratorFromIndex(indexes[j]);
				SynGlyphX::DataMappingGlyphGraph::iterator oldParentGlyph = m_minMaxGlyphTree->parent(oldGlyph);

				//Only drop if parents are different
				if (oldParentGlyph != newParentGlyph) {

					unsigned int numberOfChildren = m_minMaxGlyphTree->children(newParentGlyph);

					beginMoveRows(indexes[j].parent(), indexes[j].row(), indexes[j].row(), parent, numberOfChildren);
					SynGlyphX::Vector3 newPosition = { { 15.0, 0.0, 0.0 } };
					if (numberOfChildren > 0) {

						newPosition = m_minMaxGlyphTree->child(newParentGlyph, numberOfChildren - 1)->GetMinGlyph().GetPosition();
					}
					stlplus::ntree<SynGlyphX::DataMappingGlyph> oldGlyphSubtree = m_minMaxGlyphTree->cut(oldGlyph);
					SynGlyphX::DataMappingGlyphGraph::iterator newGlyph = m_minMaxGlyphTree->insert(newParentGlyph, oldGlyphSubtree);

					//For now, update position to 15.0 less than the last x coordinate.  This follows what ANTz does
					newPosition[0] -= 15.0;
					newGlyph->GetPosition()[0].GetValue().first = newPosition[0];
					newGlyph->GetPosition()[1].GetValue().first = newPosition[1];
					newGlyph->GetPosition()[2].GetValue().first = newPosition[2];

					endMoveRows();
				}
			}

			return true;
		}

		return false;
	}

	bool MinMaxGlyphTreeModel::LoadFromFile(const QString& filename) {

		beginResetModel();

		try {

			QString extension = filename.right(4);
			if (extension == ".sgt") {

				SynGlyphX::DataMappingGlyphFile fileReader;
				fileReader.ReadFromFile(filename.toStdString());
				m_minMaxGlyphTree = fileReader.GetDataMappingGlyphGraph();
			}
			else if (extension == ".csv") {

				m_minMaxGlyphTree = std::make_shared<SynGlyphX::DataMappingGlyphGraph>(*SynGlyphXANTz::GlyphNodeConverter::CreateGlyphGraphFromCSV(filename.toStdString()).get());
			}
			else {

				throw std::exception("File type not supported");
			}
		}
		catch (const std::exception& e) {

			m_minMaxGlyphTree->erase();
			endResetModel();
			throw;
		}

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
		csvWriter.Write(filename.toStdString(), "", trees, std::vector<ANTzGrid>());

		return true;
	}

	bool MinMaxGlyphTreeModel::IsClipboardEmpty() const {

		return (!m_clipboardGlyph.valid());
	}

	SynGlyphX::DataMappingGlyphGraph::iterator MinMaxGlyphTreeModel::GetClipboardGlyph() const {

		return m_clipboardGlyph;
	}

	void MinMaxGlyphTreeModel::CopyToClipboard(const QModelIndex& index, bool removeFromTree) {

		//m_clipboardGlyph.reset(new SynGlyphX::Glyph(index.))
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

	SynGlyphX::DataMappingGlyphGraph::iterator MinMaxGlyphTreeModel::GetIteratorFromIndex(const QModelIndex& index) {

		SynGlyphX::DataMappingGlyphGraph::iterator iT(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
		return iT;
	}

	unsigned int MinMaxGlyphTreeModel::GetBranchLevel(const QModelIndex& index) const {

		SynGlyphX::DataMappingGlyphGraph::iterator node = GetIteratorFromIndex(index);
		return m_minMaxGlyphTree->depth(node);
	}

	SynGlyphX::DataMappingGlyphGraph::const_iterator MinMaxGlyphTreeModel::GetMinMaxGlyph(const QModelIndex& index) const {

		SynGlyphX::DataMappingGlyphGraph::iterator iT = GetIteratorFromIndex(index);
		return iT.constify();
	}

} //namespace SynGlyphXANTz