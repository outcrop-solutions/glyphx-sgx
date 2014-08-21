#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QtCore/QItemSelection>
#include "glyphforestmodel.h"
#include <zSpace.h>

class ANTzViewerWidget : public QGLWidget
{
    Q_OBJECT

public:
	ANTzViewerWidget(GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent = 0);
	~ANTzViewerWidget();

	bool IsInStereoMode() const;
	bool IsStereoSupported() const;
	bool IsZSpaceAvailable() const;

	bool eventFilter(QObject *object, QEvent *event);

public slots:
    void ResetCamera();
	void SetStereo(bool enableStereo);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void moveEvent(QMoveEvent* event);

private slots:
    void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected);

private:
    void CenterCameraOnNode(pNPnode node);
    void InitIO();
	void CheckZSpaceError(ZSError error);
	void SetZSpacePosition();
	void ResizeZSpaceViewport();
	bool IsInZSpaceStereo() const;
	void SetZSpaceMatricesForDrawing(ZSEye eye, const ZSMatrix4& originialViewMatrix, NPcameraPtr camData);
	void DrawSelectedNodeAndHUDText();
	void ClearZSpaceContext();

    static QGLFormat s_format;

    QItemSelectionModel* m_selectionModel;
	GlyphForestModel* m_model;
    QPoint m_lastMousePosition;
	QWidget* m_topLevelWindow;

	ZSContext m_zSpaceContext;
	ZSHandle m_zSpaceDisplay;
	ZSHandle m_zSpaceBuffer;
	ZSHandle m_zSpaceViewport;
	ZSHandle m_zSpaceFrustum;
	ZSHandle m_zSpaceStylus;
	ZSMatrix4 m_originialProjectionMatrix;
};

#endif // ANTZWIDGET_H
