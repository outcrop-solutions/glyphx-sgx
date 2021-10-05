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
	QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
	QJsonObject obj = doc.object();
	int x = obj.value("x").toInt();
	int y = obj.value("y").toInt();
	int w = obj.value("width").toInt();
	int h = obj.value("height").toInt();
	int t = obj.value("top").toInt();
	int b = obj.value("bottom").toInt();
	int l = obj.value("left").toInt();
	int r = obj.value("right").toInt();
	QStringList qsl = obj.keys();

	glyphDrawer->setMinimumSize(QSize(r-(w*3)-1, h));
	glyphDrawer->move(w*2, t);

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
	QMessageBox::information(parent, tr("Client message"), text);
}

void Core::UpdateFilter(const QString &text)
{
	QMessageBox::information(parent, tr("Client message"), text);
}

void Core::ChangeState(const QString &text)
{
	QMessageBox::information(parent, tr("Client message"), text);
}

void Core::ReloadDrawer(const QString &text)
{
	QMessageBox::information(parent, tr("Client message"), text);
}