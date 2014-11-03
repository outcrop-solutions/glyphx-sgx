#include "datatransformmodel.h"
#include "sourcedatamanager.h"
#include <QtCore/QDir>
#include <boost/uuid/uuid_io.hpp>
#include "application.h"

DataTransformModel::DataTransformModel(QObject *parent)
	: QAbstractItemModel(parent),
	m_dataTransform(new SynGlyphX::DataTransformMapping())
{
	SetIntermediateDirectoryToCurrentID();
}

DataTransformModel::~DataTransformModel()
{

}

void DataTransformModel::SetIntermediateDirectoryToCurrentID() {

	SynGlyphX::SourceDataManager::SetIntermediateDirectory(GetIntermediateDirectoryForID(m_dataTransform->GetID()));
}

QString DataTransformModel::GetIntermediateDirectoryForID(const boost::uuids::uuid& id) {

	return QDir::toNativeSeparators(QDir::tempPath() + QDir::separator() + SynGlyphX::Application::applicationName() + QDir::separator() + "int_" + QString::fromStdString(boost::uuids::to_string(id)));
}

int DataTransformModel::columnCount(const QModelIndex& parent) const {

	return 1;
}

QVariant DataTransformModel::data(const QModelIndex& index, int role) const {

	if (m_dataTransform->GetGlyphTrees().empty()) {
		return QVariant();
	}

	if (!index.isValid()) {
		return QVariant();
	}

	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	SynGlyphX::MinMaxGlyphTree::const_iterator iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(index.internalPointer()));
	const SynGlyphX::GlyphProperties& minGlyph = iterator->GetMinGlyph();

	std::wstring displayedData = SynGlyphX::GlyphProperties::s_shapeNames.left.at(minGlyph.GetShape()) + L": " + SynGlyphX::GlyphProperties::s_topologyNames.left.at(minGlyph.GetTopology());
	if (iterator == iterator.owner()->root().constify()) {
		displayedData += L" (Root)";
	}

	return QString::fromStdWString(displayedData);
}

QModelIndex	DataTransformModel::index(int row, int column, const QModelIndex& parent) const{

	if ((!hasIndex(row, column, parent)) || (m_dataTransform->GetGlyphTrees().empty())) {
		return QModelIndex();
	}

	if (!parent.isValid()) {

		SynGlyphX::DataTransformMapping::MinMaxGlyphTreeMap::const_iterator iterator = m_dataTransform->GetGlyphTrees().begin();
		std::advance(iterator, row);
		return createIndex(row, column, static_cast<void*>(iterator->second->root().node()));
	}
	else {

		SynGlyphX::MinMaxGlyphTree::const_iterator parentIterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(parent.internalPointer()));
		SynGlyphX::MinMaxGlyphTree::const_iterator childIterator = parentIterator.owner()->child(parentIterator, row);
		if (childIterator.valid()) {

			return createIndex(row, column, static_cast<void*>(childIterator.node()));
		}
		else {

			return QModelIndex();
		}
	}
}

QModelIndex	DataTransformModel::parent(const QModelIndex& index) const {

	if (m_dataTransform->GetGlyphTrees().empty() || !index.isValid()) {
		return QModelIndex();
	}

	SynGlyphX::MinMaxGlyphTree::const_iterator iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(index.internalPointer()));

	if (iterator == iterator.owner()->root()) {
		return QModelIndex();
	}

	const SynGlyphX::MinMaxGlyphTree* currentTree = static_cast<const SynGlyphX::MinMaxGlyphTree*>(iterator.owner());
	SynGlyphX::MinMaxGlyphTree::const_iterator parent = currentTree->parent(iterator);

	int row = 0;

	if (parent == currentTree->root()) {

		for (auto tree : m_dataTransform->GetGlyphTrees()) {

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

int	DataTransformModel::rowCount(const QModelIndex& parent) const {

	if (m_dataTransform->GetGlyphTrees().empty()) {
		return 0;
	}

	if (!parent.isValid()) {
		return m_dataTransform->GetGlyphTrees().size();
	}

	SynGlyphX::MinMaxGlyphTree::const_iterator iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(parent.internalPointer()));
	return iterator.owner()->children(iterator);
}

void DataTransformModel::AddGlyphFile(const QString& filename) {

	SynGlyphX::MinMaxGlyphTree::SharedPtr glyphTree(new SynGlyphX::MinMaxGlyphTree());
	glyphTree->ReadFromFile(filename.toStdString());
	int row = m_dataTransform->GetGlyphTrees().size();
	beginInsertRows(QModelIndex(), row, row + 1);
	m_dataTransform->AddGlyphTree(glyphTree);
	endInsertRows();
}

void DataTransformModel::LoadDataTransformFile(const QString& filename) {

	Clear();
	beginResetModel();
	m_dataTransform->ReadFromFile(filename.toStdString());
	SetIntermediateDirectoryToCurrentID();
	m_sourceDataManager.AddDatabaseConnections(m_dataTransform->GetDatasources());
	endResetModel();
}

void DataTransformModel::SaveDataTransformFile(const QString& filename) {

	m_dataTransform->WriteToFile(filename.toStdString());
}

void DataTransformModel::Clear() {

	beginResetModel();
	m_sourceDataManager.ClearDatabaseConnections();
	m_dataTransform->Clear();
	endResetModel();
	SetIntermediateDirectoryToCurrentID();
}

void DataTransformModel::SetBaseImage(const SynGlyphX::BaseImage& baseImage) {

	m_dataTransform->SetBaseImage(baseImage);
}

boost::uuids::uuid DataTransformModel::AddFileDatasource(SynGlyphX::FileDatasource::SourceType type, const std::wstring& name) {

	boost::uuids::uuid id = m_dataTransform->AddFileDatasource(type, name);
	m_sourceDataManager.AddDatabaseConnection(m_dataTransform->GetDatasources().GetFileDatasources().at(id), id);

	return id;
}

void DataTransformModel::SetInputField(const boost::uuids::uuid& treeID, SynGlyphX::MinMaxGlyphTree::const_iterator& node, int index, const SynGlyphX::InputField& inputfield) {

	m_dataTransform->SetInputField(treeID, node, index, inputfield);
}

void DataTransformModel::ClearInputBinding(const boost::uuids::uuid& treeID, SynGlyphX::MinMaxGlyphTree::const_iterator& node, int index) {

	m_dataTransform->ClearInputBinding(treeID, node, index);
}

void DataTransformModel::EnableTables(const boost::uuids::uuid& id, const SynGlyphX::Datasource::TableSet& tables, bool enable) {

	m_dataTransform->EnableTables(id, tables, enable);
}

SynGlyphX::DataTransformMapping::ConstSharedPtr DataTransformModel::GetDataTransform() const {

	return m_dataTransform;
}

bool DataTransformModel::removeRows(int row, int count, const QModelIndex& parent) {

	if ((!parent.isValid()) && (count > 0)) {

		int lastRow = row + count - 1;
		SynGlyphX::DataTransformMapping::MinMaxGlyphTreeMap::const_iterator begin = m_dataTransform->GetGlyphTrees().begin();
		std::advance(begin, row);
		SynGlyphX::DataTransformMapping::MinMaxGlyphTreeMap::const_iterator end = begin;
		std::advance(end, count);
		
		//Store IDs to be removed so that we don't have to worry about removal affecting iterators on the tree map
		std::vector<boost::uuids::uuid> treeIDs;
		for (SynGlyphX::DataTransformMapping::MinMaxGlyphTreeMap::const_iterator iT = begin; iT != end; ++iT) {

			treeIDs.push_back(iT->first);
		}

		beginRemoveRows(parent, row, lastRow);
		for (const boost::uuids::uuid& id : treeIDs) {

			m_dataTransform->RemoveGlyphTree(id);
		}
		endRemoveRows();
		
		return true;
	}

	return false;
}

void DataTransformModel::SetDefaults(const SynGlyphX::DataMappingDefaults& defaults) {

	m_dataTransform->SetDefaults(defaults);
}