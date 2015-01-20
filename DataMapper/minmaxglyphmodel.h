#ifndef MINMAXGLYPHMODEL_H
#define MINMAXGLYPHMODEL_H

#include <QtCore/QAbstractTableModel>
#include "datamappingglyphgraph.h"
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

	SynGlyphX::DataTransformMapping::ConstSharedPtr GetDataTransformMapping() const;
	bool IsCurrentGlyphRoot() const;
	bool IsClear() const;

public slots:
	void SetMinMaxGlyph(const QModelIndex& index);
	void Clear();
	void ClearInputBindings();

private:
	QVariant GetDataByRow(SynGlyphX::DataMappingGlyph& glyph, const QModelIndex& index) const;
	bool SetDataByRow(SynGlyphX::DataMappingGlyph& glyph, const QVariant& value, const QModelIndex& index);
	SynGlyphX::NumericMappingProperty& GetGlyphProperty(SynGlyphX::DataMappingGlyph& glyph, int row) const;
	bool IsTextField(int row) const;
	bool IsColorField(int row) const;

	SynGlyphX::DataMappingGlyphGraph::const_iterator m_glyph;
	SynGlyphX::DataMappingGlyphGraph::ConstSharedPtr m_glyphTree;
	boost::uuids::uuid m_glyphTreeID;
	QStringList m_propertyHeaders;
	QStringList m_columnHeaders;
	DataTransformModel* m_dataTransformModel;

	QPersistentModelIndex m_selectedIndex;
};

#endif // MINMAXGLYPHMODEL_H
