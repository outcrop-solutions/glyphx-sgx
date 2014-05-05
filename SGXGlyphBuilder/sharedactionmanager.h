#ifndef SHAREDACTIONMANAGER_H
#define SHAREDACTIONMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QItemSelectionModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QDialog>
#include "glyphtreemodel.h"
#include "singleglyphwidget.h"

class SharedActionManager : public QObject
{
    Q_OBJECT

public:
    SharedActionManager(GlyphTreeModel* model, QItemSelectionModel* selectionModel, QObject *parent);
    ~SharedActionManager();

    const QList<QAction*>& GetGlyphActions() const;

private slots:
    void SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void PropertiesActivated();

private:
    void CreateDialogs();
    void EnableActions(bool enable);

    QDialog* m_propertiesDialog;
    SingleGlyphWidget* m_glyphWidget;

    GlyphTreeModel* m_model;
    QItemSelectionModel* m_selectionModel;
    QList<QAction*> m_glyphActions;
};

#endif // SHAREDACTIONMANAGER_H
