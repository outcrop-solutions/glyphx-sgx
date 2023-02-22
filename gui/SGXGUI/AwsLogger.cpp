#include "AwsLogger.h"
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QFile>
#include <QDir>
#include <QEventLoop>

AwsLogger *AwsLogger::instance = 0;

AwsLogger* AwsLogger::getInstance() {
	if (!instance)
		instance = new AwsLogger();
	return instance;
}

void AwsLogger::logger(QString userId, QString logText) {

	try {
		QUrl url = QUrl("https://api.glyphx.co/etl/write-log-to-s3");

		QNetworkAccessManager * mgr = new QNetworkAccessManager();
		//QObject::connect(mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinish(QNetworkReply* rep)));
		QObject::connect(mgr, SIGNAL(finished(QNetworkReply*)), mgr, SLOT(deleteLater()));

		QNetworkRequest request(url);
		request.setHeader(request.ContentTypeHeader, "application/json");

		QByteArray postData;
		QString params = "{\r\n  \"userId\": \"" + userId + "\",\r\n  \"log\": \"" + logText.remove("\"") + "\"\r\n}";
		postData.append(params.toStdString());

		QNetworkReply* reply = mgr->post(request, postData);
		QEventLoop loop;
		QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

		if (reply->isRunning()) {
			loop.exec();
		}
	}
	catch (std::exception& e){
		const char *msg = e.what();
	}
}

void AwsLogger::imageWriter(QString userId, QString name, QByteArray image) {
	
	try {
		QUrl url = QUrl("https://api.glyphx.co/etl/write-image-to-s3");

		QNetworkAccessManager * mgr = new QNetworkAccessManager();
		//QObject::connect(mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinish(QNetworkReply* rep)));
		QObject::connect(mgr, SIGNAL(finished(QNetworkReply*)), mgr, SLOT(deleteLater()));

		QNetworkRequest request(url);
		request.setHeader(request.ContentTypeHeader, "application/json");

		QByteArray postData;
		QString params = "{\r\n  \"userId\": \"" + userId + "\",\r\n  \"name\": \"" + name.remove("\"") + "\",\r\n  \"image\": \"" + image.toBase64().data() + "\"\r\n}";
		postData.append(params.toStdString());

		QNetworkReply* reply = mgr->post(request, postData);
		QEventLoop loop;
		QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

		if (reply->isRunning()) {
			loop.exec();
		}
	}
	catch (std::exception& e) {
		const char *msg = e.what();
	}
}

void AwsLogger::localLogger(QString logText) {

	try {
		QDir dir(outputDir);
		if (!QDir(outputDir + "/logs").exists()) {
			dir.mkdir("logs");
		}

		QFile file(dir.absolutePath()+"/logs/log.txt");
		if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
			QTextStream stream(&file);
			stream << "[" + QDateTime::currentDateTime().toString() + "] " + logText << "\n";
		}

	}
	catch (std::exception& e) {
		const char *msg = e.what();
	}
}

void AwsLogger::onFinish(QNetworkReply *rep)
{
	QFile file("debug_url.txt");
	if (file.open(QIODevice::ReadWrite)) {
		QTextStream stream(&file);
		stream << rep->readAll() << "\n";
	}
}
