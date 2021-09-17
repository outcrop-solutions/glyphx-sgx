#include "core.h"
#include <QtWidgets/QMessageBox>

Core::Core(QWidget *prt, QObject *parent)
	: QObject(parent), parent(prt)
{

}

void Core::SendDrawerPosition(const QString &text)
{
	QMessageBox::information(parent, tr("Client message"), text);
}

void Core::OpenProject(const QString &text)
{
	//QMessageBox::information(parent, tr("Client message"), text);
	emit OP(text);
}

void Core::ToggleDrawer(const QString &text)
{
	QMessageBox::information(parent, tr("Client message"), text);
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