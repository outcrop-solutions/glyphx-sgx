#ifndef MODALGLYPHWIDGET_H
#define MODALGLYPHWIDGET_H

#include "singleglyphwidget.h"
#include "glyphtreemodel.h"
#include <QtCore/QItemSelectionModel>

class ModalGlyphWidget : public SingleGlyphWidget
{
    Q_OBJECT

public:
    ModalGlyphWidget(GlyphTreeModel* model, QItemSelectionModel* selectionModel, QWidget *parent = 0);
    ~ModalGlyphWidget();

private slots:
    void OnWidgetUpdated();
    void SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void OnNodeUpdated(const QModelIndex& index);

private:
    void ConnectWidgetSignals();
    void DisconnectWidgetSignals();
    void UpdateWidget(pNPnode node);
    
    GlyphTreeModel* m_model;
    QItemSelectionModel* m_selectionModel;
};

#endif // MODALGLYPHWIDGET_H
