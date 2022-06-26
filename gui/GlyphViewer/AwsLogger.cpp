#include "AwsLogger.h"
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QFile>
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
		postData.append("{\r\n  \"userId\": \"" + userId + "\",\r\n  \"log\": \"" + logText.remove("\"") + "\"\r\n}");

		QNetworkReply* reply = mgr->post(request, postData);
		QEventLoop loop;
		QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

		if (reply->isRunning()) {
			loop.exec();
		}
		/*
		reply->deleteLater();

		if (reply->error() != QNetworkReply::NoError) {
			QFile file("debug_url.txt");
			if (file.open(QIODevice::ReadWrite)) {
				QTextStream stream(&file);
				stream << reply->errorString() << endl;
			}
		}
		*/
	}
	catch (...) {

	}
}

void AwsLogger::onFinish(QNetworkReply *rep)
{
	QFile file("debug_url.txt");
	if (file.open(QIODevice::ReadWrite)) {
		QTextStream stream(&file);
		stream << rep->readAll() << endl;
	}
}
