#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QtCore/QItemSelection>
#include "glyphtreemodel.h"

class ANTzWidget : public QGLWidget
{
    Q_OBJECT

public:
    ANTzWidget(GlyphTreeModel* model, QWidget *parent = 0);
    ~ANTzWidget();

public slots:
    void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void InitIO();

    static QGLFormat s_format;

    GlyphTreeModel* m_model;
};

#endif // ANTZWIDGET_H
