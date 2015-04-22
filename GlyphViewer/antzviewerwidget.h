///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

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
	ANTzViewerWidget(const QGLFormat& format, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent = 0);
	~ANTzViewerWidget();

	bool IsInStereoMode() const;
	//bool IsStereoSupported() const;
	bool IsZSpaceAvailable() const;

	bool eventFilter(QObject *object, QEvent *event);

	static const QGLFormat& GetNonStereoFormat();
	static const QGLFormat& GetStereoFormat();

signals:
	void NewStatusMessage(const QString& message, int timeout = 0) const;

public slots:
    void ResetCamera();
	//void SetStereo(bool enableStereo);
	void SetHideUnselectedGlyphTrees(bool hideUnselectedGlyphTrees);

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
	void OnModelReset();

private:
	enum Eye {
		Left,
		Right
	};

	void DrawSceneForEye(Eye eye, bool drawHUD);
	void SetCameraToDefaultPosition();
	unsigned int BindTextureInFile(const QString& imageFilename);
	void SetGridTexture(pNPnode grid);
    void CenterCameraOnNode(pNPnode node);
    void InitIO();
	void DrawHUD();
	bool SelectAtPoint(int x, int y, bool multiSelect);

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

	void UpdateGlyphTreesShowHideForSelection();
	void ShowAllGlyphTrees();

    static QGLFormat s_format;
	static QGLFormat s_stereoFormat;
	static const float s_stylusLength;

	QFont m_oglTextFont;
	bool m_isReseting;

	bool m_hideUnselectedGlyphTrees;

	unsigned int m_worldTextureID;
	std::vector<unsigned int> m_textureIDs;

	ANTzPlus::ANTzData::SharedPtr m_antzData;

    QItemSelectionModel* m_selectionModel;
	GlyphForestModel* m_model;
    QPoint m_lastMousePosition;
	QRect m_regionSelectionRect;
	QWidget* m_topLevelWindow;
	//QPoint m_zSpaceStylusScreenPoint;

	ZSContext m_zSpaceContext;
	ZSHandle m_zSpaceDisplay;
	ZSHandle m_zSpaceBuffer;
	ZSHandle m_zSpaceViewport;
	ZSHandle m_zSpaceFrustum;
	ZSHandle m_zSpaceStylus;
	ZSMatrix4 m_originialProjectionMatrix;
	ZSMatrix4 m_zSpaceStylusWorldMatrix;
	ZSMatrix4 m_originialViewMatrix;
	ZSVector3 m_zSpaceStylusLastPosition;
	ZSVector3 m_zSpaceStylusIntersectionPosition;
};

#endif // ANTZWIDGET_H
