#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include <QtOpenGL/QGLWidget>

class ANTzWidget : public QGLWidget
{
    Q_OBJECT

public:
    ANTzWidget(QWidget *parent = 0);
    ~ANTzWidget();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void InitIO();

    static QGLFormat s_format;

    void* m_antzData;
};

#endif // ANTZWIDGET_H
