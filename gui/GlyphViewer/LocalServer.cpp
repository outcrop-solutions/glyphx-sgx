#include "LocalServer.h"

#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QDir>
#include <QUrl>
#include <QtWidgets/QMessageBox>
#include <QDesktopServices>

LocalServer::LocalServer(QWidget *parent)
	: QObject(parent),
	parent(parent)
{

}

LocalServer::~LocalServer()
{}

void LocalServer::sendKeepAlive()
{
	core->KeepAlive("Keep Alive");
}

void LocalServer::startServer(QString name, int port)
{
	QFileInfo jsFileInfo(QDir::currentPath() + "/qwebchannel.js");

	if (!jsFileInfo.exists())
		QFile::copy(":/qtwebchannel/qwebchannel.js", jsFileInfo.absoluteFilePath());

	// setup the QWebSocketServer
	QWebSocketServer *server = new QWebSocketServer(name, QWebSocketServer::NonSecureMode);
	if (!server->listen(QHostAddress::LocalHost, port)) {
		QMessageBox::information(parent, tr("Server message"), "Failed to open web socket server for filtering.");
	}

	// wrap WebSocket clients in QWebChannelAbstractTransport objects
	clientWrapper = new WebSocketClientWrapper(server);

	// setup the channel
	channel = new QWebChannel;
	QObject::connect(clientWrapper, &WebSocketClientWrapper::clientConnected,
		channel, &QWebChannel::connectTo);

	// setup the core and publish it to the QWebChannel
	core = new Core(parent);
	channel->registerObject(QStringLiteral("core"), core);

	// open a browser window with the client HTML page
	//QUrl url = QUrl::fromLocalFile(QDir::currentPath() + "/index.html");
	//QDesktopServices::openUrl(url);
	//"?webChannelBaseUrl=ws://localhost:" + QString::number(ws_port))

	QTimer *t = new QTimer(this);
	connect(t, &QTimer::timeout, this, &LocalServer::sendKeepAlive);
	t->start(30000);

}
