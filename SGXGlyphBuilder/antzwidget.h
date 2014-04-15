#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include <QtOpenGL/QGLWidget>

class ANTzWidget : public QGLWidget
{
    Q_OBJECT

public:
    ANTzWidget(QWidget *parent = 0);
    ~ANTzWidget();

private:
    
};

#endif // ANTZWIDGET_H
