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

signals:
    void ObjectEdited(const QModelIndex& index);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

	virtual void BeforeDrawScene() = 0;
	virtual void AfterDrawScene() = 0;

	pData m_antzData;

private slots:
    void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected);
	void OnNodeDeleted(const QModelIndex& parent, int start, int end);
	void OnModelChanged();

private:
    void CenterCameraOnNode(pNPnode node);
    void InitIO();
	bool IsRootNodeSelected() const;

    static QGLFormat s_format;
};

#endif // ANTZWIDGET_H
