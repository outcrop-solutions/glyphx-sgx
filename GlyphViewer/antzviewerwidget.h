#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QtCore/QItemSelection>
#include "glyphforestmodel.h"
#include <zSpace.h>
#include "antzdata.h"

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

signals:
	void NewStatusMessage(const QString& message, int timeout = 0) const;

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
	virtual void wheelEvent(QWheelEvent* event);

private slots:
    void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected);

private:
    void CenterCameraOnNode(pNPnode node);
    void InitIO();
	void DrawSelectedNodeAndHUDText();
	void SelectAtPoint(int x, int y) const;

	void CheckZSpaceError(ZSError error);
	void SetZSpacePosition();
	void ResizeZSpaceViewport();
	bool IsInZSpaceStereo() const;
	void SetZSpaceMatricesForDrawing(ZSEye eye, const ZSMatrix4& originialViewMatrix, NPcameraPtr camData);
	void ClearZSpaceContext();
	void DrawZSpaceStylus(const ZSMatrix4& stylusMatrix, ZSFloat stylusLength, QColor color);

	void ZSpaceButtonPressHandler(ZSHandle targetHandle, const ZSTrackerEventData* eventData);
	void ZSpaceButtonReleaseHandler(ZSHandle targetHandle, const ZSTrackerEventData* eventData);
	void ZSpaceStylusMoveHandler(ZSHandle targetHandle, const ZSTrackerEventData* eventData);
	static void ZSpaceEventDispatcher(ZSHandle targetHandle, const ZSTrackerEventData* eventData, const void* userData);

    static QGLFormat s_format;
	static const float s_stylusLength;

	QFont m_oglTextFont;

	ANTzPlus::ANTzData::SharedPtr m_antzData;

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
	ZSVector3 m_zSpaceStylusLastPosition;
};

#endif // ANTZWIDGET_H
