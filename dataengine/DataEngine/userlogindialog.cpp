#include "userlogindialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtCore/QSettings>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtWidgets/QApplication>

namespace DataEngine {

	UserLoginDialog::UserLoginDialog(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent)
		: QFrame(parent),
		m_dataEngineConnection(dataEngineConnection)
	{

		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setDirection(QBoxLayout::TopToBottom);

		QLabel* welcomeLabel = new QLabel(tr("Enter your Username and Password"), this);
		welcomeLabel->setAlignment(Qt::AlignCenter);
		welcomeLabel->setWordWrap(true);
		welcomeLabel->setTextFormat(Qt::RichText);
		welcomeLabel->setStyleSheet("QLabel{font-size: 12.5pt; font-weight: bold; background-color: white;}");
		//layout->addWidget(welcomeLabel, 0, Qt::AlignTop);

		m_usernameLineEdit = new QLineEdit(this);
		m_passwordLineEdit = new QLineEdit(this);
		m_passwordLineEdit->setEchoMode(QLineEdit::Password);
		QString line(" QLineEdit { "
			" border:4px outset; "
			" border-radius: 8px; "
			" color:rgb(0, 0, 0); "
			" background-color: rgb(255, 255, 255); "
			" border-color: gray; "
			" font-size: 11pt; "
			" margin-left: 50px; "
			" margin-right: 50px; "
			" min-height: 40px; } ");
		m_usernameLineEdit->setStyleSheet(line);
		m_passwordLineEdit->setStyleSheet(line);/*
		stayLoggedInCheckBox = new QCheckBox("Keep me logged in", this);
		stayLoggedInCheckBox->setLayoutDirection(Qt::RightToLeft);
		stayLoggedInCheckBox->setStyleSheet("font-size:11pt;margin-right:50px;");*/
		QFormLayout* formLayout = new QFormLayout(this);
		formLayout->setContentsMargins(-1, -1, -1, 0);
		formLayout->setFormAlignment(Qt::AlignCenter);
		QLabel* username = new QLabel(tr("Username: "), this);
		QLabel* password = new QLabel(tr("Password: "), this);
		username->setStyleSheet("font-size:11pt;margin-left: 50px;");
		password->setStyleSheet("font-size:11pt;margin-left: 50px;");
		formLayout->addRow(welcomeLabel);
		formLayout->addRow(username);
		formLayout->addRow(m_usernameLineEdit);
		formLayout->addRow(password);
		formLayout->addRow(m_passwordLineEdit);

		QLabel* chgpw = new QLabel(tr("<a href='http://sample-env.zccmpffgqr.us-west-2.elasticbeanstalk.com/password.jsp'>Change Password</a>"), this);
		chgpw->setOpenExternalLinks(true);
		chgpw->setLayoutDirection(Qt::LeftToRight);
		chgpw->setStyleSheet("font-size:11pt;margin-left: 50px;");

		QHBoxLayout* hboxlayout = new QHBoxLayout;
		hboxlayout->addWidget(chgpw);
		//hboxlayout->addWidget(stayLoggedInCheckBox);

		formLayout->addRow(hboxlayout);

		QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

		loginButton = new QPushButton(tr("Log In"), this);
		loginButton->setMinimumWidth(80);
		QString style("QPushButton{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #a5d9eb, stop: 1 #e8f3eb);border-radius: 8px;border-top: 0.5px solid #fff;border-right: 1px outset #fff;border-bottom: 1px solid gray;border-left: 0.5px solid gray;padding: 5px;font-size: 14pt;}");
		QString hover("QPushButton:hover{background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 #1ec1fd, stop: 1 #b3e8fb);}");
		loginButton->setStyleSheet(style + hover);
		buttonsLayout->addWidget(loginButton, 0, Qt::AlignCenter);

		formLayout->addRow(buttonsLayout);
		layout->addLayout(formLayout);

		setLayout(layout);

		QObject::connect(loginButton, &QPushButton::clicked, this, &UserLoginDialog::LoginActivated);
		auto appfile = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
		if (appfile.contains("GlyphEd") || appfile.contains("GlyphViewer")){
			QObject::connect(m_usernameLineEdit, &QLineEdit::returnPressed, this, &UserLoginDialog::LoginActivated);
			QObject::connect(m_passwordLineEdit, &QLineEdit::returnPressed, this, &UserLoginDialog::LoginActivated);
		}
	}


	UserLoginDialog::~UserLoginDialog()
	{
	}

	bool UserLoginDialog::Login() {

		QSettings l_settings;
		l_settings.beginGroup("LoggedInUser");
		bool stayLoggedIn = l_settings.value("StayLogged", false).toBool();
		QString user = stayLoggedIn ? l_settings.value("Username", "Guest").toString() : m_usernameLineEdit->text();
		QString pass = stayLoggedIn ? l_settings.value("Password", "").toString() : m_passwordLineEdit->text();
		QString name;
		QString inst;
		bool canLogIn = false;
		int valid = m_dataEngineConnection->UserAccessControls()->ValidateCredentials(user, pass);
		if (valid == 2){
			QSettings settings;
			settings.beginGroup(user);
			QString un = settings.value("Username", "Guest").toString();
			QString pw = settings.value("Password", "").toString();
			QString nm = settings.value("Name", "Guest").toString();
			QString in = settings.value("Institution", "").toString();
			QString id = settings.value("UserID", "0").toString();
			QStringList groups = settings.value("GroupNames", "").toStringList();
			settings.endGroup();
			if (user == un && pass == pw){
				m_dataEngineConnection->UserAccessControls()->SetUsersInformation(id, nm, in);
				m_dataEngineConnection->UserAccessControls()->SetVisualizationGroupNames(groups);
				canLogIn = true;
			}
		}

		if (valid == 1 || (valid == 2 && canLogIn)){
			name = m_dataEngineConnection->UserAccessControls()->NameOfUser();
			inst = m_dataEngineConnection->UserAccessControls()->NameOfInstitution();
			/*if (stayLoggedInCheckBox->checkState() == Qt::Checked){
				stayLoggedIn = true;
			}*/
			l_settings.setValue("Username", user);
			l_settings.setValue("Password", pass);
			l_settings.setValue("Name", name);
			l_settings.setValue("Institution", inst);
			l_settings.setValue("StayLogged", true);
		}
		if (valid == 1) {
			m_dataEngineConnection->UserAccessControls()->GenerateLicenseKey(m_dataEngineConnection->GetGlyphEdPath().remove("Content"));
			QSettings settings;
			settings.beginGroup(user);
			settings.setValue("Username", user);
			settings.setValue("Password", pass);
			settings.setValue("Name", name);
			settings.setValue("Institution", inst);
			settings.setValue("UserID", m_dataEngineConnection->UserAccessControls()->GetUserID());
			settings.setValue("LicenseType", m_dataEngineConnection->UserAccessControls()->GetLicenseType());
			settings.setValue("GroupNames", m_dataEngineConnection->UserAccessControls()->GetFormattedGroupNames());
			settings.endGroup();
			canLogIn = true;
		}
		l_settings.endGroup();
		
		return canLogIn;
	}

	void UserLoginDialog::Clear() {

		m_usernameLineEdit->setText("");
		m_passwordLineEdit->setText("");
	}

}