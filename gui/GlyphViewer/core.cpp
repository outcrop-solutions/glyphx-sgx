#include "core.h"
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QWindow>
#include <QScreen>
//#include <QDesktopWidget>
#include <QBuffer>
#include "AwsLogger.h"
#include <QSettings>

Core::Core(QWidget *prt, QObject *parent)
	: QObject(parent), parent(prt)
{

}

void Core::SendDrawerPosition(const QString &text)
{
	AwsLogger::getInstance()->localLogger("SendDrawerPosition called.\n"+text);
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

		//QTimer::singleShot(1000, this, SLOT(TakeScreenShot("", x, y, w, h)));

		//QMessageBox::information(parent, tr("Error message"), "X: " + QString::number(x) + ", Y: " + QString::number(y) + ", W: " + QString::number(w) + ", H: " + QString::number(h));
	}
	catch (...) {
		QMessageBox::information(parent, tr("Error message"), "Failed to resize drawer.");
	}

}

void Core::TakeScreenShot(const QString &text, int x, int y, int w, int h)
{
	AwsLogger::getInstance()->localLogger("TakeScreenShot called.\n" + text);
	QRect crop_rect = glyphDrawer->frameGeometry();

	if (x != 0 || y != 0 || w != 0 || h != 0)
		crop_rect = QRect(x, y, w, h);

	auto window = parent->windowHandle();
	QScreen* screen = window->screen();

	QImage img = screen->grabWindow(
		glyphDrawer->winId(),
		crop_rect.left(),
		crop_rect.top()+60,
		crop_rect.width(),
		crop_rect.height()).toImage();

	QString name = "capture.png";
	if (!text.isEmpty())
		name = text;

	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	img.save(&buffer, "PNG");

	QSettings qsettings;
	qsettings.beginGroup("GlyphX_User");
	QString userID = qsettings.value("userid", "123456").toString();
	AwsLogger::getInstance()->imageWriter(userID, name, ba);

	/*QImage desk;
	desk.loadFromData(ba, "PNG");
	desk.save(name + ".png");*/

}

void Core::OpenProject(const QString &text, const bool load_from_cache)
{
	AwsLogger::getInstance()->localLogger("OpenProject called.\n" + text);
	if (!text.isEmpty() && !text.isNull())
	{
		emit OP(text, load_from_cache);
	}
}

void Core::ToggleDrawer(const bool flag)
{
	AwsLogger::getInstance()->localLogger("ToggleDrawer called.\n Flag value: " + flag);
	if (flag) {
		glyphDrawer->show();
		GetDrawerPosition();
	}
	else {
		glyphDrawer->hide();
		drawerDock->hide();
	}
}

void Core::ResizeEvent(const QString &text)
{
	AwsLogger::getInstance()->localLogger("ResizeEvent called.\n" + text);
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
		glyphDrawer->resize(QSize(w, h));
		glyphDrawer->move(x, y);
	}
	catch (...) {
		QMessageBox::information(parent, tr("Error message"), "Failed to resize drawer.");
	}

}

void Core::UpdateFilter(const QString &text)
{
	AwsLogger::getInstance()->localLogger("UpdateFilter called.\n" + text);
	emit UF(text);
}

void Core::ChangeState(const QString &text)
{
	AwsLogger::getInstance()->localLogger("ChangeState called.\n" + text);
	QMessageBox::information(parent, tr("Error message"), text);
	emit CS(text);
}

void Core::ReloadDrawer(const QString &text)
{
	AwsLogger::getInstance()->localLogger("ReloadDrawer called.\n" + text);
	emit RD(text);
}

void Core::GetCameraPosition(const QString &text) {

	std::vector<float> pos = viewer->getCameraPosition();
	QString json = "\"camera\": {\"position\": [" + QString::number(pos[0]) + "," + QString::number(pos[1]) + "," + QString::number(pos[2]) + "]," +
		"\"direction\": [" + QString::number(pos[3]) + "," + QString::number(pos[4]) + "," + QString::number(pos[5]) + "]}";
	SendCameraPosition(json);
}

void Core::GetDrawerStatus(const QString &text) {

	if (glyphDrawer->isHidden()) {
		SendDrawerStatus("Hidden");
	}
	else {
		SendDrawerStatus("Visible");
	}
}

void Core::GetSdtName(const QString &text) {

	emit SN(text);
}

void Core::CloseModel()
{
	AwsLogger::getInstance()->localLogger("CloseModel called.");
	emit CM();
}

void Core::LoadSettings(const QString &text)
{
	AwsLogger::getInstance()->localLogger("LoadSettings called.\n" + text);
	try {
		QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
		QJsonObject obj = doc.object();

		QJsonValue summation = true;
		QJsonValue explosion = false;
		if (obj.contains("summation")) {
			summation = obj.value("summation");
		}
		if (!summation.toBool() && obj.contains("explosion")) {
			explosion = obj.value("explosion");
		}

		QMap<QString, QJsonValue> settings;
		settings["summation"] = summation;
		settings["explosion"] = explosion;

		emit Settings(settings);

	}
	catch (...) {
		QMessageBox::information(parent, tr("Error message"), "Failed to parse settings.");
	}

}