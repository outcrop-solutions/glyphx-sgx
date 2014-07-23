#include "glyphtemplatesmodel.h"

GlyphTemplatesModel::GlyphTemplatesModel(SynGlyphX::DataTransform::SharedPtr dataTransform, QObject *parent)
	: QAbstractItemModel(parent),
	m_dataTransform(dataTransform)
{

}

GlyphTemplatesModel::~GlyphTemplatesModel()
{

}

int GlyphTemplatesModel::columnCount(const QModelIndex& parent) const {

	return 1;
}

QVariant GlyphTemplatesModel::data(const QModelIndex& index, int role) const {

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

QModelIndex	GlyphTemplatesModel::index(int row, int column, const QModelIndex& parent) const{

	if ((!hasIndex(row, column, parent)) || (m_dataTransform->GetGlyphTrees().empty())) {
		return QModelIndex();
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

QModelIndex	GlyphTemplatesModel::parent(const QModelIndex& index) const {

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

int	GlyphTemplatesModel::rowCount(const QModelIndex& parent) const {

	if (m_dataTransform->GetGlyphTrees().empty()) {
		return 0;
	}

	if (!parent.isValid()) {
		return 1;
	}

	SynGlyphX::MinMaxGlyphTree::const_iterator iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(parent.internalPointer()));
	return m_dataTransform->GetGlyphTrees().begin()->second->children(iterator);
}

QVariant GlyphTemplatesModel::headerData(int section, Qt::Orientation orientation, int role) const {

	//We don't need column headers in views so this gets rid of them
	return "";
}

void GlyphTemplatesModel::AddGlyphFile(const QString& filename) {

	SynGlyphX::MinMaxGlyphTree::SharedPtr glyphTree(new SynGlyphX::MinMaxGlyphTree());
	glyphTree->ReadFromFile(filename.toStdString());
	beginResetModel();
	m_dataTransform->AddGlyphTree(glyphTree);
	endResetModel();
}