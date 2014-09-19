#ifndef GLYPHTEMPLATESMODEL_H
#define GLYPHTEMPLATESMODEL_H

#include <QtCore/QAbstractItemModel>
#include "datatransformmapping.h"

class GlyphTemplatesModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	GlyphTemplatesModel(QObject *parent);
	~GlyphTemplatesModel();

	//Functions from QAbstractItemModel that need to be implemented
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex	parent(const QModelIndex& index) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	void AddGlyphFile(const QString& filename);
	void LoadDataTransformFile(const QString& filename);

	SynGlyphX::DataTransformMapping::SharedPtr GetDataTransform() const;

private:
	SynGlyphX::DataTransformMapping::SharedPtr m_dataTransform;
};

#endif // GLYPHTEMPLATESMODEL_H
