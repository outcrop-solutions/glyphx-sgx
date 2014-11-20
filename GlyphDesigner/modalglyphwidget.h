#ifndef MODALGLYPHWIDGET_H
#define MODALGLYPHWIDGET_H

#include "singleglyphwidget.h"
#include "minmaxglyphtreemodel.h"
#include <QtCore/QItemSelectionModel>

class ModalGlyphWidget : public SingleGlyphWidget
{
    Q_OBJECT

public:
	ModalGlyphWidget(MinMaxGlyphTreeModel::GlyphType glyphTreeType, QWidget *parent = 0);
    ~ModalGlyphWidget();

	void SetModel(MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel);

private slots:
	void OnGlyphUpdated(const QModelIndex& index);
	void OnWidgetUpdated(MinMaxGlyphTreeModel::PropertyUpdates updates);
    void SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    void ConnectWidgetSignals();
    void DisconnectWidgetSignals();
	void UpdateWidget(const QModelIndex& index);
    
	MinMaxGlyphTreeModel::GlyphType m_glyphTreeType;
	MinMaxGlyphTreeModel* m_model;
    QItemSelectionModel* m_selectionModel;

    std::vector<QMetaObject::Connection> m_propertyConnections;
	QMetaObject::Connection m_glyphUpdateConnection;
	QMetaObject::Connection m_selectionConnection;
};

#endif // MODALGLYPHWIDGET_H
