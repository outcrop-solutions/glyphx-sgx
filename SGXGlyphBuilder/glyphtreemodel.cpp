#include "glyphtreemodel.h"
#include "npdata.h"

GlyphTreeModel::GlyphTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_antzData = npInitData(0, NULL);

    m_rootGlyph = npNodeNew(kNodePin, NULL, m_antzData);
    npNodeNew(kNodePin, m_rootGlyph, m_antzData);
}

GlyphTreeModel::~GlyphTreeModel()
{
    npCloseData();
}

int GlyphTreeModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QVariant GlyphTreeModel::data(const QModelIndex& index, int role) const {

    if (!index.isValid()) {
        return QVariant();
    }
    return QVariant::fromValue(m_rootGlyph);
}

QModelIndex	GlyphTreeModel::index(int row, int column, const QModelIndex& parent) const {
    return QModelIndex();
}

QModelIndex	GlyphTreeModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int	GlyphTreeModel::rowCount(const QModelIndex& parent) const {
    return 0;
}

bool GlyphTreeModel::setData(const QModelIndex& index, const QVariant & value, int role) {

    return true;
}

Qt::ItemFlags GlyphTreeModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void* GlyphTreeModel::GetANTzData() const {
    return m_antzData;
}

pNPnode GlyphTreeModel::GetRootGlyph() const {
    return m_rootGlyph;
}