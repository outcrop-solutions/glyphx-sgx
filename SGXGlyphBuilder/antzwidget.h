#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QtCore/QItemSelection>
#include "glyphtreemodel.h"

class ANTzWidget : public QGLWidget
{
    Q_OBJECT

public:
    enum EditingMode {
        Move = 0,
        Rotate,
        Size
    };

    ANTzWidget(GlyphTreeModel* model, QItemSelectionModel* selectionModel, QWidget *parent = 0);
    ~ANTzWidget();

public slots:
    void ResetCamera();
    void SetEditingMode(EditingMode mode);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

private slots:
    void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected);

private:
    void CenterCameraOnNode(pNPnode node);
    void InitIO();

    static QGLFormat s_format;

    GlyphTreeModel* m_model;
    QItemSelectionModel* m_selectionModel;

    QPoint m_lastMousePosition;

    EditingMode m_editingMode;
};

#endif // ANTZWIDGET_H
