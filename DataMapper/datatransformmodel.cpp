#include "datatransformmodel.h"
#include "sourcedatamanager.h"
#include <QtCore/QDir>
#include <boost/uuid/uuid_io.hpp>
#include "application.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"
#include <boost/filesystem.hpp>
#include "datamappingglyphfile.h"

DataTransformModel::DataTransformModel(QObject *parent)
	: QAbstractItemModel(parent),
	m_dataMapping(new SynGlyphX::DataTransformMapping())
{
	SetIntermediateDirectoryToCurrentID();
}

DataTransformModel::~DataTransformModel()
{

}

void DataTransformModel::SetIntermediateDirectoryToCurrentID() {

	SynGlyphX::SourceDataManager::SetIntermediateDirectory(GetIntermediateDirectoryForID(m_dataMapping->GetID()));
}

QString DataTransformModel::GetIntermediateDirectoryForID(const boost::uuids::uuid& id) {

	return QDir::toNativeSeparators(QDir::tempPath() + QDir::separator() + SynGlyphX::Application::applicationName() + QDir::separator() + "int_" + QString::fromStdString(boost::uuids::to_string(id)));
}

int DataTransformModel::columnCount(const QModelIndex& parent) const {

	return 1;
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

	return QVariant();
}

QVariant DataTransformModel::GetDisplayData(const QModelIndex& index) const {

	if (!index.parent().isValid()) {

		if (IsRowInDataType(DataType::BaseObjects, index.row())) {

			int baseObjectIndex = index.row() - m_dataMapping->GetGlyphGraphs().size();
			const SynGlyphX::BaseImage& baseObject = m_dataMapping->GetBaseObjects()[baseObjectIndex];
			SynGlyphX::BaseImage::Type baseImageType = baseObject.GetType();
			if (baseImageType == SynGlyphX::BaseImage::UserImage) {

				const SynGlyphX::UserDefinedBaseImageProperties* const properties = static_cast<const SynGlyphX::UserDefinedBaseImageProperties* const>(baseObject.GetProperties());
				QFileInfo baseObjectFileInfo(QString::fromStdWString(properties->GetFilename()));
				return baseObjectFileInfo.fileName();
			}
			else if (baseImageType == SynGlyphX::BaseImage::DownloadedMap) {

				const SynGlyphX::DownloadedMapProperties* const properties = static_cast<const SynGlyphX::DownloadedMapProperties* const>(baseObject.GetProperties());
				return QString::fromStdWString(L"Downloaded Map: " + SynGlyphX::DownloadedMapProperties::s_mapSourceStrings.left.at(properties->GetSource()) + L", " + SynGlyphX::DownloadedMapProperties::s_mapTypeStrings.left.at(properties->GetType()));
			}
			else {

				return "Default";
			}
		}
		else if (IsRowInDataType(DataType::DataSources, index.row())) {

			int datasourceIndex = index.row() - m_dataMapping->GetGlyphGraphs().size() - m_dataMapping->GetBaseObjects().size();
			SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator fileDatasource = m_dataMapping->GetDatasources().GetFileDatasources().begin();
			std::advance(fileDatasource, datasourceIndex);
			QFileInfo fileDatasourceFileInfo(QString::fromStdWString(fileDatasource->second.GetFilename()));
			return fileDatasourceFileInfo.fileName();
		}
		else if (IsRowInDataType(DataType::GlyphTrees, index.row())) {

			return GetGlyphData(index);
		}
	}
	else {

		return GetGlyphData(index);
	}

	return QVariant();
}

QVariant DataTransformModel::GetDataTypeData(const QModelIndex& index) const {

	if (!index.parent().isValid()) {

		if (IsRowInDataType(DataType::BaseObjects, index.row())) {

			return DataType::BaseObjects;
		}
		else if (IsRowInDataType(DataType::DataSources, index.row())) {

			return DataType::DataSources;
		}
		else if (IsRowInDataType(DataType::GlyphTrees, index.row())) {

			return DataType::GlyphTrees;
		}
	}
	else {

		return DataType::GlyphTrees;
	}
}

QVariant DataTransformModel::GetGlyphData(const QModelIndex& index) const {

	if (!m_dataMapping->GetGlyphGraphs().empty()) {

		SynGlyphX::DataMappingGlyphGraph::const_iterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));

		std::wstring glyphData = SynGlyphX::GlyphStructuralProperties::s_shapeNames.left.at(iterator->GetStructure().GetGeometryShape()) + L": " + 
			SynGlyphX::GlyphStructuralProperties::s_virtualTopologyNames.left.at(iterator->GetStructure().GetVirtualTopology());
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

		if (IsRowInDataType(DataType::GlyphTrees, row)) {

			SynGlyphX::DataTransformMapping::DataMappingGlyphGraphMap::const_iterator iterator = m_dataMapping->GetGlyphGraphs().begin();
			std::advance(iterator, row);
			return createIndex(row, column, static_cast<void*>(iterator->second->root().node()));
		}
		else {

			return createIndex(row, column, nullptr);
		}
	}
	else {

		SynGlyphX::DataMappingGlyphGraph::const_iterator parentIterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
		SynGlyphX::DataMappingGlyphGraph::const_iterator childIterator = parentIterator.owner()->child(parentIterator, row);
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

	SynGlyphX::DataMappingGlyphGraph::const_iterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	if (iterator != iterator.owner()->root()) {

		const SynGlyphX::DataMappingGlyphGraph* currentTree = static_cast<const SynGlyphX::DataMappingGlyphGraph*>(iterator.owner());
		SynGlyphX::DataMappingGlyphGraph::const_iterator parent = currentTree->parent(iterator);

		int row = 0;

		if (parent == currentTree->root()) {

			for (auto tree : m_dataMapping->GetGlyphGraphs()) {

				if (currentTree->root() == tree.second->root().constify()) {

					break;
				}
				++row;
			}
		}
		else {

			SynGlyphX::DataMappingGlyphGraph::const_iterator grandparent = currentTree->parent(parent);
			if (grandparent.valid()) {

				for (int i = 0; i < currentTree->children(grandparent); ++i) {

					if (currentTree->child(grandparent, i) == parent) {
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

		SynGlyphX::DataMappingGlyphGraph::const_iterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
		return iterator.owner()->children(iterator);
	}
	
	return 0;
}

boost::uuids::uuid DataTransformModel::AddFileDatasource(SynGlyphX::FileDatasource::SourceType type, const std::wstring& name) {

	boost::uuids::uuid id = m_dataMapping->AddFileDatasource(type, name);
	m_sourceDataManager.AddDatabaseConnection(m_dataMapping->GetDatasources().GetFileDatasources().at(id), id);

	return id;
}

void DataTransformModel::SetInputField(const boost::uuids::uuid& treeID, SynGlyphX::DataMappingGlyphGraph::const_iterator& node, SynGlyphX::DataMappingGlyph::MappableField field, const SynGlyphX::InputField& inputfield) {

	m_dataMapping->SetInputField(treeID, node, field, inputfield);
}

void DataTransformModel::ClearInputBinding(const boost::uuids::uuid& treeID, SynGlyphX::DataMappingGlyphGraph::const_iterator& node, SynGlyphX::DataMappingGlyph::MappableField field) {

	m_dataMapping->ClearInputBinding(treeID, node, field);
}

void DataTransformModel::EnableTables(const boost::uuids::uuid& id, const SynGlyphX::Datasource::TableSet& tables, bool enable) {

	m_dataMapping->EnableTables(id, tables, enable);
}

bool DataTransformModel::removeRows(int row, int count, const QModelIndex& parent) {

	if ((!parent.isValid()) && (count > 0)) {

		int lastRow = row + count - 1;
		beginRemoveRows(parent, row, lastRow);

		for (int i = row; i <= lastRow; ++i) {

			if (IsRowInDataType(DataType::GlyphTrees, i)) {

				SynGlyphX::DataTransformMapping::DataMappingGlyphGraphMap::const_iterator glyphTree = m_dataMapping->GetGlyphGraphs().begin();
				std::advance(glyphTree, i);
				m_dataMapping->RemoveGlyphTree(glyphTree->first);
			}
			else if (IsRowInDataType(DataType::BaseObjects, i)) {

				m_dataMapping->RemoveBaseObject(i - m_dataMapping->GetGlyphGraphs().size());
			}
			else if (IsRowInDataType(DataType::DataSources, i)) {


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

SynGlyphX::DataTransformMapping::ConstSharedPtr DataTransformModel::GetDataMapping() const {

	return m_dataMapping;
}

void DataTransformModel::LoadDataTransformFile(const QString& filename) {

	Clear();
	beginResetModel();
	m_dataMapping->ReadFromFile(filename.toStdString());
	SetIntermediateDirectoryToCurrentID();
	m_sourceDataManager.AddDatabaseConnections(m_dataMapping->GetDatasources());
	endResetModel();
}

void DataTransformModel::SaveDataTransformFile(const QString& filename) {

	m_dataMapping->WriteToFile(filename.toStdString());
}

void DataTransformModel::Clear() {

	beginResetModel();
	m_sourceDataManager.ClearDatabaseConnections();
	m_dataMapping->Clear();
	endResetModel();
	SetIntermediateDirectoryToCurrentID();
}

void DataTransformModel::AddGlyphFile(const QString& filename) {

	SynGlyphX::DataMappingGlyphFile fileReader;
	fileReader.ReadFromFile(filename.toStdString());
	SynGlyphX::DataMappingGlyphGraph::SharedPtr glyphTree = fileReader.GetDataMappingGlyphGraph();
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

bool DataTransformModel::IsRowInDataType(DataType type, int row) const {

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

		SynGlyphX::DataMappingGlyphGraph::iterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
		*glyph = newGlyph;
		emit dataChanged(index, index);
	}
}

void DataTransformModel::UpdateGlyphStructure(const QModelIndex& index, const SynGlyphX::GlyphStructuralProperties& structure) {

	if (!m_dataMapping->GetGlyphGraphs().empty() && index.isValid()) {

		SynGlyphX::DataMappingGlyphGraph::iterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
		glyph->GetStructure() = structure;
		emit dataChanged(index, index);
	}
}

const SynGlyphX::DataMappingGlyph& DataTransformModel::GetGlyph(const QModelIndex& index) const {

	if (m_dataMapping->GetGlyphGraphs().empty() || !index.isValid()) {

		throw std::invalid_argument("Index doesn't exist in DataTransformModel");
	}

	SynGlyphX::DataMappingGlyphGraph::const_iterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	return (*glyph);
}

void DataTransformModel::ResetDataMappingID() {

	m_dataMapping->ResetID();
}