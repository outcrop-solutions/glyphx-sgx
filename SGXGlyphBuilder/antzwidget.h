#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include <QtOpenGL/QGLWidget>
#include "glyphtreemodel.h"

class ANTzWidget : public QGLWidget
{
    Q_OBJECT

public:
    ANTzWidget(GlyphTreeModel* model, QWidget *parent = 0);
    ~ANTzWidget();

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
