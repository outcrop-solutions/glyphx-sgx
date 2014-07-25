#ifndef MINMAXGLYPHMODEL_H
#define MINMAXGLYPHMODEL_H

#include <QtCore/QAbstractTableModel>
#include "minmaxglyphtree.h"

class MinMaxGlyphModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	MinMaxGlyphModel(QObject *parent);
	~MinMaxGlyphModel();

	//Functions from QAbstractItemModel that need to be implemented
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;

public slots:
	void SetMinMaxGlyph(const QModelIndex& index);

private:
	QVariant GetDataByRow(const SynGlyphX::GlyphMappableProperties& minProperties, const SynGlyphX::GlyphMappableProperties& diffProperties, int row) const;

	SynGlyphX::MinMaxGlyphTree::iterator m_glyph;
};

#endif // MINMAXGLYPHMODEL_H
