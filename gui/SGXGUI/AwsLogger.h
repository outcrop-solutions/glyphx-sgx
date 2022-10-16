#include <QtCore/QString>
#include <QNetworkReply>
#include "sgxgui_global.h"

class SGXGUI_EXPORT AwsLogger : public QObject {

	// Private constructor so that no objects can be created.
	AwsLogger() {};

public:
	static AwsLogger* getInstance();

	void setCommonDataPath(QString path) { outputDir = path; }

	void logger(QString userId, QString logText);
	void imageWriter(QString userId, QString name, QByteArray image);
	void localLogger(QString logText);

private:
	 static AwsLogger *instance;
	 QString outputDir;

private slots:
	void onFinish(QNetworkReply *rep);
	void httpError(QNetworkReply::NetworkError err);

};
