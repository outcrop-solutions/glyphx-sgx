#ifndef MINMAXGLYPHTREEMODEL_H
#define MINMAXGLYPHTREEMODEL_H

#include "antzgui_global.h"
#include <QtCore/QAbstractItemModel>
#include "datamappingglyphgraph.h"

namespace SynGlyphXANTz {

	class ANTZGUI_EXPORT MinMaxGlyphTreeModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		enum GlyphType {
			Min,
			Max
		};

		enum PropertyUpdate {
			UpdateNone = 0x00,
			UpdateColor = 0x01,
			UpdateGeometry = 0x02,
			UpdateTopology = 0x04,
			UpdateSurface = 0x08,
			UpdatePosition = 0x10,
			UpdateRotation = 0x20,
			UpdateScale = 0x40,
			UpdateRotationRate = 0x80,
			//All for when all properties are being updated
			UpdateAll = 0xFFFF
		};

		Q_DECLARE_FLAGS(PropertyUpdates, PropertyUpdate);

		MinMaxGlyphTreeModel(QObject *parent);
		~MinMaxGlyphTreeModel();

		void SetMinMaxGlyphTree(SynGlyphX::DataMappingGlyphGraph::SharedPtr minMaxGlyphTree);
		SynGlyphX::DataMappingGlyphGraph::ConstSharedPtr GetMinMaxGlyphTree() const;

		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
		virtual QModelIndex	parent(const QModelIndex& index) const;
		virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual Qt::ItemFlags flags(const QModelIndex& index) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		virtual Qt::DropActions supportedDropActions() const;
		virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

		void AppendChild(const QModelIndex& parent, const SynGlyphX::DataMappingGlyph& glyph, unsigned int numberOfChildren = 1);

		void UpdateGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyph& glyph, PropertyUpdates updates = UpdateAll);
		void UpdateGlyphs(const QModelIndexList& indexList, const SynGlyphX::DataMappingGlyph& glyph, PropertyUpdates updates = UpdateAll);

		void UpdateGlyph(const QModelIndex& index, GlyphType type, const SynGlyphX::Glyph& glyph, PropertyUpdates updates = UpdateAll);
		void UpdateGlyphs(const QModelIndexList& indexList, GlyphType type, const SynGlyphX::Glyph& glyph, PropertyUpdates updates = UpdateAll);

		SynGlyphX::DataMappingGlyphGraph::const_iterator GetMinMaxGlyph(const QModelIndex& index) const;

		virtual QStringList mimeTypes() const;
		virtual QMimeData* mimeData(const QModelIndexList& indexes) const;
		virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

		bool LoadFromFile(const QString& filename);
		void SaveToTemplateFile(const QString& filename) const;
		bool SaveToCSV(const QString& filename, bool writeMaxGlyph = true);

		bool IsClipboardEmpty() const;
		SynGlyphX::DataMappingGlyphGraph::iterator GetClipboardGlyph() const;
		void CopyToClipboard(const QModelIndex& index, bool removeFromTree = false);

		void RepaceModelWithDefaultGlyphTree();

		static bool GreaterBranchLevel(const QModelIndex& left, const QModelIndex& right);

	private:
		void UpdateGlyphMin(const QModelIndex& index, const SynGlyphX::Glyph& glyph, PropertyUpdates updates);
		void UpdateGlyphMax(const QModelIndex& index, const SynGlyphX::Glyph& glyph, PropertyUpdates updates);

		bool IsValidCSVFile(const QString& filename) const;
		bool IsRootGlyph(const SynGlyphX::DataMappingGlyphGraph::iterator& glyph) const;
		unsigned int GetBranchLevel(const QModelIndex& index) const;

		static SynGlyphX::DataMappingGlyphGraph::iterator GetIteratorFromIndex(const QModelIndex& index);

		SynGlyphX::DataMappingGlyphGraph::SharedPtr m_minMaxGlyphTree;
		SynGlyphX::DataMappingGlyphGraph::iterator m_clipboardGlyph;
	};

	Q_DECLARE_OPERATORS_FOR_FLAGS(MinMaxGlyphTreeModel::PropertyUpdates)

} //namespace SynGlyphXANTz

#endif // MINMAXGLYPHTREEMODEL_H
