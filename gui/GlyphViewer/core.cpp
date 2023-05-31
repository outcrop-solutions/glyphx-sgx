#include "core.h"
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QWindow>
#include <QScreen>
#include <QDesktopWidget>
#include <QBuffer>
#include "AwsLogger.h"
#include <QSettings>
#include <QJsonArray>
#include <QJsonDocument>
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

		int x = obj.value("x").toInt();
		int y = obj.value("y").toInt();
		int w = obj.value("w").toInt();
		int h = obj.value("h").toInt();


		glyphDrawer->setMinimumSize(QSize(w, h));
		glyphDrawer->move(x, y);

		//QTimer::singleShot(1000, this, SLOT(TakeScreenShot("", x, y, w, h)));

		//QMessageBox::information(parent, tr("Error message"), "X: " + QString::number(x) + ", Y: " + QString::number(y) + ", W: " + QString::number(w) + ", H: " + QString::number(h));
	}
	catch (...) {
		QMessageBox::information(parent, tr("Error message"), "Failed to resize drawer.");
	}

}

void Core::TakeScreenShot()
{
		
	QPixmap pixmap = glyphDrawer->grab();

	// Save the screenshot as a PNG image
	//QString fileName = "widget_screenshot.png";
	//pixmap.save(fileName, "PNG");


	// Convert QPixmap to QImage
	QImage image = pixmap.toImage();

	// Convert QImage to QByteArray
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "PNG");
	buffer.close();

	// Base64 encode the QByteArray
	QString base64Data = QString::fromLatin1(byteArray.toBase64());

	// Create JSON object
	QJsonObject jsonObject;
	jsonObject["imageData"] = base64Data;

	// Convert JSON object to QJsonDocument
	QJsonDocument jsonDoc(jsonObject);

	// Convert QJsonDocument to QString
	QString jsonString = QString::fromUtf8(jsonDoc.toJson());
	emit SendScreenShot(jsonString);
	
	QSettings qsettings;
	qsettings.beginGroup("GlyphX_User");
	QString userID = qsettings.value("userid", "123456").toString();
	

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
	AwsLogger::getInstance()->localLogger("ToggleDrawer called.\n Flag value: " + QString(flag));
	if (flag) {
		glyphDrawer->show();
		GetDrawerPosition();
	}
	else {
		glyphDrawer->hide();
		
	}
}

void Core::ResizeEvent(const QString &text)
{
	AwsLogger::getInstance()->localLogger("ResizeEvent called.\n" + text);
	try {
		QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
		QJsonObject obj = doc.object();

		int x = obj.value("x").toInt();
		int y = obj.value("y").toInt();
		int w = obj.value("w").toInt();
		int h = obj.value("h").toInt();

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

void Core::SendRowIdsToClient(const QList<int>& rowIds) {
	QString arr_str = "[";
	for (int i = 0; i < rowIds.size(); i++)
	{
		arr_str += QString::number(rowIds[i]);
		if (i < rowIds.size() - 1)
			arr_str += ",";
	}
	arr_str += "]";
	QString json = "{\"rowIds\":" + arr_str + "}";
	emit SendRowIds(json);
}
