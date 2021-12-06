#include "core.h"
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

Core::Core(QWidget *prt, QObject *parent)
	: QObject(parent), parent(prt)
{

}

void Core::SendDrawerPosition(const QString &text)
{
	try {
		QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
		QJsonObject obj = doc.object();
		QJsonObject projects = obj.value("filterSidebar").toObject();
		QJsonObject comments = obj.value("commentsSidebar").toObject();

		int x = projects.value("right").toInt();
		int y = projects.value("y").toInt();
		int w = comments.value("left").toInt() - x;
		int h = projects.value("height").toInt();

		/*int x = 512;
		int y = 64;
		int w = 1966;
		int h = 800;*/

		glyphDrawer->setMinimumSize(QSize(w, h));
		glyphDrawer->move(x, y);
	}
	catch (...) {
		QMessageBox::information(parent, tr("Error message"), "Failed to resize drawer.");
	}

}

void Core::OpenProject(const QString &text)
{
	emit OP(text);
}

void Core::ToggleDrawer(const QString &text)
{
	if (glyphDrawer->isHidden())
		glyphDrawer->show();
	else
		glyphDrawer->hide();
}

void Core::ResizeEvent(const QString &text)
{
	try {
		QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
		QJsonObject obj = doc.object();
		QJsonObject projects = obj.value("filterSidebar").toObject();
		QJsonObject comments = obj.value("commentsSidebar").toObject();

		int x = projects.value("right").toInt();
		int y = projects.value("y").toInt();
		int w = comments.value("left").toInt() - x;
		int h = projects.value("height").toInt();

		glyphDrawer->setMinimumSize(QSize(w, h));
		glyphDrawer->move(x, y);
	}
	catch (...) {
		QMessageBox::information(parent, tr("Error message"), "Failed to resize drawer.");
	}

}

void Core::UpdateFilter(const QString &text)
{
	emit UF(text);
}

void Core::ChangeState(const QString &text)
{
	emit CS(text);
}

void Core::ReloadDrawer(const QString &text)
{
	emit RD(text);
}

void Core::GetCameraPosition(const QString &text) {

	std::vector<float> pos = viewer->getCameraPosition();
	QString json = "\"camera\": {\"position\": [" + QString::number(pos[0]) + "," + QString::number(pos[1]) + "," + QString::number(pos[2]) + "]," +
		"\"direction\": [" + QString::number(pos[3]) + "," + QString::number(pos[4]) + "," + QString::number(pos[5]) + "]}";
	SendCameraPosition(json);
}