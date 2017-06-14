#include "AnnouncementDialog.h"
#include <QtWidgets/QDialogButtonBox>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include <QtCore/QTextCodec>

namespace SynGlyphX {

	AnnouncementDialog::AnnouncementDialog(QString title, QWidget *parent)
		: QDialog(parent), firstTime(true)
	{
		setWindowTitle(title);

		layout = new QVBoxLayout();

		setStyleSheet("font-family:'Calibri', Helvetica, Arial, Sans;");
		//setAttribute(Qt::WA_DeleteOnClose);
	}

	AnnouncementDialog::~AnnouncementDialog()
	{
	}

	QString AnnouncementDialog::UseURLText(QUrl url){

		QNetworkRequest request(url);
		QNetworkAccessManager networkManager;
		QNetworkReply *reply = networkManager.get(request);

		QEventLoop loop;
		QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

		if (reply->isRunning()) {
			loop.exec();
		}

		reply->deleteLater();

		if (reply->error() != QNetworkReply::NoError) {
			return QString();
		}

		QByteArray dialogData = reply->readAll();
		if (dialogData.isEmpty()) {
			return QString();
		}
		return QTextCodec::codecForMib(106)->toUnicode(dialogData);
	}

	void AnnouncementDialog::AddLabel(QString url){

		m_label = new QLabel(this);
		m_label->setWordWrap(true);
		m_label->setOpenExternalLinks(true);
		m_label->setStyleSheet("font-family:'Calibri', Helvetica, Arial, Sans; font-size: 10pt;");
		m_label->setText(UseURLText(QUrl(url)));
		layout->addWidget(m_label);
	}

	void AnnouncementDialog::AddWebView(QString url){

		webView = new QWebEngineView(this);
		webView->load(QUrl(url));
		layout->addWidget(webView);
	}

	void AnnouncementDialog::ReplaceLabelText(QString toReplace, QString replaceWith){

		m_label->setText(m_label->text().replace(toReplace, replaceWith));
	}

	void AnnouncementDialog::show(){

		if (firstTime){
			QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
			layout->addWidget(buttonBox);

			setLayout(layout);

			QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
			QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
			firstTime = false;
		}
		QDialog::show();

	}
} //namespace SynGlyphX
