#ifndef DATAMAPPING3DWIDGET_H
#define DATAMAPPING3DWIDGET_H

#include "minmaxglyph3dwidget.h"
#include "roledatafilterproxymodel.h"
#include "datatransformmodel.h"

class DataMapping3DWidget : public MinMaxGlyph3DWidget
{
	Q_OBJECT

public:
	DataMapping3DWidget(DataTransformModel* model, QWidget *parent = 0);
	~DataMapping3DWidget();

	virtual void SetModel(SynGlyphX::RoleDataFilterProxyModel* model, QItemSelectionModel* selectionModel);

private slots:
	void OnInternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnExternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	void ConnectInternalSelection();
	void ConnectExternalSelection();

	DataTransformModel* m_dataTransformModel;

	MinMaxGlyphTreeModel* m_internalModel;
	QItemSelectionModel* m_internalSelectionModel;

	SynGlyphX::RoleDataFilterProxyModel* m_externalModel;
	QItemSelectionModel* m_externalSelectionModel;

	QMetaObject::Connection m_internalSelectionConnection;
	QMetaObject::Connection m_externalSelectionConnection;

	int m_glyphTreeIndex;
};

#endif // DATAMAPPING3DWIDGET_H
