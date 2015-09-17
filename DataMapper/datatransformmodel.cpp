#include "datatransformmodel.h"
#include "sourcedatamanager.h"
#include <QtCore/QDir>
#include <boost/uuid/uuid_io.hpp>
#include "application.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"
#include <boost/filesystem.hpp>
#include "datamappingglyphfile.h"
#include "glyphnodeconverter.h"
#include "glyphmimedata.h"
#include "defaultbaseimageproperties.h"
#include "defaultbaseimagescombobox.h"

DataTransformModel::DataTransformModel(QObject *parent)
	: QAbstractItemModel(parent),
	m_dataMapping(new SynGlyphX::DataTransformMapping())
{
	ClearAndReset();
}

DataTransformModel::~DataTransformModel()
{

}

QString DataTransformModel::GetCacheLocationForID(const boost::uuids::uuid& id) {

	return QDir::toNativeSeparators(SynGlyphX::Application::GetAppTempDirectory() + QDir::separator() + "cache_" + QString::fromStdString(boost::uuids::to_string(id)) + ".db");
}

int DataTransformModel::columnCount(const QModelIndex& parent) const {

	return 1;
}

bool DataTransformModel::setData(const QModelIndex& index, const QVariant& value, int role) {

	if ((role >= PropertyRole::PositionX) && (role <= PropertyRole::GeometryTorusRatio) && (GetDataType(index) == DataType::GlyphTrees)) {

		if (!m_dataMapping->GetGlyphGraphs().empty()) {

			SynGlyphX::DataMappingGlyphGraph::GlyphIterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));

			if (role == PropertyRole::PositionX) {

				iterator->second.GetPosition()[0] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::PositionY) {

				iterator->second.GetPosition()[1] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::PositionZ) {

				iterator->second.GetPosition()[2] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::RotationX) {

				iterator->second.GetRotation()[0] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::RotationY) {

				iterator->second.GetRotation()[1] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::RotationZ) {

				iterator->second.GetRotation()[2] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::ScaleX) {

				iterator->second.GetScale()[0] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::ScaleY) {

				iterator->second.GetScale()[1] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::ScaleZ) {

				iterator->second.GetScale()[2] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::Color) {

				iterator->second.GetColor() = value.value<SynGlyphX::ColorMappingProperty>();
			}
			else if (role == PropertyRole::Transparency) {

				iterator->second.GetTransparency() = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::Tag) {

				iterator->second.GetTag() = value.value<SynGlyphX::TextMappingProperty>();
			}
			else if (role == PropertyRole::Description) {

				iterator->second.GetDescription() = value.value<SynGlyphX::TextMappingProperty>();
			}
			else if (role == PropertyRole::RotationRateX) {

				iterator->second.GetRotationRate()[0] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::RotationRateY) {

				iterator->second.GetRotationRate()[1] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::RotationRateZ) {

				iterator->second.GetRotationRate()[2] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::GeometryShape) {

				iterator->second.GetStructure().SetGeometryShape(value.value<SynGlyphX::GeometryShapeMappingProperty>());
			}
			else if (role == PropertyRole::GeometrySurface) {

				iterator->second.GetStructure().SetGeometrySurface(static_cast<SynGlyphX::GlyphGeometryInfo::Surface>(value.toInt()));
			}
			else if (role == PropertyRole::GeometryTorusRatio) {

				iterator->second.GetStructure().SetTorusRatio(value.toDouble());
			}
			else if (role == PropertyRole::VirtualTopology) {

				iterator->second.GetVirtualTopology().SetType(value.value<SynGlyphX::VirtualTopologyMappingProperty>());
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

QVariant DataTransformModel::data(const QModelIndex& index, int role) const {

	if (!index.isValid()) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {

		return GetDisplayData(index);
	}
	else if (role == DataTypeRole) {

		return GetDataTypeData(index);
	}
	else if ((role >= PropertyRole::PositionX) && (role <= PropertyRole::GeometryTorusRatio)) {

		return GetPropertyData(index, role);
	}

	return QVariant();
}

QVariant DataTransformModel::GetPropertyData(const QModelIndex& index, int role) const {

	if ((role >= PropertyRole::PositionX) && (role <= PropertyRole::GeometryTorusRatio) && (GetDataType(index) == DataType::GlyphTrees)) {

		if (!m_dataMapping->GetGlyphGraphs().empty()) {

			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));

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

QVariant DataTransformModel::GetDisplayData(const QModelIndex& index) const {

	if (!index.parent().isValid()) {

		if (IsParentlessRowInDataType(DataType::BaseObjects, index.row())) {

			int baseObjectIndex = index.row() - m_dataMapping->GetGlyphGraphs().size();
			const SynGlyphX::BaseImage& baseObject = m_dataMapping->GetBaseObjects()[baseObjectIndex];
			SynGlyphX::BaseImage::Type baseImageType = baseObject.GetType();
			if (baseImageType == SynGlyphX::BaseImage::UserImage) {

				auto properties = std::dynamic_pointer_cast<const SynGlyphX::UserDefinedBaseImageProperties>(baseObject.GetProperties());
				QFileInfo baseObjectFileInfo(QString::fromStdWString(properties->GetFilename()));
				return baseObjectFileInfo.fileName();
			}
			else if (baseImageType == SynGlyphX::BaseImage::DownloadedMap) {

				auto properties = std::dynamic_pointer_cast<const SynGlyphX::DownloadedMapProperties>(baseObject.GetProperties());
				return QString::fromStdWString(L"Downloaded Map: " + SynGlyphX::DownloadedMapProperties::s_mapSourceStrings.left.at(properties->GetSource()) + L", " + SynGlyphX::DownloadedMapProperties::s_mapTypeStrings.left.at(properties->GetType()));
			}
			else {

				auto properties = std::dynamic_pointer_cast<const SynGlyphX::DefaultBaseImageProperties>(baseObject.GetProperties());
				return "Default: " + SynGlyphX::DefaultBaseImagesComboBox::s_typeToNameMap.at(properties->GetDefaultBaseImageType());
			}
		}
		else if (IsParentlessRowInDataType(DataType::DataSources, index.row())) {

			int datasourceIndex = index.row() - m_dataMapping->GetGlyphGraphs().size() - m_dataMapping->GetBaseObjects().size();
			SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator fileDatasource = m_dataMapping->GetDatasources().GetFileDatasources().begin();
			std::advance(fileDatasource, datasourceIndex);
			QFileInfo fileDatasourceFileInfo(QString::fromStdWString(fileDatasource->second.GetFilename()));
			return fileDatasourceFileInfo.fileName();
		}
		else if (IsParentlessRowInDataType(DataType::GlyphTrees, index.row())) {

			return GetGlyphData(index);
		}
	}
	else {

		return GetGlyphData(index);
	}

	return QVariant();
}

QVariant DataTransformModel::GetDataTypeData(const QModelIndex& index) const {

	return GetDataType(index);
}

DataTransformModel::DataType DataTransformModel::GetDataType(const QModelIndex& index) const {

	if (!index.parent().isValid()) {

		if (IsParentlessRowInDataType(DataType::BaseObjects, index.row())) {

			return DataType::BaseObjects;
		}
		else if (IsParentlessRowInDataType(DataType::DataSources, index.row())) {

			return DataType::DataSources;
		}
		else if (IsParentlessRowInDataType(DataType::GlyphTrees, index.row())) {

			return DataType::GlyphTrees;
		}
	}
	else {

		return DataType::GlyphTrees;
	}
}

QVariant DataTransformModel::GetGlyphData(const QModelIndex& index) const {

	if (!m_dataMapping->GetGlyphGraphs().empty()) {

		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));

		std::wstring glyphData = SynGlyphX::GlyphGeometryInfo::s_shapeNames.left.at(iterator->second.GetStructure().GetGeometryShape().GetValue()) + L": " + 
			SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.left.at(iterator->second.GetVirtualTopology().GetType().GetValue());
		if (iterator == iterator.owner()->root().constify()) {
			glyphData += L" (Root)";
		}

		return QString::fromStdWString(glyphData);
	}
	else {

		return QVariant();
	}
}

QModelIndex	DataTransformModel::index(int row, int column, const QModelIndex& parent) const{

	if (!hasIndex(row, column, parent)) {
		
		return QModelIndex();
	}

	if (!parent.isValid()) {

		if (IsParentlessRowInDataType(DataType::GlyphTrees, row)) {

			SynGlyphX::DataTransformMapping::DataMappingGlyphGraphMap::const_iterator iterator = m_dataMapping->GetGlyphGraphs().begin();
			std::advance(iterator, row);
			return createIndex(row, column, static_cast<void*>(iterator->second->GetRoot().node()));
		}
		else {

			return createIndex(row, column, nullptr);
		}
	}
	else {

		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator parentIterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator childIterator = parentIterator.owner()->child(parentIterator, row);
		if (childIterator.valid()) {

			return createIndex(row, column, static_cast<void*>(childIterator.node()));
		}
	}

	return QModelIndex();
}

QModelIndex	DataTransformModel::parent(const QModelIndex& index) const {

	if (!index.isValid()) {
		
		return QModelIndex();
	}

	if (index.internalPointer() == nullptr) {

		return QModelIndex();
	}

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	if (iterator != iterator.owner()->root()) {

		const SynGlyphX::DataMappingGlyphGraph* currentTree = static_cast<const SynGlyphX::DataMappingGlyphGraph*>(iterator.owner());
		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator parent = currentTree->GetParent(iterator);

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

			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator grandparent = currentTree->GetParent(parent);
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

int	DataTransformModel::rowCount(const QModelIndex& parent) const {

	if (!parent.isValid()) {

		return m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size() + m_dataMapping->GetDatasources().Count();
	}

	if (parent.internalPointer() != nullptr) {

		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
		return iterator.owner()->children(iterator);
	}
	
	return 0;
}

boost::uuids::uuid DataTransformModel::AddFileDatasource(SynGlyphX::FileDatasource::SourceType type, const std::wstring& name) {

	boost::uuids::uuid id = m_dataMapping->AddFileDatasource(type, name);
	m_sourceDataManager.AddDatabaseConnection(m_dataMapping->GetDatasources().GetFileDatasources().at(id), id);

	return id;
}
/*
void DataTransformModel::SetInputField(const boost::uuids::uuid& treeID, SynGlyphX::DataMappingGlyphGraph::const_iterator& node, SynGlyphX::DataMappingGlyph::MappableField field, const SynGlyphX::InputField& inputfield) {

	m_dataMapping->SetInputField(treeID, node, field, inputfield);
}*/

void DataTransformModel::SetInputField(const boost::uuids::uuid& treeID, const QModelIndex& index, SynGlyphX::DataMappingGlyph::MappableField field, const SynGlyphX::InputField& inputfield) {

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator node(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	//SetInputField(treeID, node, field, inputfield);
	m_dataMapping->SetInputField(treeID, node, field, inputfield);
	emit dataChanged(index, index);
}
/*
void DataTransformModel::ClearInputBinding(const boost::uuids::uuid& treeID, SynGlyphX::DataMappingGlyphGraph::const_iterator& node, SynGlyphX::DataMappingGlyph::MappableField field) {

	m_dataMapping->ClearInputBinding(treeID, node, field);
}*/

void DataTransformModel::ClearInputBinding(const boost::uuids::uuid& treeID, const QModelIndex& index, SynGlyphX::DataMappingGlyph::MappableField field) {

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator node(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	//ClearInputBinding(treeID, node, field);
	m_dataMapping->ClearInputBinding(treeID, node, field);
	emit dataChanged(index, index);
}

void DataTransformModel::EnableTables(const boost::uuids::uuid& id, const SynGlyphX::Datasource::TableNames& tables, bool enable) {

	m_dataMapping->EnableTables(id, tables, enable);
}

bool DataTransformModel::removeRows(int row, int count, const QModelIndex& parent) {

	if (count > 0) {

		int lastRow = row + count - 1;
		beginRemoveRows(parent, row, lastRow);

		if (parent.isValid()) {

			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator parentGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
			
			boost::uuids::uuid treeId = GetTreeId(parent);
			for (int i = lastRow; i >= row; --i) {

				m_dataMapping->RemoveGlyph(treeId, parentGlyph, i);
			}
			
		} else {

			
			for (int i = lastRow; i >= row; --i) {

				if (IsParentlessRowInDataType(DataType::GlyphTrees, i)) {

					m_dataMapping->RemoveGlyphTree(GetTreeId(i));
				}
				else if (IsParentlessRowInDataType(DataType::BaseObjects, i)) {

					m_dataMapping->RemoveBaseObject(i - m_dataMapping->GetGlyphGraphs().size());
				}
				else if (IsParentlessRowInDataType(DataType::DataSources, i)) {


				}
			}
		}
		endRemoveRows();
		
		return true;
	}

	return false;
}

void DataTransformModel::SetDefaults(const SynGlyphX::DataMappingDefaults& defaults) {

	m_dataMapping->SetDefaults(defaults);
}

void DataTransformModel::SetSceneProperties(const SynGlyphX::SceneProperties& sceneProperties) {

	m_dataMapping->SetSceneProperties(sceneProperties);
}

SynGlyphX::DataTransformMapping::ConstSharedPtr DataTransformModel::GetDataMapping() const {

	return m_dataMapping;
}

void DataTransformModel::LoadDataTransformFile(const QString& filename) {

	Clear();
	beginResetModel();
	m_dataMapping->ReadFromFile(filename.toStdString());
	m_sourceDataManager.SetCacheLocation(GetCacheLocationForID(m_dataMapping->GetID()));
	m_sourceDataManager.AddDatabaseConnections(m_dataMapping->GetDatasources());
	endResetModel();
}

void DataTransformModel::SaveDataTransformFile(const QString& filename) {

	m_dataMapping->WriteToFile(filename.toStdString());
}

void DataTransformModel::ClearAndReset() {

	Clear();
	m_sourceDataManager.SetCacheLocation(GetCacheLocationForID(m_dataMapping->GetID()));
}

void DataTransformModel::Clear() {

	beginResetModel();
	m_sourceDataManager.Clear();
	m_dataMapping->Clear();
	endResetModel();
}

void DataTransformModel::AddGlyphFile(const QString& filename) {

	SynGlyphX::DataMappingGlyphGraph::SharedPtr glyphTree;
	QString extension = filename.right(4);
	if (extension == ".csv") {

		glyphTree = std::make_shared<SynGlyphX::DataMappingGlyphGraph>(*SynGlyphXANTz::GlyphNodeConverter::CreateGlyphGraphFromCSV(filename.toStdString()).get());
	}
	else if (extension == ".sgt") {

		SynGlyphX::DataMappingGlyphFile fileReader;
		fileReader.ReadFromFile(filename.toStdString());
		glyphTree = fileReader.GetDataMappingGlyphGraph();
	}
	else {

		throw std::invalid_argument("File does not contain a glyph tree");
	}

	AddGlyphTree(glyphTree);
}

void DataTransformModel::AddGlyphTree(SynGlyphX::DataMappingGlyphGraph::SharedPtr glyphTree) {

	if (!glyphTree) {

		throw std::invalid_argument("Can't add empty glyph tree to data mapping");
	}

	int row = m_dataMapping->GetGlyphGraphs().size();
	beginInsertRows(QModelIndex(), row, row);
	m_dataMapping->AddGlyphTree(glyphTree);
	endInsertRows();
}

void DataTransformModel::SetBaseObject(unsigned int position, const SynGlyphX::BaseImage& baseImage) {

	m_dataMapping->SetBaseObject(position, baseImage);
	QModelIndex modelIndex = index(m_dataMapping->GetGlyphGraphs().size() + position);
	emit dataChanged(modelIndex, modelIndex);
}

void DataTransformModel::AddBaseObject(const SynGlyphX::BaseImage& baseImage) {

	int row = m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size();
	beginInsertRows(QModelIndex(), row, row);
	m_dataMapping->AddBaseObject(baseImage);
	endInsertRows();
}

bool DataTransformModel::IsParentlessRowInDataType(DataType type, int row) const {

	int min = 0;
	int max = 0;
	if (type == DataType::BaseObjects) {

		min = m_dataMapping->GetGlyphGraphs().size();
		max = min + m_dataMapping->GetBaseObjects().size();
	}
	else if (type == DataType::DataSources) {

		min = m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size();
		max = min + m_dataMapping->GetDatasources().GetFileDatasources().size();
	}
	else if (type == DataType::GlyphTrees) {

		max = m_dataMapping->GetGlyphGraphs().size();
	}

	return ((row >= min) && (row < max));
}

void DataTransformModel::UpdateGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyph& newGlyph) {

	if (!m_dataMapping->GetGlyphGraphs().empty() && index.isValid()) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator vertex(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
		m_dataMapping->UpdateGlyph(GetTreeId(index), vertex, newGlyph);
		emit dataChanged(index, index);
	}
}

void DataTransformModel::UpdateGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph& subgraph) {

	if (m_dataMapping->GetGlyphGraphs().empty() || !index.isValid()) {

		throw std::invalid_argument("Index given to MinMaxGlyphTreeModel::GetSubgraph is invalid");
	}

	SynGlyphX::DataMappingGlyphGraph& nonConstGraph = const_cast<SynGlyphX::DataMappingGlyphGraph&>(subgraph);

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator vertex(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	SynGlyphX::DataMappingGlyph glyph = nonConstGraph.GetRoot()->second;
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

void DataTransformModel::UpdateGlyphGeometry(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGeometry& structure) {

	if (!m_dataMapping->GetGlyphGraphs().empty() && index.isValid()) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
		glyph->second.GetStructure() = structure;
		emit dataChanged(index, index);
	}
}

void DataTransformModel::UpdateVirtualTopology(const QModelIndex& index, const SynGlyphX::DataMappingVirtualTopology& virtualTopology) {

	if (!m_dataMapping->GetGlyphGraphs().empty() && index.isValid()) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
		glyph->second.GetVirtualTopology() = virtualTopology;
		emit dataChanged(index, index);
	}
}

const SynGlyphX::DataMappingGlyph& DataTransformModel::GetGlyph(const QModelIndex& index) const {

	if (m_dataMapping->GetGlyphGraphs().empty() || !index.isValid()) {

		throw std::invalid_argument("Index doesn't exist in DataTransformModel");
	}

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	return glyph->second;
}

SynGlyphX::DataMappingGlyphGraph DataTransformModel::GetSubgraph(const QModelIndex& index, bool includeChildren) {

	if (m_dataMapping->GetGlyphGraphs().empty() || !index.isValid()) {

		throw std::invalid_argument("Index doesn't exist in DataTransformModel");
	}

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	return m_dataMapping->GetSubgraph(GetTreeId(index), glyph, includeChildren);
}

void DataTransformModel::AddChildGlyph(const QModelIndex& parent, const SynGlyphX::DataMappingGlyph& glyphTemplate, unsigned int numberOfChildren) {

	if (!parent.isValid()) {

		throw std::invalid_argument("Can't append children to invalid parent");
	}
	if (numberOfChildren == 0) {

		throw std::invalid_argument("Can't append 0 children");
	}

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator parentGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));

	unsigned int startingNumberOfChildren = rowCount(parent);
	beginInsertRows(parent, startingNumberOfChildren, startingNumberOfChildren + numberOfChildren - 1);
	m_dataMapping->AddChildGlyph(GetTreeId(parent), parentGlyph, glyphTemplate, numberOfChildren);
	endInsertRows();
}

void DataTransformModel::AddChildGlyphGraph(const QModelIndex& parent, const SynGlyphX::DataMappingGlyphGraph& graph) {

	if (!parent.isValid()) {

		throw std::invalid_argument("Can't append children to invalid parent");
	}

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator parentGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));

	unsigned int startingNumberOfChildren = rowCount(parent);
	beginInsertRows(parent, startingNumberOfChildren, startingNumberOfChildren);
	m_dataMapping->AddChildTree(GetTreeId(parent), parentGlyph, graph);
	endInsertRows();
}

void DataTransformModel::ResetDataMappingID() {

	m_dataMapping->ResetID();
}

boost::uuids::uuid DataTransformModel::GetTreeId(int row) const {

	SynGlyphX::DataTransformMapping::DataMappingGlyphGraphMap::const_iterator glyphTree = m_dataMapping->GetGlyphGraphs().begin();
	std::advance(glyphTree, row);
	return glyphTree->first;
}

boost::uuids::uuid DataTransformModel::GetTreeId(const QModelIndex& index) const {

	QModelIndex treeRootIndex = index;
	int rootRow = -1;
	while (treeRootIndex.isValid()) {

		rootRow = treeRootIndex.row();
		treeRootIndex = treeRootIndex.parent();
	}

	return GetTreeId(rootRow);
}

Qt::DropActions DataTransformModel::supportedDropActions() const {

	return Qt::MoveAction;
}

Qt::ItemFlags DataTransformModel::flags(const QModelIndex& index) const {

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

QStringList DataTransformModel::mimeTypes() const {

	QStringList types;
	types.push_back(SynGlyphX::GlyphMimeData::s_format);
	return types;
}

bool DataTransformModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const {

	const SynGlyphX::GlyphMimeData* glyphData = qobject_cast<const SynGlyphX::GlyphMimeData*>(data);

	if ((glyphData == NULL) || (GetDataType(parent) != DataType::GlyphTrees)) {

		return false;
	}

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator parentGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
	const QModelIndexList& indexes = glyphData->GetGlyphs();
	for (int j = 0; j < indexes.length(); ++j) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator mimeDataGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(indexes[j].internalPointer()));
		if (mimeDataGlyph.owner() != parentGlyph.owner()) {

			return false;
		}
	}

	return true;
}

QMimeData* DataTransformModel::mimeData(const QModelIndexList& indexes) const {

	SynGlyphX::GlyphMimeData* mimeData = new SynGlyphX::GlyphMimeData(indexes);
	return mimeData;
}

bool DataTransformModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {

	//canDropMimeData isn't being called when it should as per this bug:
	//https://bugreports.qt.io/browse/QTBUG-30534
	//Thus it is being called here until we upgrade to Qt 5.4.1 or later
	if (!canDropMimeData(data, action, row, column, parent)) {

		return false;
	}

	const SynGlyphX::GlyphMimeData* glyphData = qobject_cast<const SynGlyphX::GlyphMimeData*>(data);

	if ((glyphData != NULL) && (row == -1)) {

		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator newParentGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
		const QModelIndexList& indexes = glyphData->GetGlyphs();

		bool glyphsMoved = false;

		for (int j = 0; j < indexes.length(); ++j) {

			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator oldGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(indexes[j].internalPointer()));
			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator oldParentGlyph = oldGlyph.owner()->parent(oldGlyph);

			//Only drop if parents are different
			if (oldParentGlyph != newParentGlyph) {
				
				unsigned int numberOfChildren = newParentGlyph.owner()->children(newParentGlyph);

				//Only do an insert here.  The MoveAction will take care of deleting the old object
				beginInsertRows(parent, numberOfChildren, numberOfChildren);
				SynGlyphX::DataMappingGlyphGraph oldGlyphSubtree = GetSubgraph(indexes[j], true);
				m_dataMapping->AddChildTreeResetPosition(GetTreeId(parent), newParentGlyph.deconstify(), oldGlyphSubtree);
				endInsertRows();
				glyphsMoved = true;
			}
		}

		return glyphsMoved;
	}

	return false;
}

const boost::uuids::uuid& DataTransformModel::GetCacheConnectionID() const {

	return m_sourceDataManager.GetCSVCacheConnectionID();
}