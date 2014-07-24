#include "datatransformmodel.h"

DataTransformModel::DataTransformModel(QObject *parent)
	: QAbstractItemModel(parent),
	m_dataTransform(new SynGlyphX::DataTransform())
{

}

DataTransformModel::~DataTransformModel()
{

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
	if (iterator == m_dataTransform->GetGlyphTrees().begin()->second->root().constify()) {
		displayedData += L" (Root)";
	}

	return QString::fromStdWString(displayedData);
}

QModelIndex	DataTransformModel::index(int row, int column, const QModelIndex& parent) const{

	if (!hasIndex(row, column, parent))  {
		return QModelIndex();
	}

	if (parent.internalPointer() == &(m_dataTransform->GetDatasources())) {

		SynGlyphX::DataTransform::DatasourceMap::const_iterator iterator = m_dataTransform->GetDatasources().begin();
		std::advance(iterator, row);
		return createIndex(row, column, static_cast<const void*>(&(iterator->second)));
	}

	if (!parent.isValid()) {

		return createIndex(row, column, static_cast<void*>(m_dataTransform->GetGlyphTrees().begin()->second->root().node()));
	}
	else {

		SynGlyphX::MinMaxGlyphTree::const_iterator parent(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(parent.internalPointer()));
		SynGlyphX::MinMaxGlyphTree::const_iterator child = m_dataTransform->GetGlyphTrees().begin()->second->child(parent, row);
		if (child.valid()) {

			return createIndex(row, column, static_cast<void*>(child.node()));
		}
		else {

			return QModelIndex();
		}
	}
}

QModelIndex	DataTransformModel::parent(const QModelIndex& index) const {

	if (!index.isValid()) {
		return QModelIndex();
	}

	if (m_dataTransform->GetGlyphTrees().empty()) {
		return QModelIndex();
	}

	SynGlyphX::MinMaxGlyphTree::const_iterator iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(index.internalPointer()));
	SynGlyphX::MinMaxGlyphTree::const_iterator parent = m_dataTransform->GetGlyphTrees().begin()->second->parent(iterator);

	if (!parent.valid()) {
		return QModelIndex();
	}

	int row = 0;
	SynGlyphX::MinMaxGlyphTree::const_iterator grandparent = m_dataTransform->GetGlyphTrees().begin()->second->parent(parent);
	if (grandparent.valid()) {

		for (int i = 0; i < m_dataTransform->GetGlyphTrees().begin()->second->children(grandparent); ++i) {

			if (m_dataTransform->GetGlyphTrees().begin()->second->child(grandparent, i) == parent) {
				row = i;
				break;
			}
		}
	}

	return createIndex(row, 0, static_cast<void*>(parent.node()));
}

int	DataTransformModel::rowCount(const QModelIndex& parent) const {

	if (!parent.isValid()) {

		//Return children of root.  Right now 2 for Datasources and Glyphs
		return 2;
	}

	if (parent.internalPointer() == &(m_dataTransform->GetDatasources())) {

		return m_dataTransform->GetDatasources().size();
	}

	if (parent.internalPointer() == &(m_dataTransform->GetGlyphTrees()))  {

		return m_dataTransform->GetGlyphTrees().size();
	}

	

	

	SynGlyphX::MinMaxGlyphTree::const_iterator iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(parent.internalPointer()));
	return m_dataTransform->GetGlyphTrees().begin()->second->children(iterator);
}

void DataTransformModel::AddGlyphFile(const QString& filename) {

	SynGlyphX::MinMaxGlyphTree::SharedPtr glyphTree(new SynGlyphX::MinMaxGlyphTree());
	glyphTree->ReadFromFile(filename.toStdString());
	beginResetModel();
	m_dataTransform->AddGlyphTree(glyphTree);
	endResetModel();
}

void DataTransformModel::LoadDataTransformFile(const QString& filename) {

	beginResetModel();
	m_dataTransform->ReadFromFile(filename.toStdString());
	endResetModel();
}

SynGlyphX::DataTransform::SharedPtr DataTransformModel::GetDataTransform() const {

	return m_dataTransform;
}