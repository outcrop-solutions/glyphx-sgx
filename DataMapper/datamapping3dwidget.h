#ifndef DATAMAPPING3DWIDGET_H
#define DATAMAPPING3DWIDGET_H

#include "minmaxglyph3dwidget.h"
#include "roledatafilterproxymodel.h"
#include "datatransformmodel.h"
#include <stack>

class DataMapping3DWidget : public SynGlyphXANTz::MinMaxGlyph3DWidget
{
	Q_OBJECT

public:
	DataMapping3DWidget(DataTransformModel* model, QWidget *parent = 0);
	~DataMapping3DWidget();

	virtual void SetModel(SynGlyphX::RoleDataFilterProxyModel* model, QItemSelectionModel* selectionModel);

private slots:
	void OnInternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnExternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnExternalDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
	void OnExternalModelReset();

private:
	void Clear();
	void ConnectInternalSelection();
	void ConnectExternalSelection();
	std::stack<unsigned int> GetRowIndiciesFromStack(const QModelIndex& index) const;
	QModelIndex GetModelIndexFromStack(std::stack<unsigned int> rows, const QAbstractItemModel* const model) const;

	DataTransformModel* m_dataTransformModel;

	SynGlyphXANTz::MinMaxGlyphTreeModel* m_internalModel;
	QItemSelectionModel* m_internalSelectionModel;

	SynGlyphX::RoleDataFilterProxyModel* m_externalModel;
	QItemSelectionModel* m_externalSelectionModel;

	QMetaObject::Connection m_internalSelectionConnection;
	QMetaObject::Connection m_externalSelectionConnection;

	int m_glyphTreeIndex;
};

#endif // DATAMAPPING3DWIDGET_H
