#ifndef MINMAXGLYPHMODEL_H
#define MINMAXGLYPHMODEL_H

#include <QtCore/QAbstractTableModel>
#include "minmaxglyphtree.h"
#include "datatransformmodel.h"

class MinMaxGlyphModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	MinMaxGlyphModel(DataTransformModel* dataTransformModel, QObject *parent = nullptr);
	~MinMaxGlyphModel();

	//Functions from QAbstractItemModel that need to be implemented
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public slots:
	void SetMinMaxGlyph(const QModelIndex& index);

private:
	QVariant GetDataByRow(const SynGlyphX::GlyphMappableProperties& minProperties, const SynGlyphX::GlyphMappableProperties& diffProperties, int row) const;
	bool SetDataByRow(SynGlyphX::GlyphMappableProperties& minProperties, SynGlyphX::GlyphMappableProperties& diffProperties, const QVariant& value, const QModelIndex& index);

	SynGlyphX::MinMaxGlyphTree::iterator m_glyph;
	const SynGlyphX::MinMaxGlyphTree* m_glyphTree;
	boost::uuids::uuid m_glyphTreeID;
	QStringList m_propertyHeaders;
	QStringList m_columnHeaders;
	DataTransformModel* m_dataTransformModel;
};

#endif // MINMAXGLYPHMODEL_H
