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
#pragma once
#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include "antzsingleglyphgui_global.h"
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLTexture>
#include <QtCore/QTimer>
#include "data/nptypes.h"

namespace SynGlyphXANTz {

	class ANTZSINGLEGLYPHGUI_EXPORT ANTzWidget : public QOpenGLWidget
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
		int PickPinAtPoint(const QPoint& point);

		virtual void DeleteNode(pNPnode node);
		void DeleteChildren(pNPnode parent, unsigned int first, unsigned int count);
		QOpenGLTexture* BindTextureInFile(const QString& imageFilename);

		void DrawLogo();

		virtual void BeforeDrawScene() = 0;
		virtual void AfterDrawScene() = 0;

		pData m_antzData;

		QSize m_logoSize;
		QOpenGLTexture* m_logoTextureID;

	private:
		void InitIO();
        
        QTimer timer;

		//static QGLFormat s_format;
	};

} //namespace SynGlyphXANTz

#endif // ANTZWIDGET_H
