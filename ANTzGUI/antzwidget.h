#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include "antzgui_global.h"
#include <QtOpenGL/QGLWidget>

class ANTZGUI_EXPORT ANTzWidget : public QGLWidget
{
    Q_OBJECT

public:
	ANTzWidget(QWidget *parent = 0);
    ~ANTzWidget();

public slots:
    void ResetCamera();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

	void CenterCameraOnNode(pNPnode node);

	virtual void BeforeDrawScene() = 0;
	virtual void AfterDrawScene() = 0;

	pData m_antzData;

private:
    void InitIO();

    static QGLFormat s_format;
};

#endif // ANTZWIDGET_H
