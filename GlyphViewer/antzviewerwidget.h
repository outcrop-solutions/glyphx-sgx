#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QtCore/QItemSelection>
#include "glyphforestmodel.h"

class ANTzViewerWidget : public QGLWidget
{
    Q_OBJECT

public:
	ANTzViewerWidget(GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent = 0);
	~ANTzViewerWidget();

public slots:
    void ResetCamera();

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

    QItemSelectionModel* m_selectionModel;
	GlyphForestModel* m_model;
    QPoint m_lastMousePosition;
};

#endif // ANTZWIDGET_H
