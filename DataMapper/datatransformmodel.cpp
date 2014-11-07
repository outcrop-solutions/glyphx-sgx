#include "datatransformmodel.h"
#include "sourcedatamanager.h"
#include <QtCore/QDir>
#include <boost/uuid/uuid_io.hpp>
#include "application.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"
#include <boost/filesystem.hpp>

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

			int baseObjectIndex = index.row() - m_dataMapping->GetGlyphTrees().size();
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

			int datasourceIndex = index.row() - m_dataMapping->GetGlyphTrees().size() - m_dataMapping->GetBaseObjects().size();
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

	if (!m_dataMapping->GetGlyphTrees().empty()) {

		SynGlyphX::MinMaxGlyphTree::const_iterator iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(index.internalPointer()));
		const SynGlyphX::GlyphProperties& minGlyph = iterator->GetMinGlyph();

		std::wstring glyphData = SynGlyphX::GlyphProperties::s_shapeNames.left.at(minGlyph.GetShape()) + L": " + SynGlyphX::GlyphProperties::s_topologyNames.left.at(minGlyph.GetTopology());
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

			SynGlyphX::DataTransformMapping::MinMaxGlyphTreeMap::const_iterator iterator = m_dataMapping->GetGlyphTrees().begin();
			std::advance(iterator, row);
			return createIndex(row, column, static_cast<void*>(iterator->second->root().node()));
		}
		else {

			return createIndex(row, column, nullptr);
		}
	}
	else {

		SynGlyphX::MinMaxGlyphTree::const_iterator parentIterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(parent.internalPointer()));
		SynGlyphX::MinMaxGlyphTree::const_iterator childIterator = parentIterator.owner()->child(parentIterator, row);
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

	SynGlyphX::MinMaxGlyphTree::const_iterator iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(index.internalPointer()));
	if (iterator != iterator.owner()->root()) {

		const SynGlyphX::MinMaxGlyphTree* currentTree = static_cast<const SynGlyphX::MinMaxGlyphTree*>(iterator.owner());
		SynGlyphX::MinMaxGlyphTree::const_iterator parent = currentTree->parent(iterator);

		int row = 0;

		if (parent == currentTree->root()) {

			for (auto tree : m_dataMapping->GetGlyphTrees()) {

				if (currentTree->root() == tree.second->root().constify()) {

					break;
				}
				++row;
			}
		}
		else {

			SynGlyphX::MinMaxGlyphTree::const_iterator grandparent = currentTree->parent(parent);
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

		return m_dataMapping->GetGlyphTrees().size() + m_dataMapping->GetBaseObjects().size() + m_dataMapping->GetDatasources().Count();
	}

	if (parent.internalPointer() != nullptr) {

		SynGlyphX::MinMaxGlyphTree::const_iterator iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(parent.internalPointer()));
		return iterator.owner()->children(iterator);
	}
	
	return 0;
}

boost::uuids::uuid DataTransformModel::AddFileDatasource(SynGlyphX::FileDatasource::SourceType type, const std::wstring& name) {

	boost::uuids::uuid id = m_dataMapping->AddFileDatasource(type, name);
	m_sourceDataManager.AddDatabaseConnection(m_dataMapping->GetDatasources().GetFileDatasources().at(id), id);

	return id;
}

void DataTransformModel::SetInputField(const boost::uuids::uuid& treeID, SynGlyphX::MinMaxGlyphTree::const_iterator& node, int index, const SynGlyphX::InputField& inputfield) {

	m_dataMapping->SetInputField(treeID, node, index, inputfield);
}

void DataTransformModel::ClearInputBinding(const boost::uuids::uuid& treeID, SynGlyphX::MinMaxGlyphTree::const_iterator& node, int index) {

	m_dataMapping->ClearInputBinding(treeID, node, index);
}

void DataTransformModel::EnableTables(const boost::uuids::uuid& id, const SynGlyphX::Datasource::TableSet& tables, bool enable) {

	m_dataMapping->EnableTables(id, tables, enable);
}

bool DataTransformModel::removeRows(int row, int count, const QModelIndex& parent) {

	if ((!parent.isValid()) && (count > 0)) {

		int lastRow = row + count - 1;
		beginRemoveRows(parent, row, lastRow);

		if (IsRowInDataType(DataType::GlyphTrees, row)) {

			SynGlyphX::DataTransformMapping::MinMaxGlyphTreeMap::const_iterator begin = m_dataMapping->GetGlyphTrees().begin();
			std::advance(begin, row);
			SynGlyphX::DataTransformMapping::MinMaxGlyphTreeMap::const_iterator end = begin;
			std::advance(end, count);

			//Store IDs to be removed so that we don't have to worry about removal affecting iterators on the tree map
			std::vector<boost::uuids::uuid> treeIDs;
			for (SynGlyphX::DataTransformMapping::MinMaxGlyphTreeMap::const_iterator iT = begin; iT != end; ++iT) {

				treeIDs.push_back(iT->first);
			}

			for (const boost::uuids::uuid& id : treeIDs) {

				m_dataMapping->RemoveGlyphTree(id);
			}
		}
		else if (IsRowInDataType(DataType::BaseObjects, row)) {


		}
		else if (IsRowInDataType(DataType::DataSources, row)) {


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

	SynGlyphX::MinMaxGlyphTree::SharedPtr glyphTree(new SynGlyphX::MinMaxGlyphTree());
	glyphTree->ReadFromFile(filename.toStdString());
	int row = m_dataMapping->GetGlyphTrees().size();
	beginInsertRows(QModelIndex(), row, row + 1);
	m_dataMapping->AddGlyphTree(glyphTree);
	endInsertRows();
}

void DataTransformModel::SetBaseImage(const SynGlyphX::BaseImage& baseImage) {

	m_dataMapping->SetBaseObject(0, baseImage);
}

bool DataTransformModel::IsRowInDataType(DataType type, int row) const {

	int min = 0;
	int max = 0;
	if (type == DataType::BaseObjects) {

		min = m_dataMapping->GetGlyphTrees().size();
		max = min + m_dataMapping->GetBaseObjects().size();
	}
	else if (type == DataType::DataSources) {

		min = m_dataMapping->GetGlyphTrees().size() + m_dataMapping->GetBaseObjects().size();
		max = min + m_dataMapping->GetDatasources().GetFileDatasources().size();
	}
	else if (type == DataType::GlyphTrees) {

		max = m_dataMapping->GetGlyphTrees().size();
	}

	return ((row >= min) && (row < max));
}