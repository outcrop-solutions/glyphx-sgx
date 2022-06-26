#include <QtCore/QString>
#include <QNetworkReply>

class AwsLogger : public QObject {
	
	// Private constructor so that no objects can be created.
	AwsLogger() {};

public:
	static AwsLogger* getInstance();

	void logger(QString userId, QString logText);

private:
	 static AwsLogger *instance;

private slots:
	void onFinish(QNetworkReply *rep);
	void httpError(QNetworkReply::NetworkError err);


};
