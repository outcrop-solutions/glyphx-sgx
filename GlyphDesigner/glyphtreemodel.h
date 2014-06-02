#ifndef GLYPHTREEMODEL_H
#define GLYPHTREEMODEL_H

#include <QtCore/QAbstractItemModel>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "data/nptypes.h"
#include "glyph.h"

class GlyphTreeModel : public QAbstractItemModel
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

    pData GetANTzData() const;

    pNPnode GetRootGlyph() const;

    bool LoadFromFile(const std::string& filename);
    bool SaveToCSV(const std::string& filename, const QModelIndexList& selectedItems) const;
    void CreateNewTree(const std::vector<boost::shared_ptr<SynGlyphX::Glyph>>& newGlyphTemplates);
    void UpdateNode(const QModelIndex& index, boost::shared_ptr<const SynGlyphX::Glyph> glyph, PropertyUpdates updates = UpdateAll);
    void UpdateNodes(const QModelIndexList& indexList, boost::shared_ptr<const SynGlyphX::Glyph> glyph, PropertyUpdates updates = UpdateAll);

    QModelIndex IndexFromANTzID(int id);

    void AppendChild(const QModelIndex& parent, boost::shared_ptr<const SynGlyphX::Glyph> glyph, unsigned int numberOfChildren = 1);
    void DeleteNode(const QModelIndex& index);
    void DeleteChildren(const QModelIndex& parent);

    bool IsClipboardEmpty() const;
    boost::shared_ptr<const SynGlyphX::Glyph> GetClipboardGlyph() const;
    void CopyToClipboard(const QModelIndex& index, bool removeFromTree = false);

    static PropertyUpdates FindUpdates(boost::shared_ptr<const SynGlyphX::Glyph> oldGlyph, boost::shared_ptr<const SynGlyphX::Glyph> newGlyph);

signals:
    void NodeUpdated(const QModelIndex& index);

private:
    pNPnode CreateNodeFromTemplate(pNPnode parent, boost::shared_ptr<const SynGlyphX::Glyph> glyphTemplate);
    void CreateRootPinNode();
    int GetChildIndexFromParent(pNPnode node) const;
    void UpdateNode(pNPnode glyph, boost::shared_ptr<const SynGlyphX::Glyph> glyphTemplate, PropertyUpdates updates = UpdateAll);

    pNPnode m_rootGlyph;
    pData m_antzData;
    boost::shared_ptr<SynGlyphX::Glyph> m_clipboardGlyph;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GlyphTreeModel::PropertyUpdates)

#endif // GLYPHTREEMODEL_H
