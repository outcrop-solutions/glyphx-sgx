#include "glyphforestmodel.h"
#include "npdata.h"
#include "data/npmapfile.h"
#include <QtCore/QDir>
#include <QtCore/QRect>
#include <QtCore/QThread>
#include "SOIL.h"
#include "io/gl/nptags.h"
#include "io/npfile.h"
#include "io/npch.h"
#include "npctrl.h"
#include <QtCore/QProcess>

namespace SynGlyphXANTz {

	GlyphForestModel::GlyphForestModel(QObject *parent)
		: QAbstractItemModel(parent),
		m_antzData(new ANTzPlus::ANTzData()),
		m_tagNotToBeShownIn3d()
	{

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
		//return displayedData + QString(" - (%1, %2, %3)").arg(glyph->world.x).arg(glyph->world.y).arg(glyph->world.z);
		//return displayedData + QString(" - %1").arg(glyph->id);
	}

	QModelIndex	GlyphForestModel::index(int row, int column, const QModelIndex& parent) const {

		if (!hasIndex(row, column, parent)) {
			return QModelIndex();
		}

		pNPnode parentGlyph;
		if (!parent.isValid()) {

			return createIndex(row, column, m_antzData->GetData()->map.node[row + kNPnodeRootPin]);
		}
		else {

			parentGlyph = static_cast<pNPnode>(parent.internalPointer());
		}

		std::set<unsigned int> nonLinkChildIndexes = GetChildIndexesThatAreNotLinks(parentGlyph);
		if (row < nonLinkChildIndexes.size()) {

			std::set<unsigned int>::const_iterator iT = nonLinkChildIndexes.begin();
			std::advance(iT, row);
			return createIndex(row, column, parentGlyph->child[*iT]);
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

	std::set<unsigned int> GlyphForestModel::GetChildIndexesThatAreNotLinks(pNPnode node) const {

		std::set<unsigned int> linkChildIndexes;

		for (int i = 0; i < node->childCount; ++i) {

			if (node->child[i]->type == 5) {

				linkChildIndexes.insert(i);
			}
		}

		return linkChildIndexes;
	}

	int	GlyphForestModel::rowCount(const QModelIndex& parent) const {

		pNPnode glyph;
		if (!parent.isValid()) {

			return m_antzData->GetData()->map.nodeRootCount - kNPnodeRootPin;
		}
		
		glyph = static_cast<pNPnode>(parent.internalPointer());
		
		std::set<unsigned int> nonLinkChildIndexes = GetChildIndexesThatAreNotLinks(glyph);

		return nonLinkChildIndexes.size();
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

	ANTzPlus::ANTzData::SharedPtr GlyphForestModel::GetANTzData() const {

		return m_antzData;
	}

	int GlyphForestModel::GetChildIndexFromParent(pNPnode node) const {

		pNPnode parent = node->parent;
		int i = 0;
		int linkCount = 0;
		for (; i < parent->childCount; ++i) {
			
			if (parent->child[i]->type != 5) {

				++linkCount;
			}

			if (parent->child[i] == node) {
				
				break;
			}
		}

		return i - linkCount;
	}

	void GlyphForestModel::Clear() {

		Clear(true);
	}

	void GlyphForestModel::Clear(bool resetModel) {

		pData antzData = m_antzData->GetData();
		antzData->map.nodeRootIndex = kNPnodeRootPin;
		pNPnode rootGrid = static_cast<pNPnode>(m_antzData->GetData()->map.node[kNPnodeRootGrid]);

		if (resetModel) {

			beginResetModel();
		}

		while (antzData->map.nodeRootCount > kNPnodeRootPin) {

			antzData->map.nodeRootIndex = antzData->map.nodeRootCount - 1;
			npNodeDelete(static_cast<pNPnode>(antzData->map.node[antzData->map.nodeRootCount - 1]), m_antzData->GetData());
			//npNodeDelete(static_cast<pNPnode>(antzData->map.node[kNPnodeRootPin]), m_antzData->GetData());
		}

		while (rootGrid->childCount > 0) {

			pNPnode grid = rootGrid->child[rootGrid->childCount - 1];
			npNodeDelete(grid, m_antzData->GetData());
		}
		rootGrid->textureID = 1;

		//This will clear out tags if needed
		npDeleteAllTags(antzData);

		antzData->map.nodeRootIndex = 0;

		m_baseImageFilenames.clear();
		m_urlRedirectFilename.clear();

		if (resetModel) {

			endResetModel();
		}
	}

	void GlyphForestModel::LoadANTzVisualization(const SynGlyphXANTz::ANTzCSVWriter::FilenameList& filesToLoad, const QStringList& baseImageFilenames) {

		pData antzData = m_antzData->GetData();
		antzData->map.nodeRootIndex = kNPnodeRootPin;

		beginResetModel();

		Clear(false);
		m_baseImageFilenames = baseImageFilenames;

		npFileOpenCore(filesToLoad[SynGlyphXANTz::ANTzCSVWriter::s_nodeFilenameIndex].c_str(), NULL, antzData);
		npFileOpenCore(filesToLoad[SynGlyphXANTz::ANTzCSVWriter::s_tagFilenameIndex].c_str(), NULL, antzData);

		m_urlRedirectFilename = QString::fromStdString(filesToLoad[SynGlyphXANTz::ANTzCSVWriter::s_redirectFilenameIndex]);
		
		npSyncTags(static_cast<void*>(antzData));
		endResetModel();

		antzData->map.nodeRootIndex = 0;
	}

	QModelIndex GlyphForestModel::IndexFromANTzID(int id) const {

		pNPnode node = static_cast<pNPnode>(m_antzData->GetData()->map.nodeID[id]);

		if (node->branchLevel == 0) {
			return createIndex(FindRowForRootNode(node), 0, node);
		}
		else {
			return createIndex(GetChildIndexFromParent(node), 0, node);
		}
	}

	int GlyphForestModel::FindRowForRootNode(pNPnode node) const {

		int i = kNPnodeRootPin;
		for (; i < m_antzData->GetData()->map.nodeRootCount; ++i) {

			if (m_antzData->GetData()->map.node[i] == node) {
				break;
			}
		}

		return i - kNPnodeRootPin;
	}
	/*
	void GlyphForestModel::SetParentGridToDefaultBaseImage() {

		pNPnode grid = static_cast<pNPnode>(m_antzData->GetData()->map.node[kNPnodeRootGrid]);
		grid->textureID = 1; // m_textures.begin()->second;
	}*/

	const QStringList& GlyphForestModel::GetBaseImageFilenames() const {

		return m_baseImageFilenames;
	}

	void GlyphForestModel::FindIndexesInRegion(const QRect& region, QItemSelection& itemSelection) const {

		QModelIndexList indexesInRegion;

		for (int i = kNPnodeRootPin; i < m_antzData->GetData()->map.nodeRootCount; ++i) {

			FindNodesInRegion(region, static_cast<pNPnode>(m_antzData->GetData()->map.node[i]), i - kNPnodeRootPin, itemSelection);
		}
	}

	void GlyphForestModel::FindNodesInRegion(const QRect& region, pNPnode node, int row, QItemSelection& itemSelection) const {

		if (node->hide) {

			return;
		}

		if ((node->screen.z >= 0.0f) && (node->screen.z <= 1.0f) && (region.contains(node->screen.x, node->screen.y, false))) {

			QModelIndex indexInRegion = createIndex(row, 0, node);
			itemSelection.select(indexInRegion, indexInRegion);
		}

		for (int i = 0; i < node->childCount; ++i) {

			FindNodesInRegion(region, node->child[i], i, itemSelection);
		}
	}

	void GlyphForestModel::SetTagNotToBeShownIn3d(const QString& tag) {

		m_tagNotToBeShownIn3d = tag;
	}

	bool GlyphForestModel::IsTagShownIn3d(const QString& tag) {

		return (tag != m_tagNotToBeShownIn3d);
	}

	void GlyphForestModel::OpenURLs(const QModelIndexList& indexList) {

		for (const QModelIndex& index : indexList) {

			pNPnode glyph = static_cast<pNPnode>(index.internalPointer());
			QProcess process;
			process.setProgram(m_urlRedirectFilename);
			process.setArguments(QStringList() << QString::number(glyph->recordID));
			process.start();
			process.waitForFinished();
		}
	}

} //namespace SynGlyphXANTz