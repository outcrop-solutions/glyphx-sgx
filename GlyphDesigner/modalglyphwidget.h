#ifndef MODALGLYPHWIDGET_H
#define MODALGLYPHWIDGET_H

#include "singleglyphwidget.h"
#include "minmaxglyphtreemodel.h"
#include <QtCore/QItemSelectionModel>

class ModalGlyphWidget : public SingleGlyphWidget
{
    Q_OBJECT

public:
    ModalGlyphWidget(MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel, QWidget *parent = 0);
    ~ModalGlyphWidget();

public slots:
	void OnNodeUpdated(const QModelIndex& index);

private slots:
	void OnWidgetUpdated(MinMaxGlyphTreeModel::PropertyUpdates updates);
    void SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    void ConnectWidgetSignals();
    void DisconnectWidgetSignals();
    void UpdateWidget(pNPnode node);
    
	MinMaxGlyphTreeModel* m_model;
    QItemSelectionModel* m_selectionModel;
    std::vector<QMetaObject::Connection> m_propertyConnections;
};

#endif // MODALGLYPHWIDGET_H
