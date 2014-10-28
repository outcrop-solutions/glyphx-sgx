#ifndef GLYPHTREEMODEL_H
#define GLYPHTREEMODEL_H

#include "antzgui_global.h"
#include <QtCore/QAbstractItemModel>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "data/nptypes.h"
#include "glyph.h"

class ANTZGUI_EXPORT GlyphTreeModel : public QAbstractItemModel
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

    GlyphTreeModel(QObject *parent);
    ~GlyphTreeModel();

    //Functions from QAbstractItemModel that need to be implemented
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex	parent(const QModelIndex& index) const;
    virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
    //virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
    //virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
	//virtual bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild);

    virtual QStringList mimeTypes() const;
    virtual QMimeData* mimeData(const QModelIndexList& indexes) const;
    virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

    pData GetANTzData() const;

    pNPnode GetRootGlyph() const;

    bool LoadFromFile(const QString& filename);
	void SaveToTemplateFile(const QString& filename) const;
    bool SaveToCSV(const std::string& filename, const QModelIndexList& selectedItems);
	void CreateNewTree(SynGlyphX::GlyphTree::ConstSharedPtr newGlyphTree, bool usePositionsInGlyphTree = false);
    void UpdateNode(const QModelIndex& index, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph, PropertyUpdates updates = UpdateAll);
    void UpdateNodes(const QModelIndexList& indexList, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph, PropertyUpdates updates = UpdateAll);

    QModelIndex IndexFromANTzID(int id);

    void AppendChild(const QModelIndex& parent, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph, unsigned int numberOfChildren = 1);

    bool IsClipboardEmpty() const;
    SynGlyphX::GlyphProperties::ConstSharedPtr GetClipboardGlyph() const;
    void CopyToClipboard(const QModelIndex& index, bool removeFromTree = false);

    static PropertyUpdates FindUpdates(boost::shared_ptr<const SynGlyphX::GlyphProperties> oldGlyph, boost::shared_ptr<const SynGlyphX::GlyphProperties> newGlyph);
    static bool GreaterBranchLevel(const QModelIndex& left, const QModelIndex& right);

	void CreateDefaultGlyph();
	void ShowGlyph(bool show);

signals:
    void NodeUpdated(const QModelIndex& index);
    void ModelChanged(bool isDifferentFromSavedFileOrDefaultGlyph);

private slots:
	void NotifyModelUpdate();
	void MarkDifferentNotifyModelUpdate();

private:
	void CreateDefaultGlyph(bool resetModel);
	pNPnode CreateNodeFromTemplate(pNPnode parent, const SynGlyphX::GlyphProperties& glyphTemplate, bool updatePosition = false);
    int GetChildIndexFromParent(pNPnode node) const;
	void UpdateNode(pNPnode glyph, const SynGlyphX::GlyphProperties& glyphTemplate, PropertyUpdates updates = UpdateAll);
	void CreateNewSubTree(pNPnode parent, SynGlyphX::GlyphTree::ConstSharedPtr newGlyphTree, const SynGlyphX::GlyphTree::const_iterator& location, bool updatePosition = false);
	bool IsANTzCSVFile(const QString& filename) const;
	void DeleteGlyphRootNode();

    pNPnode m_rootGlyph;
    pData m_antzData;
	SynGlyphX::GlyphProperties::SharedPtr m_clipboardGlyph;
	bool m_isDifferentFromSavedFileOrDefaultGlyph;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GlyphTreeModel::PropertyUpdates)

#endif // GLYPHTREEMODEL_H
