#include "glyphforestmodel.h"
#include "npdata.h"
#include "data/npmapfile.h"
#include <QtCore/QDir>
#include <QtCore/QThread>
#include "SOIL.h"
#include "io/gl/nptags.h"

GlyphForestModel::GlyphForestModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	m_antzData = static_cast<pData>(npInitData(0, NULL));
	m_defaultBaseImage = QDir::currentPath() + QDir::separator() + "usr" + QDir::separator() + "images" + QDir::separator() + "map00001.jpg";
	m_textures[m_defaultBaseImage.toStdWString()] = 1;
}

GlyphForestModel::~GlyphForestModel()
{

}

int GlyphForestModel::columnCount(const QModelIndex& parent) const {

	return 1;
}

QVariant GlyphForestModel::data(const QModelIndex& index, int role) const {

	if (!index.isValid()) {
		return QVariant();
	}

	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	pNPnode glyph = static_cast<pNPnode>(index.internalPointer());

	QString displayedData = glyph->tag->title;

	return displayedData;
}

QModelIndex	GlyphForestModel::index(int row, int column, const QModelIndex& parent) const {

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	pNPnode parentGlyph;
	if (!parent.isValid()) {
		return createIndex(row, column, m_antzData->map.node[row + kNPnodeRootPin]);
	}
	else {
		parentGlyph = static_cast<pNPnode>(parent.internalPointer());
	}

	if (row < parentGlyph->childCount) {
		return createIndex(row, column, parentGlyph->child[row]);
	}
	else {
		return QModelIndex();
	}
}

QModelIndex	GlyphForestModel::parent(const QModelIndex& index) const {

	if (!index.isValid()) {
		return QModelIndex();
	}

	pNPnode glyph = static_cast<pNPnode>(index.internalPointer());

	if (glyph->branchLevel == 0) {
		return QModelIndex();
	}
	else {
		pNPnode parent = glyph->parent;
		if (parent->branchLevel == 0) {
			
			return createIndex(FindRowForRootNode(parent), 0, parent);
		}
		else {
			return createIndex(GetChildIndexFromParent(parent), 0, parent);
		}
	}
}

int	GlyphForestModel::rowCount(const QModelIndex& parent) const {

	pNPnode glyph;
	if (!parent.isValid()) {
		
		return m_antzData->map.nodeRootCount - kNPnodeRootPin;
	}
	else {
		glyph = static_cast<pNPnode>(parent.internalPointer());
	}
	return glyph->childCount;
}

Qt::ItemFlags GlyphForestModel::flags(const QModelIndex& index) const {

	if (!index.isValid()) {
		return 0;
	}

	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	return flags;
}

QVariant GlyphForestModel::headerData(int section, Qt::Orientation orientation, int role) const {

	//We don't need column headers in views so this gets rid of them
	return "";
}

pData GlyphForestModel::GetANTzData() const {

	return m_antzData;
}

int GlyphForestModel::GetChildIndexFromParent(pNPnode node) const {

	pNPnode parent = node->parent;
	int i = 0;
	for (; i < parent->childCount; ++i) {
		if (parent->child[i] == node) {
			break;
		}
	}

	return i;
}

void GlyphForestModel::LoadANTzFiles(const QStringList& filenames) {

	m_antzData->map.nodeRootIndex = kNPnodeRootPin;

	beginResetModel();
	while (m_antzData->map.nodeRootCount > kNPnodeRootPin) {
	
		npNodeDelete(static_cast<pNPnode>(m_antzData->map.node[kNPnodeRootPin]), m_antzData);
	}
	for (const QString& filename : filenames) {

		npFileOpenCore(filename.toStdString().c_str(), NULL, m_antzData);
	}
	npSyncTags(static_cast<void*>(m_antzData));
	endResetModel();

	m_antzData->map.nodeRootIndex = 0;
}

QModelIndex GlyphForestModel::IndexFromANTzID(int id) const {

	pNPnode node = static_cast<pNPnode>(m_antzData->map.nodeID[id]);

	if (node->branchLevel == 0) {
		return createIndex(FindRowForRootNode(node), 0, node);
	}
	else {
		return createIndex(GetChildIndexFromParent(node), 0, node);
	}
}

int GlyphForestModel::FindRowForRootNode(pNPnode node) const {

	int i = kNPnodeRootPin;
	for (; i < m_antzData->map.nodeRootCount; ++i) {

		if (m_antzData->map.node[i] == node) {
			break;
		}
	}

	return i - kNPnodeRootPin;
}

void GlyphForestModel::UseDefaultBaseImage() {

	pNPnode grid = static_cast<pNPnode>(m_antzData->map.node[kNPnodeRootGrid]);
	grid->textureID = m_textures.begin()->second;
}

void GlyphForestModel::UseLocalBaseImage(const QString& filename) {

	pNPnode grid = static_cast<pNPnode>(m_antzData->map.node[kNPnodeRootGrid]);
	std::unordered_map<std::wstring, int>::iterator iT = m_textures.find(filename.toStdWString());
	if (iT == m_textures.end()) {

		int textureID = SOIL_load_OGL_texture(filename.toStdString().c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		if (textureID == 0) {

			throw std::exception("Failed to load base image");
		}

		m_textures[filename.toStdWString()] = textureID;
		grid->textureID = textureID;
		m_antzData->io.gl.textureCount = textureID;
	}
	else {

		grid->textureID = iT->second;
	}
	
}