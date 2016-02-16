#include "datamappingmodel.h"
#include <QtCore/QDir>
#include <boost/uuid/uuid_io.hpp>
#include "application.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"
#include <boost/filesystem.hpp>
#include "datamappingglyphfile.h"
#include "glyphmimedata.h"
#include "defaultbaseimageproperties.h"
#include "defaultbaseimagescombobox.h"

namespace SynGlyphX {

	DataMappingModel::DataMappingModel(QObject *parent)
		: QAbstractItemModel(parent),
		m_dataMapping(new DataTransformMapping())
	{
		Clear();
	}

	DataMappingModel::~DataMappingModel()
	{

	}

	QString DataMappingModel::GetCacheLocationForID(const boost::uuids::uuid& id) {

		return QDir::toNativeSeparators(Application::GetAppTempDirectory() + QDir::separator() + "cache_" + QString::fromStdString(boost::uuids::to_string(id)) + ".db");
	}

	int DataMappingModel::columnCount(const QModelIndex& parent) const {

		return 1;
	}

	bool DataMappingModel::setData(const QModelIndex& index, const QVariant& value, int role) {

		if ((role >= PropertyRole::PositionX) && (role <= PropertyRole::GeometryTorusRatio) && (GetDataType(index) == DataType::GlyphTrees)) {

			if (!m_dataMapping->GetGlyphGraphs().empty()) {

				DataMappingGlyphGraph::GlyphIterator iterator(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));

				if (role == PropertyRole::PositionX) {

					iterator->second.GetPosition()[0] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::PositionY) {

					iterator->second.GetPosition()[1] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::PositionZ) {

					iterator->second.GetPosition()[2] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::RotationX) {

					iterator->second.GetRotation()[0] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::RotationY) {

					iterator->second.GetRotation()[1] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::RotationZ) {

					iterator->second.GetRotation()[2] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::ScaleX) {

					iterator->second.GetScale()[0] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::ScaleY) {

					iterator->second.GetScale()[1] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::ScaleZ) {

					iterator->second.GetScale()[2] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::Color) {

					iterator->second.GetColor() = value.value<ColorMappingProperty>();
				}
				else if (role == PropertyRole::Transparency) {

					iterator->second.GetTransparency() = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::Tag) {

					iterator->second.GetTag() = value.value<TextMappingProperty>();
				}
				else if (role == PropertyRole::Description) {

					iterator->second.GetDescription() = value.value<TextMappingProperty>();
				}
				else if (role == PropertyRole::RotationRateX) {

					iterator->second.GetRotationRate()[0] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::RotationRateY) {

					iterator->second.GetRotationRate()[1] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::RotationRateZ) {

					iterator->second.GetRotationRate()[2] = value.value<NumericMappingProperty>();
				}
				else if (role == PropertyRole::GeometryShape) {

					iterator->second.GetStructure().SetGeometryShape(value.value<GeometryShapeMappingProperty>());
				}
				else if (role == PropertyRole::GeometrySurface) {

					iterator->second.GetStructure().SetGeometrySurface(static_cast<GlyphGeometryInfo::Surface>(value.toInt()));
				}
				else if (role == PropertyRole::GeometryTorusRatio) {

					iterator->second.GetStructure().SetTorusRatio(value.toDouble());
				}
				else if (role == PropertyRole::VirtualTopology) {

					iterator->second.GetVirtualTopology().SetType(value.value<VirtualTopologyMappingProperty>());
				}

				QVector<int> roles;
				roles.push_back(role);

				if ((role == PropertyRole::GeometryShape) || (role == PropertyRole::VirtualTopology)) {

					roles.push_back(Qt::DisplayRole);
				}

				emit dataChanged(index, index, roles);

				return true;
			}

		}

		return false;
	}

	QVariant DataMappingModel::data(const QModelIndex& index, int role) const {

		if (!index.isValid()) {
			return QVariant();
		}

		if (role == Qt::DisplayRole) {

			return GetDisplayData(index);
		}
		else if (role == UUIDRole) {

			if (!index.parent().isValid()) {

				if (IsParentlessRowInDataType(DataType::DataSources, index.row())) {

					auto datasource = m_dataMapping->GetDatasources().begin();
					std::advance(datasource, index.row() - GetFirstIndexForDataType(DataType::DataSources));
					return QString::fromStdString(boost::uuids::to_string(datasource->first));
				}
				else if (IsParentlessRowInDataType(DataType::GlyphTrees, index.row())) {

					auto glyphGraph = m_dataMapping->GetGlyphGraphs().begin();
					std::advance(glyphGraph, index.row() - GetFirstIndexForDataType(DataType::GlyphTrees));
					return QString::fromStdString(boost::uuids::to_string(glyphGraph->first));
				}
			}
		}
		else if (role == DataTypeRole) {

			return GetDataTypeData(index);
		}
		else if ((role >= PropertyRole::PositionX) && (role <= PropertyRole::GeometryTorusRatio)) {

			return GetPropertyData(index, role);
		}

		return QVariant();
	}

	QVariant DataMappingModel::GetPropertyData(const QModelIndex& index, int role) const {

		if ((role >= PropertyRole::PositionX) && (role <= PropertyRole::GeometryTorusRatio) && (GetDataType(index) == DataType::GlyphTrees)) {

			if (!m_dataMapping->GetGlyphGraphs().empty()) {

				DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));

				QVariant var;
				if (role == PropertyRole::PositionX) {

					var.setValue(iterator->second.GetPosition()[0]);
				}
				else if (role == PropertyRole::PositionY) {

					var.setValue(iterator->second.GetPosition()[1]);
				}
				else if (role == PropertyRole::PositionZ) {

					var.setValue(iterator->second.GetPosition()[2]);
				}
				else if (role == PropertyRole::RotationX) {

					var.setValue(iterator->second.GetRotation()[0]);
				}
				else if (role == PropertyRole::RotationY) {

					var.setValue(iterator->second.GetRotation()[1]);
				}
				else if (role == PropertyRole::RotationZ) {

					var.setValue(iterator->second.GetRotation()[2]);
				}
				else if (role == PropertyRole::ScaleX) {

					var.setValue(iterator->second.GetScale()[0]);
				}
				else if (role == PropertyRole::ScaleY) {

					var.setValue(iterator->second.GetScale()[1]);
				}
				else if (role == PropertyRole::ScaleZ) {

					var.setValue(iterator->second.GetScale()[2]);
				}
				else if (role == PropertyRole::Color) {

					var.setValue(iterator->second.GetColor());
				}
				else if (role == PropertyRole::Transparency) {

					var.setValue(iterator->second.GetTransparency());
				}
				else if (role == PropertyRole::Tag) {

					var.setValue(iterator->second.GetTag());
				}
				else if (role == PropertyRole::Description) {

					var.setValue(iterator->second.GetDescription());
				}
				else if (role == PropertyRole::RotationRateX) {

					var.setValue(iterator->second.GetRotationRate()[0]);
				}
				else if (role == PropertyRole::RotationRateY) {

					var.setValue(iterator->second.GetRotationRate()[1]);
				}
				else if (role == PropertyRole::RotationRateZ) {

					var.setValue(iterator->second.GetRotationRate()[2]);
				}
				else if (role == PropertyRole::GeometryShape) {

					var.setValue(iterator->second.GetStructure().GetGeometryShape());
				}
				else if (role == PropertyRole::GeometrySurface) {

					var.setValue(iterator->second.GetStructure().GetGeometrySurface());
				}
				else if (role == PropertyRole::GeometryTorusRatio) {

					var.setValue(iterator->second.GetStructure().GetTorusRatio());
				}
				else if (role == PropertyRole::VirtualTopology) {

					var.setValue(iterator->second.GetVirtualTopology().GetType());
				}

				if (var.isValid()) {

					return var;
				}
			}
		}

		return QVariant();
	}

	QVariant DataMappingModel::GetDisplayData(const QModelIndex& index) const {

		if (!index.parent().isValid()) {

			if (IsParentlessRowInDataType(DataType::BaseObjects, index.row())) {

				int baseObjectIndex = index.row() - m_dataMapping->GetGlyphGraphs().size();
				const BaseImage& baseObject = m_dataMapping->GetBaseObjects()[baseObjectIndex];
				BaseImage::Type baseImageType = baseObject.GetType();
				if (baseImageType == BaseImage::UserImage) {

					auto properties = std::dynamic_pointer_cast<const UserDefinedBaseImageProperties>(baseObject.GetProperties());
					QFileInfo baseObjectFileInfo(QString::fromStdWString(properties->GetFilename()));
					return baseObjectFileInfo.fileName();
				}
				else if (baseImageType == BaseImage::DownloadedMap) {

					auto properties = std::dynamic_pointer_cast<const DownloadedMapProperties>(baseObject.GetProperties());
					return QString::fromStdWString(L"Downloaded Map: " + DownloadedMapProperties::s_mapSourceStrings.left.at(properties->GetSource()) + L", " + DownloadedMapProperties::s_mapTypeStrings.left.at(properties->GetType()));
				}
				else {

					auto properties = std::dynamic_pointer_cast<const DefaultBaseImageProperties>(baseObject.GetProperties());
					return "Default: " + DefaultBaseImagesComboBox::s_typeToNameMap.at(properties->GetDefaultBaseImageType());
				}
			}
			else if (IsParentlessRowInDataType(DataType::DataSources, index.row())) {

				int datasourceIndex = index.row() - m_dataMapping->GetGlyphGraphs().size() - m_dataMapping->GetBaseObjects().size();
				DataTransformMapping::DatasourceMap::const_iterator datasource = m_dataMapping->GetDatasources().begin();
				std::advance(datasource, datasourceIndex);
				return QString::fromStdWString(datasource->second->GetFormattedName());
			}
			else if (IsParentlessRowInDataType(DataType::GlyphTrees, index.row())) {

				return GetGlyphData(index);
			}
			else if (IsParentlessRowInDataType(DataType::FieldGroups, index.row())) {

				int fieldGroupIndex = index.row() - (m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size() + m_dataMapping->GetDatasources().size());
				DataTransformMapping::FieldGroupMap::const_iterator fieldGroup = m_dataMapping->GetFieldGroupMap().begin();
				std::advance(fieldGroup, fieldGroupIndex);
				return QString::fromStdWString(fieldGroup->first);
			}
		}
		else {

			return GetGlyphData(index);
		}

		return QVariant();
	}

	QVariant DataMappingModel::GetDataTypeData(const QModelIndex& index) const {

		return GetDataType(index);
	}

	DataMappingModel::DataType DataMappingModel::GetDataType(const QModelIndex& index) const {

		if (!index.parent().isValid()) {

			if (IsParentlessRowInDataType(DataType::BaseObjects, index.row())) {

				return DataType::BaseObjects;
			}
			else if (IsParentlessRowInDataType(DataType::DataSources, index.row())) {

				return DataType::DataSources;
			}
			else if (IsParentlessRowInDataType(DataType::FieldGroups, index.row())) {

				return DataType::FieldGroups;
			}
			else if (IsParentlessRowInDataType(DataType::GlyphTrees, index.row())) {

				return DataType::GlyphTrees;
			}
		}
		else {

			return DataType::GlyphTrees;
		}
	}

	QVariant DataMappingModel::GetGlyphData(const QModelIndex& index) const {

		if (!m_dataMapping->GetGlyphGraphs().empty()) {

			DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));

			std::wstring glyphData = GlyphGeometryInfo::s_shapeNames.left.at(iterator->second.GetStructure().GetGeometryShape().GetValue()) + L": " +
				VirtualTopologyInfo::s_virtualTopologyNames.left.at(iterator->second.GetVirtualTopology().GetType().GetValue());
			if (iterator == iterator.owner()->root().constify()) {
				glyphData += L" (Root)";
			}

			return QString::fromStdWString(glyphData);
		}
		else {

			return QVariant();
		}
	}

	QModelIndex	DataMappingModel::index(int row, int column, const QModelIndex& parent) const{

		if (!hasIndex(row, column, parent)) {

			return QModelIndex();
		}

		if (!parent.isValid()) {

			if (IsParentlessRowInDataType(DataType::GlyphTrees, row)) {

				DataTransformMapping::DataMappingGlyphGraphMap::const_iterator iterator = m_dataMapping->GetGlyphGraphs().begin();
				std::advance(iterator, row);
				return createIndex(row, column, static_cast<void*>(iterator->second->GetRoot().node()));
			}
			else {

				return createIndex(row, column, nullptr);
			}
		}
		else {

			DataMappingGlyphGraph::ConstGlyphIterator parentIterator(static_cast<DataMappingGlyphGraph::Node*>(parent.internalPointer()));
			DataMappingGlyphGraph::ConstGlyphIterator childIterator = parentIterator.owner()->child(parentIterator, row);
			if (childIterator.valid()) {

				return createIndex(row, column, static_cast<void*>(childIterator.node()));
			}
		}

		return QModelIndex();
	}

	QModelIndex	DataMappingModel::parent(const QModelIndex& index) const {

		if (!index.isValid()) {

			return QModelIndex();
		}

		if (index.internalPointer() == nullptr) {

			return QModelIndex();
		}

		DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));
		if (iterator != iterator.owner()->root()) {

			const DataMappingGlyphGraph* currentTree = static_cast<const DataMappingGlyphGraph*>(iterator.owner());
			DataMappingGlyphGraph::ConstGlyphIterator parent = currentTree->GetParent(iterator);

			int row = 0;

			if (parent == currentTree->GetRoot()) {

				for (auto tree : m_dataMapping->GetGlyphGraphs()) {

					if (currentTree->GetRoot() == tree.second->GetRoot().constify()) {

						break;
					}
					++row;
				}
			}
			else {

				DataMappingGlyphGraph::ConstGlyphIterator grandparent = currentTree->GetParent(parent);
				if (grandparent.valid()) {

					for (int i = 0; i < currentTree->ChildCount(grandparent); ++i) {

						if (currentTree->GetChild(grandparent, i) == parent) {
							row = i;
							break;
						}
					}
				}
			}

			return createIndex(row, 0, static_cast<void*>(parent.node()));
		}

		return QModelIndex();
	}

	int	DataMappingModel::rowCount(const QModelIndex& parent) const {

		if (!parent.isValid()) {

			return m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size() + m_dataMapping->GetDatasources().size() + m_dataMapping->GetFieldGroupMap().size();
		}

		if (parent.internalPointer() != nullptr) {

			DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<DataMappingGlyphGraph::Node*>(parent.internalPointer()));
			return iterator.owner()->children(iterator);
		}

		return 0;
	}

	boost::uuids::uuid DataMappingModel::AddFileDatasource(FileDatasource::FileType type, const std::wstring& name) {

		int newRow = GetFirstIndexForDataType(DataType::DataSources) + m_dataMapping->GetDatasources().size();
		beginInsertRows(QModelIndex(), newRow, newRow);
		boost::uuids::uuid id = m_dataMapping->AddFileDatasource(type, name);
		endInsertRows();

		return id;
	}
	/*
	void DataMappingModel::SetInputField(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::const_iterator& node, DataMappingGlyph::MappableField field, const InputField& inputfield) {

	m_dataMapping->SetInputField(treeID, node, field, inputfield);
	}*/

	void DataMappingModel::SetInputField(const boost::uuids::uuid& treeID, const QModelIndex& index, DataMappingGlyph::MappableField field, const InputField& inputfield) {

		DataMappingGlyphGraph::ConstGlyphIterator node(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));
		//SetInputField(treeID, node, field, inputfield);
		m_dataMapping->SetInputField(treeID, node, field, inputfield);
		emit dataChanged(index, index);
	}
	/*
	void DataMappingModel::ClearInputBinding(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::const_iterator& node, DataMappingGlyph::MappableField field) {

	m_dataMapping->ClearInputBinding(treeID, node, field);
	}*/

	void DataMappingModel::ClearInputBinding(const boost::uuids::uuid& treeID, const QModelIndex& index, DataMappingGlyph::MappableField field) {

		DataMappingGlyphGraph::ConstGlyphIterator node(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));
		//ClearInputBinding(treeID, node, field);
		m_dataMapping->ClearInputBinding(treeID, node, field);
		emit dataChanged(index, index);
	}

	void DataMappingModel::ClearAllInputBindings(const boost::uuids::uuid& treeID, const QModelIndex& index) {

		DataMappingGlyphGraph::ConstGlyphIterator node(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));
		m_dataMapping->ClearAllInputBindings(treeID, node);
		emit dataChanged(index, index);
	}

	void DataMappingModel::EnableTables(const boost::uuids::uuid& id, const Datasource::TableNames& tables, bool enable) {

		m_dataMapping->EnableTables(id, tables, enable);
	}

	bool DataMappingModel::removeRows(int row, int count, const QModelIndex& parent) {

		if (count > 0) {

			int lastRow = row + count - 1;

			if (parent.isValid()) {

				DataMappingGlyphGraph::ConstGlyphIterator parentGlyph(static_cast<DataMappingGlyphGraph::Node*>(parent.internalPointer()));
				boost::uuids::uuid treeId = GetTreeId(parent);

				beginRemoveRows(parent, row, lastRow);
				for (int i = lastRow; i >= row; --i) {

					m_dataMapping->RemoveGlyph(treeId, parentGlyph, i);
				}
				endRemoveRows();

			}
			else {

				bool emitGlyphDataChanged = false;
				for (int i = lastRow; i >= row; --i) {

					if (IsParentlessRowInDataType(DataType::DataSources, i)) {

						emitGlyphDataChanged = true;

						//Clear all field groups that use the datasource before removing the datasource
						std::vector<DataTransformMapping::FieldGroupName> fieldGroupsToBeRemoved;
						for (auto& fieldGroup : m_dataMapping->GetFieldGroupMap()) {

							if (m_dataMapping->DoesFieldGroupHaveFieldsFromDatasource(fieldGroup.first, GetDatasourceId(i))) {

								fieldGroupsToBeRemoved.push_back(fieldGroup.first);
							}
						}

						for (const DataTransformMapping::FieldGroupName& name : fieldGroupsToBeRemoved) {

							RemoveFieldGroup(name, false);
						}
					}
				}

				beginRemoveRows(parent, row, lastRow);
				for (int i = lastRow; i >= row; --i) {

					if (IsParentlessRowInDataType(DataType::GlyphTrees, i)) {

						m_dataMapping->RemoveGlyphTree(GetTreeId(i));
					}
					else if (IsParentlessRowInDataType(DataType::BaseObjects, i)) {

						m_dataMapping->RemoveBaseObject(i - m_dataMapping->GetGlyphGraphs().size());
					}
					else if (IsParentlessRowInDataType(DataType::DataSources, i)) {

						boost::uuids::uuid id = GetDatasourceId(i);
						m_dataMapping->RemoveDatasource(id);
					}
					else if (IsParentlessRowInDataType(DataType::FieldGroups, i)) {

						m_dataMapping->RemoveFieldGroup(GetFieldGroupName(i));
						emitGlyphDataChanged = true;
					}
				}
				endRemoveRows();

				if (emitGlyphDataChanged) {

					//Since bindings could have been cleared due to datasources being removed, notify that the data of glyphs has been changed
					unsigned int firstGlyphIndex = GetFirstIndexForDataType(DataType::GlyphTrees);
					emit dataChanged(index(firstGlyphIndex), index(firstGlyphIndex + m_dataMapping->GetGlyphGraphs().size() - 1));
				}
			}

			return true;
		}

		return false;
	}

	void DataMappingModel::SetDefaults(const DataMappingDefaults& defaults) {

		m_dataMapping->SetDefaults(defaults);
	}

	void DataMappingModel::SetSceneProperties(const SceneProperties& sceneProperties) {

		m_dataMapping->SetSceneProperties(sceneProperties);
	}

	DataTransformMapping::ConstSharedPtr DataMappingModel::GetDataMapping() const {

		return m_dataMapping;
	}

	void DataMappingModel::LoadDataTransformFile(const QString& filename) {

		Clear();
		beginResetModel();
		m_dataMapping->ReadFromFile(filename.toStdString());
		endResetModel();
	}

	void DataMappingModel::SaveDataTransformFile(const QString& filename) {

		m_dataMapping->WriteToFile(filename.toStdString());
	}

	void DataMappingModel::Clear() {

		beginResetModel();
		m_dataMapping->Clear();
		endResetModel();
	}

	void DataMappingModel::AddGlyphFile(const QString& filename) {

		DataMappingGlyphGraph::SharedPtr glyphTree;
		QString extension = filename.right(4);
		if (extension == ".sgt") {

			DataMappingGlyphFile fileReader;
			fileReader.ReadFromFile(filename.toStdString());
			glyphTree = fileReader.GetDataMappingGlyphGraph();
		}
		else {

			throw std::invalid_argument("File does not contain a glyph tree");
		}

		AddGlyphTree(glyphTree);
	}

	void DataMappingModel::AddGlyphTree(DataMappingGlyphGraph::SharedPtr glyphTree) {

		if (!glyphTree) {

			throw std::invalid_argument("Can't add empty glyph tree to data mapping");
		}

		int row = m_dataMapping->GetGlyphGraphs().size();
		beginInsertRows(QModelIndex(), row, row);
		m_dataMapping->AddGlyphTree(glyphTree);
		endInsertRows();
	}

	void DataMappingModel::SetBaseObject(unsigned int position, const BaseImage& baseImage) {

		m_dataMapping->SetBaseObject(position, baseImage);
		QModelIndex modelIndex = index(m_dataMapping->GetGlyphGraphs().size() + position);
		emit dataChanged(modelIndex, modelIndex);
	}

	void DataMappingModel::AddBaseObject(const BaseImage& baseImage) {

		int row = m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size();
		beginInsertRows(QModelIndex(), row, row);
		m_dataMapping->AddBaseObject(baseImage);
		endInsertRows();
	}

	bool DataMappingModel::IsParentlessRowInDataType(DataType type, int row) const {

		int min = GetFirstIndexForDataType(type);
		int max = 0;
		if (type == DataType::BaseObjects) {

			max = min + m_dataMapping->GetBaseObjects().size();
		}
		else if (type == DataType::DataSources) {

			max = min + m_dataMapping->GetDatasources().size();
		}
		else if (type == DataType::FieldGroups) {

			max = min + m_dataMapping->GetFieldGroupMap().size();
		}
		else if (type == DataType::GlyphTrees) {

			max = min + m_dataMapping->GetGlyphGraphs().size();
		}

		return ((row >= min) && (row < max));
	}

	unsigned int DataMappingModel::GetFirstIndexForDataType(DataType type) const {

		if (type == DataType::BaseObjects) {

			return m_dataMapping->GetGlyphGraphs().size();
		}
		else if (type == DataType::DataSources) {

			return m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size();
		}
		else if (type == DataType::FieldGroups) {

			return m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size() + m_dataMapping->GetDatasources().size();
		}
		else if (type == DataType::GlyphTrees) {

			return 0;
		}
	}

	void DataMappingModel::UpdateGlyph(const QModelIndex& index, const DataMappingGlyph& newGlyph) {

		if (!m_dataMapping->GetGlyphGraphs().empty() && index.isValid()) {

			DataMappingGlyphGraph::GlyphIterator vertex(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));
			m_dataMapping->UpdateGlyph(GetTreeId(index), vertex, newGlyph);
			emit dataChanged(index, index);
		}
	}

	void DataMappingModel::UpdateGlyph(const QModelIndex& index, const DataMappingGlyphGraph& subgraph) {

		if (m_dataMapping->GetGlyphGraphs().empty() || !index.isValid()) {

			throw std::invalid_argument("Index given to MinMaxGlyphTreeModel::GetSubgraph is invalid");
		}

		DataMappingGlyphGraph& nonConstGraph = const_cast<DataMappingGlyphGraph&>(subgraph);

		DataMappingGlyphGraph::GlyphIterator vertex(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));
		DataMappingGlyph glyph = nonConstGraph.GetRoot()->second;
		glyph.GetPosition() = vertex->second.GetPosition();
		UpdateGlyph(index, glyph);

		unsigned int numberOfChildrenOfRootInSubgraph = subgraph.ChildCount(subgraph.GetRoot());
		if (numberOfChildrenOfRootInSubgraph > 0) {

			boost::uuids::uuid treeId = GetTreeId(index);
			unsigned int numberOfChildrenInNewParent = rowCount(index);
			beginInsertRows(index, numberOfChildrenInNewParent, numberOfChildrenInNewParent + numberOfChildrenOfRootInSubgraph - 1);

			for (int i = 0; i < numberOfChildrenOfRootInSubgraph; ++i) {

				m_dataMapping->AddChildTree(treeId, vertex, nonConstGraph.GetSubgraph(nonConstGraph.GetChild(nonConstGraph.GetRoot(), i), true));
			}

			endInsertRows();
		}
	}

	void DataMappingModel::UpdateGlyphGeometry(const QModelIndex& index, const DataMappingGlyphGeometry& structure) {

		if (!m_dataMapping->GetGlyphGraphs().empty() && index.isValid()) {

			DataMappingGlyphGraph::GlyphIterator glyph(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));
			glyph->second.GetStructure() = structure;
			emit dataChanged(index, index);
		}
	}

	void DataMappingModel::UpdateVirtualTopology(const QModelIndex& index, const DataMappingVirtualTopology& virtualTopology) {

		if (!m_dataMapping->GetGlyphGraphs().empty() && index.isValid()) {

			DataMappingGlyphGraph::GlyphIterator glyph(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));
			glyph->second.GetVirtualTopology() = virtualTopology;
			emit dataChanged(index, index);
		}
	}

	const DataMappingGlyph& DataMappingModel::GetGlyph(const QModelIndex& index) const {

		if (m_dataMapping->GetGlyphGraphs().empty() || !index.isValid()) {

			throw std::invalid_argument("Index doesn't exist in DataMappingModel");
		}

		DataMappingGlyphGraph::ConstGlyphIterator glyph(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));
		return glyph->second;
	}

	DataMappingGlyphGraph DataMappingModel::GetSubgraph(const QModelIndex& index, bool includeChildren) {

		if (m_dataMapping->GetGlyphGraphs().empty() || !index.isValid()) {

			throw std::invalid_argument("Index doesn't exist in DataMappingModel");
		}

		DataMappingGlyphGraph::ConstGlyphIterator glyph(static_cast<DataMappingGlyphGraph::Node*>(index.internalPointer()));
		return m_dataMapping->GetSubgraph(GetTreeId(index), glyph, includeChildren);
	}

	void DataMappingModel::AddChildGlyph(const QModelIndex& parent, const DataMappingGlyph& glyphTemplate, unsigned int numberOfChildren) {

		if (!parent.isValid()) {

			throw std::invalid_argument("Can't append children to invalid parent");
		}
		if (numberOfChildren == 0) {

			throw std::invalid_argument("Can't append 0 children");
		}

		DataMappingGlyphGraph::GlyphIterator parentGlyph(static_cast<DataMappingGlyphGraph::Node*>(parent.internalPointer()));

		unsigned int startingNumberOfChildren = rowCount(parent);
		beginInsertRows(parent, startingNumberOfChildren, startingNumberOfChildren + numberOfChildren - 1);
		m_dataMapping->AddChildGlyph(GetTreeId(parent), parentGlyph, glyphTemplate, numberOfChildren);
		endInsertRows();
	}

	void DataMappingModel::AddChildGlyphGraph(const QModelIndex& parent, const DataMappingGlyphGraph& graph) {

		if (!parent.isValid()) {

			throw std::invalid_argument("Can't append children to invalid parent");
		}

		DataMappingGlyphGraph::GlyphIterator parentGlyph(static_cast<DataMappingGlyphGraph::Node*>(parent.internalPointer()));

		unsigned int startingNumberOfChildren = rowCount(parent);
		beginInsertRows(parent, startingNumberOfChildren, startingNumberOfChildren);
		m_dataMapping->AddChildTree(GetTreeId(parent), parentGlyph, graph);
		endInsertRows();
	}

	void DataMappingModel::ResetDataMappingID() {

		m_dataMapping->ResetID();
	}

	boost::uuids::uuid DataMappingModel::GetTreeId(int row) const {

		DataTransformMapping::DataMappingGlyphGraphMap::const_iterator glyphTree = m_dataMapping->GetGlyphGraphs().begin();
		std::advance(glyphTree, row);
		return glyphTree->first;
	}

	boost::uuids::uuid DataMappingModel::GetTreeId(const QModelIndex& index) const {

		QModelIndex treeRootIndex = index;
		int rootRow = -1;
		while (treeRootIndex.isValid()) {

			rootRow = treeRootIndex.row();
			treeRootIndex = treeRootIndex.parent();
		}

		return GetTreeId(rootRow);
	}

	boost::uuids::uuid DataMappingModel::GetDatasourceId(int row) const {

		DataTransformMapping::DatasourceMap::const_iterator datasource = m_dataMapping->GetDatasources().begin();
		std::advance(datasource, row - GetFirstIndexForDataType(DataType::DataSources));
		return datasource->first;
	}

	Qt::DropActions DataMappingModel::supportedDropActions() const {

		return Qt::MoveAction;
	}

	Qt::ItemFlags DataMappingModel::flags(const QModelIndex& index) const {

		if (!index.isValid()) {
			return 0;
		}

		Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

		if (GetDataType(index) == DataType::GlyphTrees) {

			flags |= Qt::ItemIsDropEnabled;

			if (index.parent().isValid()) {

				flags |= Qt::ItemIsDragEnabled;
			}
		}

		return flags;
	}

	QStringList DataMappingModel::mimeTypes() const {

		QStringList types;
		types.push_back(GlyphMimeData::s_format);
		return types;
	}

	bool DataMappingModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const {

		const GlyphMimeData* glyphData = qobject_cast<const GlyphMimeData*>(data);

		if ((glyphData == NULL) || (GetDataType(parent) != DataType::GlyphTrees)) {

			return false;
		}

		DataMappingGlyphGraph::GlyphIterator parentGlyph(static_cast<DataMappingGlyphGraph::Node*>(parent.internalPointer()));
		const QModelIndexList& indexes = glyphData->GetGlyphs();
		for (int j = 0; j < indexes.length(); ++j) {

			DataMappingGlyphGraph::GlyphIterator mimeDataGlyph(static_cast<DataMappingGlyphGraph::Node*>(indexes[j].internalPointer()));
			if (mimeDataGlyph.owner() != parentGlyph.owner()) {

				return false;
			}
		}

		return true;
	}

	QMimeData* DataMappingModel::mimeData(const QModelIndexList& indexes) const {

		GlyphMimeData* mimeData = new GlyphMimeData(indexes);
		return mimeData;
	}

	bool DataMappingModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {

		//canDropMimeData isn't being called when it should as per this bug:
		//https://bugreports.qt.io/browse/QTBUG-30534
		//Thus it is being called here until we upgrade to Qt 5.4.1 or later
		if (!canDropMimeData(data, action, row, column, parent)) {

			return false;
		}

		const GlyphMimeData* glyphData = qobject_cast<const GlyphMimeData*>(data);

		if ((glyphData != NULL) && (row == -1)) {

			DataMappingGlyphGraph::ConstGlyphIterator newParentGlyph(static_cast<DataMappingGlyphGraph::Node*>(parent.internalPointer()));
			const QModelIndexList& indexes = glyphData->GetGlyphs();

			bool glyphsMoved = false;

			for (int j = 0; j < indexes.length(); ++j) {

				DataMappingGlyphGraph::ConstGlyphIterator oldGlyph(static_cast<DataMappingGlyphGraph::Node*>(indexes[j].internalPointer()));
				DataMappingGlyphGraph::ConstGlyphIterator oldParentGlyph = oldGlyph.owner()->parent(oldGlyph);

				//Only drop if parents are different
				if (oldParentGlyph != newParentGlyph) {

					unsigned int numberOfChildren = newParentGlyph.owner()->children(newParentGlyph);

					//Only do an insert here.  The MoveAction will take care of deleting the old object
					beginInsertRows(parent, numberOfChildren, numberOfChildren);
					DataMappingGlyphGraph oldGlyphSubtree = GetSubgraph(indexes[j], true);
					m_dataMapping->AddChildTreeResetPosition(GetTreeId(parent), newParentGlyph.deconstify(), oldGlyphSubtree);
					endInsertRows();
					glyphsMoved = true;
				}
			}

			return glyphsMoved;
		}

		return false;
	}

	const DataTransformMapping::FieldGroupMap& DataMappingModel::GetFieldGroupMap() const {

		return m_dataMapping->GetFieldGroupMap();
	}

	void DataMappingModel::UpdateFieldGroup(const DataTransformMapping::FieldGroupName& groupName, const FieldGroup& fieldGroup) {

		const DataTransformMapping::FieldGroupMap& fieldGroupMap = m_dataMapping->GetFieldGroupMap();
		DataTransformMapping::FieldGroupMap::const_iterator groupIterator = fieldGroupMap.find(groupName);
		bool addNew = (groupIterator == fieldGroupMap.end());

		int firstIndex = GetFirstIndexForDataType(DataType::FieldGroups);
		int lastIndex = firstIndex + fieldGroupMap.size() - 1;
		if (addNew) {

			lastIndex++;
			beginInsertRows(QModelIndex(), lastIndex, lastIndex);
		}

		m_dataMapping->UpdateFieldGroup(groupName, fieldGroup);

		if (addNew) {

			endInsertRows();
		}

		emit dataChanged(index(firstIndex), index(lastIndex));
	}

	void DataMappingModel::RemoveFieldGroup(const DataTransformMapping::FieldGroupName& groupName) {

		RemoveFieldGroup(groupName, true);
	}

	void DataMappingModel::RemoveFieldGroup(const DataTransformMapping::FieldGroupName& groupName, bool emitGlyphDataChanged) {

		const DataTransformMapping::FieldGroupMap& fieldGroupMap = m_dataMapping->GetFieldGroupMap();
		DataTransformMapping::FieldGroupMap::const_iterator groupIterator = fieldGroupMap.find(groupName);

		if (groupIterator == fieldGroupMap.end()) {

			return;
		}

		int row = GetFirstIndexForDataType(DataType::FieldGroups) + std::distance(fieldGroupMap.begin(), groupIterator);

		beginRemoveRows(QModelIndex(), row, row);
		m_dataMapping->RemoveFieldGroup(groupName);
		endRemoveRows();

		if (emitGlyphDataChanged) {

			//Since references to the removed field group could have been cleared, notify that the data of glyphs has been changed
			unsigned int firstGlyphIndex = GetFirstIndexForDataType(DataType::GlyphTrees);
			emit dataChanged(index(firstGlyphIndex), index(firstGlyphIndex + m_dataMapping->GetGlyphGraphs().size() - 1));
		}
	}

	const DataTransformMapping::FieldGroupName& DataMappingModel::GetFieldGroupName(int row) const {

		DataTransformMapping::FieldGroupMap::const_iterator fieldGroup = m_dataMapping->GetFieldGroupMap().begin();
		std::advance(fieldGroup, row - GetFirstIndexForDataType(DataType::FieldGroups));
		return fieldGroup->first;
	}

} //namespace SynGlyphX