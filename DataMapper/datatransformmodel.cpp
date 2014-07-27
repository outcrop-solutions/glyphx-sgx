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

		SynGlyphX::DataTransform::MinMaxGlyphTreeMap::const_iterator iterator = m_dataTransform->GetGlyphTrees().begin();
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

	SynGlyphX::MinMaxGlyphTree::const_iterator parent = iterator.owner()->parent(iterator);

	int row = 0;
	SynGlyphX::MinMaxGlyphTree::const_iterator grandparent = iterator.owner()->parent(parent);
	if (grandparent.valid()) {

		for (int i = 0; i < iterator.owner()->children(grandparent); ++i) {

			if (iterator.owner()->child(grandparent, i) == parent) {
				row = i;
				break;
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

	beginResetModel();
	m_dataTransform->ReadFromFile(filename.toStdString());
	endResetModel();
}

SynGlyphX::DataTransform::SharedPtr DataTransformModel::GetDataTransform() const {

	return m_dataTransform;
}