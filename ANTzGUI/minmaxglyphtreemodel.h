#ifndef MINMAXGLYPHTREEMODEL_H
#define MINMAXGLYPHTREEMODEL_H

#include <QtCore/QAbstractItemModel>
#include "minmaxglyphtree.h"

class MinMaxGlyphTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	enum PropertyUpdate {
		UpdateNone = 0x00,
		UpdateColor = 0x01,
		UpdateGeometry = 0x02,
		UpdateTopology = 0x04,
		UpdateSurface = 0x08,
		UpdatePosition = 0x10,
		UpdateRotation = 0x20,
		UpdateScale = 0x40,
		//All for when all properties are being updated
		UpdateAll = 0xFFFF
	};

	Q_DECLARE_FLAGS(PropertyUpdates, PropertyUpdate);

	MinMaxGlyphTreeModel(QObject *parent);
	~MinMaxGlyphTreeModel();

	void SetMinMaxGlyphTree(SynGlyphX::MinMaxGlyphTree::SharedPtr minMaxGlyphTree);
	SynGlyphX::MinMaxGlyphTree::ConstSharedPtr GetMinMaxGlyphTree() const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex	parent(const QModelIndex& index) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual Qt::DropActions supportedDropActions() const;
	virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

	void AppendChild(const QModelIndex& parent, const SynGlyphX::MinMaxGlyph& glyph, unsigned int numberOfChildren = 1);
	void UpdateGlyph(const QModelIndex& index, const SynGlyphX::MinMaxGlyph& glyph, PropertyUpdates updates = UpdateAll);
	void UpdateGlyphs(const QModelIndexList& indexList, const SynGlyphX::MinMaxGlyph& glyph, PropertyUpdates updates = UpdateAll);

	virtual QStringList mimeTypes() const;
	virtual QMimeData* mimeData(const QModelIndexList& indexes) const;
	virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

	bool LoadFromFile(const QString& filename);
	void SaveToTemplateFile(const QString& filename) const;
	bool SaveToCSV(const QString& filename, bool writeMaxGlyph = true);

	bool IsClipboardEmpty() const;
	SynGlyphX::MinMaxGlyphTree::iterator GetClipboardGlyph() const;
	void CopyToClipboard(const QModelIndex& index, bool removeFromTree = false);

	void RepaceModelWithDefaultGlyphTree();

	static bool GreaterBranchLevel(const QModelIndex& left, const QModelIndex& right);

private:
	bool IsValidCSVFile(const QString& filename) const;
	bool IsRootGlyph(const SynGlyphX::MinMaxGlyphTree::iterator& glyph) const;
	unsigned int GetBranchLevel(const QModelIndex& index) const;

	static SynGlyphX::MinMaxGlyphTree::iterator GetIteratorFromIndex(const QModelIndex& index);

	SynGlyphX::MinMaxGlyphTree::SharedPtr m_minMaxGlyphTree;
	SynGlyphX::MinMaxGlyphTree::iterator m_clipboardGlyph;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MinMaxGlyphTreeModel::PropertyUpdates)

#endif // MINMAXGLYPHTREEMODEL_H
