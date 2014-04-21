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

    void* GetANTzData() const;

    pNPnode GetRootGlyph() const;

    bool SaveToCSV(const std::string& filename) const;
    void CreateFromTemplates(boost::shared_ptr<const SynGlyphX::Glyph> newGlyphTemplates);

private:
    void CreateNodeFromTemplate(pNPnode parent, boost::shared_ptr<const SynGlyphX::Glyph> glyphTemplate);

    pNPnode m_rootGlyph;
    void* m_antzData;
};

#endif // GLYPHTREEMODEL_H
