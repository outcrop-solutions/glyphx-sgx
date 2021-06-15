#include "DownloadManager.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include "downloadexception.h"
#include <QtCore/QFile>
#include <QtCore/QDebug>


DownloadManager::DownloadManager(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection) : m_dataEngineConnection(dataEngineConnection)
{

}

DownloadManager::~DownloadManager()
{

}

bool DownloadManager::DownloadFile(const QUrl &url, const QString &filename) {


	QNetworkRequest request(url);
	QNetworkAccessManager networkManager;
	QNetworkReply *reply = networkManager.get(request);

	QEventLoop loop;
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

	// Execute the event loop here, now we will wait here until readyRead() signal is emitted
	// which in turn will trigger event loop quit.
	if (reply->isRunning()) {
		loop.exec();
	}

	reply->deleteLater();

	if (reply->error() != QNetworkReply::NoError) {
		qDebug() << (reply->errorString().toStdString()).c_str();
		//std::cout << "Network Error: " << reply->errorString().toStdString() << std::endl;
		throw DownloadException(("Network Error: " + reply->errorString().toStdString()).c_str());
	}

	QByteArray data = reply->readAll();
	if (data.isEmpty()) {

		throw DownloadException("Download returned no data");
	}

	return SaveToDisk(filename, data);
}

bool DownloadManager::SaveToDisk(const QString &filename, const QByteArray &data) {

	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		fprintf(stderr, "Could not open %s for writing: %s\n",
			qPrintable(filename),
			qPrintable(file.errorString()));
		return false;
	}

	file.write(data);
	file.close();

	QString zip(filename.toStdString().c_str());
	m_dataEngineConnection->UnZipFile(zip);

	return true;
}

