#ifndef ANTZWIDGET_H
#define ANTZWIDGET_H

#include "antzgui_global.h"
#include <QtOpenGL/QGLWidget>
#include "data/nptypes.h"

namespace SynGlyphXANTz {

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
		int PickPinAtPoint(const QPoint& point) const;

		void DeleteNode(pNPnode node);
		void DeleteChildren(pNPnode parent, unsigned int first, unsigned int count);
		unsigned int BindTextureInFile(const QString& imageFilename);

		virtual void BeforeDrawScene() = 0;
		virtual void AfterDrawScene() = 0;

		pData m_antzData;

	private:
		void InitIO();

		static QGLFormat s_format;
	};

} //namespace SynGlyphXANTz

#endif // ANTZWIDGET_H
