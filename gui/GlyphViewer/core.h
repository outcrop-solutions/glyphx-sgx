#ifndef CORE_H
#define CORE_H

#include <QObject>
#include "SceneViewer/sceneviewer.h"

namespace SynGlyphX
{
	class SceneViewer;
}

/*
    An instance of this class gets published over the WebChannel and is then accessible to HTML clients.
*/
class Core : public QObject
{
    Q_OBJECT

public:
	Core(QWidget *prt, QObject *parent = nullptr);

	void SetDrawerWidget(QWidget *drawer) { glyphDrawer = drawer; };
	void SetViewerWidget(SynGlyphX::SceneViewer *m_viewer) { viewer = m_viewer; }

signals:
    /*
        This signal is emitted from the C++ side and the text displayed on the HTML client side.
    */
	void KeepAlive(const QString &text);
	void SendCameraPosition(const QString &text);
    void GetDrawerPosition();
	void OP(const QString &text);
	void UF(const QString &text);
	void CS(const QString &text);
	void RD(const QString &text);

public slots:

    /*
        This slot is invoked from the HTML client side and the text displayed on the server side.
    */
	void SendDrawerPosition(const QString &text);
	void OpenProject(const QString &text);
	void ToggleDrawer(const QString &text);
	void ResizeEvent(const QString &text);
	void UpdateFilter(const QString &text);
	void ChangeState(const QString &text);
	void ReloadDrawer(const QString &text);
	void GetCameraPosition(const QString &text);

private:
	QWidget *parent;
	QWidget *glyphDrawer;
	SynGlyphX::SceneViewer *viewer;
};

#endif // CORE_H
