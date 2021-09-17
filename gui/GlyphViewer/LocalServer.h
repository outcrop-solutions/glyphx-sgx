#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QObject>
#include <QWidget>
#include <QWebSocketServer>
#include <QWebChannel>
#include "core.h"
#include "websocketclientwrapper.h"
#include "websockettransport.h"

class LocalServer : public QObject
{
	//Q_OBJECT

public:
	explicit LocalServer(QWidget *parent = nullptr);
	virtual ~LocalServer();

	void startServer(QString name, int port);

	Core* WebChannelCore() { return core; }

protected slots:
	void sendKeepAlive();

private:
	QWidget *parent;
	Core *core;
	QWebSocketServer *server;
	WebSocketClientWrapper *clientWrapper;
	QWebChannel *channel;
};

#endif // LOCALSERVER_H
