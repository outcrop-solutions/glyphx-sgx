#ifndef SHAREDACTIONMANAGER_H
#define SHAREDACTIONMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QItemSelectionModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QDialog>
#include "glyphtreemodel.h"
#include "singleglyphwidget.h"
#include "addchildrendialog.h"

class SharedActionManager : public QObject
{
    Q_OBJECT

public:
    SharedActionManager(GlyphTreeModel* model, QItemSelectionModel* selectionModel, QObject *parent);
    ~SharedActionManager();

    const QList<QAction*>& GetGlyphActions() const;
    const QList<QAction*>& GetEditActions() const;

public slots:
    void AddChildren();

private slots:
    void SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void PropertiesActivated();
    void DeleteSelected();
    void DeleteChildrenFromSelected();

private:
    void CreateGlyphActions();
    void CreateEditActions();
    void CreatePropertiesDialog();
    void CreateAddChildrenDialog();
    void EnableActions();
    QAction* CreateSeparator();

    QDialog* m_propertiesDialog;
    SingleGlyphWidget* m_glyphWidget;

    AddChildrenDialog* m_addChildrenDialog;

    GlyphTreeModel* m_model;
    QItemSelectionModel* m_selectionModel;
    QList<QAction*> m_glyphActions;
    QList<QAction*> m_editActions;
    QAction* m_cutAction;
    QAction* m_copyAction;
    QAction* m_pasteAction;
    QAction* m_pasteAsChildAction;
    QAction* m_deleteAction;
    QAction* m_deleteChildrenAction;
    QAction* m_propertiesAction;

    QAction* m_addChildrenAction;
};

#endif // SHAREDACTIONMANAGER_H
