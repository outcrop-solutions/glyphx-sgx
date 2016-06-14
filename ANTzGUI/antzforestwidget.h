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

#ifndef ANTZFORESTWIDGET_H
#define ANTZFORESTWIDGET_H

#include "antzgui_global.h"
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLFunctions>
#include <QtCore/QTimer>
#include "itemfocusselectionmodel.h"
#include "glyphforestmodel.h"
#include "antzdata.h"
#include "antzboundingbox.h"
#ifdef USE_ZSPACE
#include <zSpace.h>
#include "zspaceoptions.h"
#include "zspaceeventdispatcher.h"
#endif
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <gl/glu.h>
#endif

namespace SynGlyphXANTz {

	class ANTZGUI_EXPORT ANTzForestWidget : public QOpenGLWidget
	{
		Q_OBJECT

	public:
		enum FilteredResultsDisplayMode {

			TranslucentUnfiltered,
			HideUnfiltered
		};

		enum HUDLocation {

			TopLeft = 0,
			BottomLeft,
			BottomRight
		};

		ANTzForestWidget( GlyphForestModel* model, SynGlyphX::ItemFocusSelectionModel* selectionModel, QWidget *parent = 0 );
		~ANTzForestWidget();

		bool IsInStereoMode() const;
		bool IsStereoSupported() const;
		bool IsZSpaceAvailable() const;

		bool eventFilter( QObject *object, QEvent *event );

#ifdef USE_ZSPACE
		void SetZSpaceOptions( const SynGlyphX::ZSpaceOptions& options );
		const SynGlyphX::ZSpaceOptions& GetZSpaceOptions() const;
#endif

		FilteredResultsDisplayMode GetFilteredResultsDisplayMode() const;

		void SetBackgroundColor( const SynGlyphX::GlyphColor& color );

		bool SetStereoMode( bool stereoOn );

		void SetShowHUDAxisInfoObject(bool show);
		bool GetShowHUDAxisInfoObject() const;

		void SetAxisInfoObjectLocation(HUDLocation location);
		HUDLocation GetAxisInfoObjectLocation() const;

	signals:
		//void NewStatusMessage(const QString& message, int timeout = 0) const;

	public slots :
		void ResetCamera();

		void SetFilteredResultsDisplayMode( FilteredResultsDisplayMode mode );
		void SetFilteredResults( const SynGlyphX::IndexSet& filteredResults );
		void ClearFilteredResults();

		void ShowAnimatedRotations( bool show );

		void SetShowTagsOfSelectedObjects( bool showTagsOfSelectedObjects );
		void ClearAllTags();

	protected slots:
		void OnSelectionUpdated( const QItemSelection& selected, const QItemSelection& deselected );
		void OnFocusChanged( const QModelIndexList& indexes );
		void OnModelReset();

#ifdef USE_ZSPACE
		void ZSpaceButtonPressHandler( ZSHandle targetHandle, const ZSTrackerEventData* eventData );
		void ZSpaceButtonReleaseHandler( ZSHandle targetHandle, const ZSTrackerEventData* eventData );
		void ZSpaceStylusMoveHandler( ZSHandle targetHandle, const ZSTrackerEventData* eventData );
		void ZSpaceStylusTapHandler( ZSHandle targetHandle, const ZSTrackerEventData* eventData );
#endif

	protected:
		enum Eye {
			Left,
			Right
		};

		void initializeGL() override;
		void resizeGL( int w, int h ) override;
		void paintGL() override;

		void mousePressEvent( QMouseEvent* event ) override;
		void mouseReleaseEvent( QMouseEvent* event ) override;
		void mouseMoveEvent( QMouseEvent* event ) override;
		void keyPressEvent( QKeyEvent* event ) override;
		void keyReleaseEvent( QKeyEvent* event ) override;
		void moveEvent( QMoveEvent* event ) override;
		void wheelEvent( QWheelEvent* event ) override;

		void DrawSceneForEye( Eye eye, bool getStylusWorldPosition );
		void SetCameraToDefaultPosition();
		QOpenGLTexture* BindTextureInFile( const QString& imageFilename );
		void SetGridTexture( pNPnode grid );
		void SetGridLinesColor( pNPnode grid, const QColor& color );
		void CenterCameraOnNode( pNPnode node );
		void InitIO();
		void* CreateNewQuadricObject();
		void DrawHUDAxisInfoObject();
		void DrawSceneAxisInfoObject();
		void DrawHUD();
		bool IsHUDLocationOnRightSide( HUDLocation location ) const;
		bool SelectAtPoint( int x, int y, bool multiSelect );
		void SelectFromStylus( const SynGlyphXANTz::ANTzBoundingBox::Line& line );
		void CheckStylusIntersectionWithNode( pNPnode node, const SynGlyphXANTz::ANTzBoundingBox::Line& line, std::map<float, int>& distanceIdMap );
        
        // Drop-in replacements for old QGLWidget functionality.
        void renderText(double x, double y, double z, const QString &str, const QFont & font = QFont());
        void renderText(int x, int y, const QString &str, const QFont & font = QFont());
        void qglColor(QColor color);

#ifdef USE_ZSPACE
		void ConnectZSpaceTrackers();
		void CheckZSpaceError( ZSError error );
		void SetZSpacePosition();
		void ResizeZSpaceViewport();
		bool IsInZSpaceStereo() const;
		void SetZSpaceMatricesForDrawing( ZSEye eye, const ZSMatrix4& originialViewMatrix, NPcameraPtr camData );
		void ClearZSpaceContext();
		void DrawZSpaceStylus( const ZSMatrix4& stylusMatrix, bool getStylusWorldPosition );
#endif

		//void ShowOnlyFilteredResultGlyphTrees();
		//void ShowAllGlyphTrees();
		//void UpdateGlyphTreesForFilteredResults();

		void CreateBoundingBoxes();
		void CreateBoundingBoxes( pNPnode node, const glm::mat4& parentTransform, bool isAncestorBoundingBoxBeingUpdated );
		void UpdateBoundingBoxes();
		void UpdateBoundingBoxes( pNPnode node, const glm::mat4& parentTransform );
		void DrawBoundingBoxes();

		void StoreRotationRates();
		void StoreRotationRates( pNPnode node );
		bool IsNodeAnimated( pNPnode node );

		void DrawLogo();

		void HideFilteredGlyph( bool hide );
		void HideFilteredGlyph( pNPnode node, bool hide );

		//QGLFormat m_glFormat;
		//QGLFormat m_glStereoFormat;

		bool m_isInStereo;

		QFont m_oglTextFont;
		bool m_isReseting;
		bool m_drawHUD;

		FilteredResultsDisplayMode m_filteredResultsDisplayMode;
		SynGlyphX::IndexSet m_filteredResults;

#ifdef USE_ZSPACE
		SynGlyphX::ZSpaceOptions m_zSpaceOptions;
#endif

		QOpenGLTexture* m_worldTextureID;
		std::vector<QOpenGLTexture*> m_textureIDs;

		ANTzPlus::ANTzData::SharedPtr m_antzData;

		SynGlyphX::ItemFocusSelectionModel* m_selectionModel;
		GlyphForestModel* m_model;
		boost::optional<QPoint> m_lastMousePosition;
		QRect m_regionSelectionRect;
		QWidget* m_topLevelWindow;
		//QPoint m_zSpaceStylusScreenPoint;

#ifdef USE_ZSPACE
		ZSContext m_zSpaceContext;
		ZSHandle m_zSpaceDisplay;
		ZSHandle m_zSpaceBuffer;
		ZSHandle m_zSpaceViewport;
		ZSHandle m_zSpaceFrustum;
		ZSHandle m_zSpaceStylus;
		ZSMatrix4 m_originialProjectionMatrix;
		ZSMatrix4 m_zSpaceStylusWorldMatrix;
		ZSMatrix4 m_zSpaceDisplayWorldMatrix;
		ZSMatrix4 m_originialViewMatrix;
		ZSVector3 m_zSpaceStylusLastPosition;
		SynGlyphX::ZSpaceEventDispatcher m_zSpaceEventDispatcher;
#endif

		SynGlyphXANTz::ANTzBoundingBox::Line m_stylusWorldLine;
		//SynGlyphXANTz::ANTzBoundingBox::Line m_stylusWorldTapLine;
		std::map<int, SynGlyphXANTz::ANTzBoundingBox> m_boundingBoxes;
		std::set<int> m_objectsThatNeedBoundingBoxUpdates;

		std::map<int, NPfloatXYZ> m_rotationRates;
		bool m_showAnimation;

		QOpenGLTexture* m_logoTextureID;
		QRect m_logoPosition;

		float m_initialCameraZAngle;

		QSet<QModelIndex> m_tagIndexes;

		std::vector<std::pair<QRect, GLuint>> m_renderedIcons;

		GLUquadric* m_sceneAxisInfoQuadric;

		bool m_showSceneAxisInfoObject;
		bool m_showHUDAxisInfoObject;
		HUDLocation m_sceneAxisInfoObjectLocation;
		QRect m_sceneAxisInfoViewport;
		QRectF m_sceneAxisInfoOrtho;
        
        QTimer timer;
	};

} //namespace SynGlyphXANTz

#endif // ANTZFORESTWIDGET_H
