#include "licensingdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include "application.h"
#include <string>
#include <QtWidgets/QMessageBox>
#include "groupboxsinglewidget.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtGui/QTextDocument>
#include <QtCore/QTextStream>
#include <QtCore/QDateTime>
#include <QtCore/QCryptographicHash>
#include "filesystem.h"
#include <QtWidgets/QLabel>
#include <stdio.h>
#include <sstream>

#ifdef USE_LICENSING

//#include "rlmez.h"

namespace SynGlyphX {

	LicensingDialog::LicensingDialog(QWidget *parent)
		: QDialog(parent)
	{
		setWindowTitle(tr("Licensing"));

		QVBoxLayout* layout = new QVBoxLayout(this);
		m_licenseLabel = new QLabel(this);
		SynGlyphX::GroupBoxSingleWidget* licenseLabelGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Status:"), m_licenseLabel, this);

		ResetStatusLabel();

		layout->addWidget(licenseLabelGroupBox);

		QTextEdit* eulaBrowser = new QTextEdit(this);
		eulaBrowser->setReadOnly(true);
		eulaBrowser->setMinimumSize(700, 300);
		QString eulaPath;
		if (QFileInfo(QCoreApplication::applicationFilePath()).fileName().contains("GlyphEd")){
			eulaPath = ":SGXGUI/Resources/license_agreement_glyphed.html";
		}
		else{
			eulaPath = ":SGXGUI/Resources/license_agreement.html";
		}
		QFile eulaFile(eulaPath);
		eulaFile.open(QFile::ReadOnly | QFile::Text);
		QTextStream eulaStream(&eulaFile);
		eulaBrowser->setHtml(eulaStream.readAll());

		SynGlyphX::GroupBoxSingleWidget* eulaGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("License Agreement"), eulaBrowser, this);
		layout->addWidget(eulaGroupBox, 1);

		QHBoxLayout* buttonsLayout = new QHBoxLayout(this);
		/*
		QPushButton* installNewLicenseButton = new QPushButton(tr("Install New License File"), this);
		QObject::connect(installNewLicenseButton, &QPushButton::clicked, this, &LicensingDialog::OnInstallNewLicense);
		buttonsLayout->addWidget(installNewLicenseButton);
		*/
		QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Close, this);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
		buttonsLayout->addWidget(dialogButtonBox);

		layout->addLayout(buttonsLayout);

		setLayout(layout);
	}

	LicensingDialog::~LicensingDialog()
	{

	}

	bool LicensingDialog::CheckLicense() {

		QString previousCurrentDir = QDir::currentPath();
		QDir::setCurrent(GetLicenseDirectory());

		bool result = false;
		int numberOfDaysLeft = 0;
		char version[8];
		strcpy(version, SynGlyphX::Application::GetApplicationVersionMajorNumber().toStdString().c_str());
		//int licenseStatus = rlmez_checkout(const_cast<char*>("synglyphx"), version, &numberOfDaysLeft);

		int licenseStatus = SGXLicenseCheckout(const_cast<char*>("synglyphx"), version, numberOfDaysLeft);

		//return 0 if valid
		//return 1 if expired
		//return 2 if not logged in
		//return 3 if no license
		//return 4 if license does not support current product
		//return 5 if invalid

		if (licenseStatus == 0) {

			result = true;
		}
		else {

			QMessageBox::information(nullptr, tr("Licensing"), tr("License Status: ") + LicenseStatusToString(licenseStatus, numberOfDaysLeft) + 
				"\n\n" + tr("A valid license is needed to use SynGlyphX software.  Please contact SynGlyphX (http://www.synglyphx.com) for a license."));
			/*
			LicensingDialog dialog(nullptr);
			if (dialog.exec() == QDialog::Accepted) {
				if (rlmez_checkout(const_cast<char*>("synglyphx"), version, &numberOfDaysLeft) == 0) {
					result = true;
				}
				else {

					QMessageBox::information(nullptr, tr("Licensing"), tr("License is invalid or expired.  Please obtain a valid license."));
				}
				result = false;
			}
			*/
		}

		QDir::setCurrent(previousCurrentDir);
		return result;
	}

	QString LicensingDialog::LicenseStatusToString(int licenseStatus, int numberOfDaysLeft) {

		if (licenseStatus == 0) {

			if (numberOfDaysLeft == 0) {

				return tr("Permanent license");
			}
			else if (numberOfDaysLeft == 1) {

				return tr("License expires today");
			}
			else {

				return tr("License expiring in %1 days").arg(QString::number(numberOfDaysLeft));
			}
		}
		else if (licenseStatus == 1) {

			return tr("Expired license");
		}
		else if (licenseStatus == 2) {

			return tr("No user logged in");
		}
		else if (licenseStatus == 3){ //|| (licenseStatus == RLM_EH_UNLICENSED)) {

			return tr("No license installed");
		}
		else if (licenseStatus == 4) {

			return tr("License does not support current product");
		}
		else {

			//char error[RLM_ERRSTRING_MAX];
			//rlmez_errstring(licenseStatus, error);
			return (tr("Error with licensing: Invalid license")); //+ error);
		}
	}

	void LicensingDialog::ResetStatusLabel() {

		QString previousCurrentDir = QDir::currentPath();
		QDir::setCurrent(GetLicenseDirectory());

		int numberOfDaysLeft = 0;
		char version[8];
		strcpy(version, SynGlyphX::Application::GetApplicationVersionMajorNumber().toStdString().c_str());
		//int licenseStatus = rlmez_checkout(const_cast<char*>("synglyphx"), version, &numberOfDaysLeft);
		int licenseStatus = SGXLicenseCheckout(const_cast<char*>("synglyphx"), version, numberOfDaysLeft);

		QDir::setCurrent(previousCurrentDir);

		m_licenseLabel->setText(LicenseStatusToString(licenseStatus, numberOfDaysLeft));
	}

	void LicensingDialog::OnInstallNewLicense() {

		QSettings settings;
		settings.beginGroup("Licensing");
		QString initialDir = settings.value("LicenseDir", QDir::currentPath()).toString();

		QString filename = QFileDialog::getOpenFileName(this, tr("Select License File"), initialDir, "License Files (*.lic)");
		if (!filename.isEmpty()) {

			QFileInfo fileInfo(filename);
			settings.setValue("LicenseDir", fileInfo.absolutePath());
			try {

				Filesystem::CopyFileOverwrite(filename.toStdString(), QDir::toNativeSeparators(GetLicenseDirectory() + QDir::separator() + "license.lic").toStdString());
			}
			catch (const std::exception& e) {

				QMessageBox::critical(this, tr("Installing License File Failed"), tr("Installing License File Failed: ") + e.what());
				return;
			}
			ResetStatusLabel();
		}

		settings.endGroup();

		if (parentWidget() == nullptr) {

			accept();
		}
		else {

			ResetStatusLabel();
		}
	}

	int LicensingDialog::SGXLicenseCheckout(char *product, char *ver, int &days) {

		QSettings liu_settings;
		liu_settings.beginGroup("LoggedInUser");
		QString user = liu_settings.value("Username", "Guest").toString();
		liu_settings.endGroup();
		
		QSettings settings;
		settings.beginGroup(user);
		int id = settings.value("UserID", 0).toInt();
		int type = settings.value("LicenseType", 0).toInt();
		settings.endGroup();
		//int id = 4;
		//int type = 1;

		QStringList allowed;
		switch (type){
			case 1:
				allowed = (QStringList() << "GlyphEd" << "GlyphViewer" << "DataMapper" << "GlyphDesigner");
				break;
			case 2:
				allowed = (QStringList() << "GlyphViewer");
				break;
			case 3:
				allowed = (QStringList() << "GlyphEd");
				break;
			default:
				return 3;
		}

		std::ifstream input("SGXLicense");
		if (input.is_open()){

			std::string lic_key;
			getline(input, lic_key);
			QString key = QString::fromStdString(lic_key);
			QStringList keyList = key.split("-");
			QString license_key = GenerateLicenseKeyString(QString::number(id), QString::number(type), keyList.at(4));

			auto appfile = QFileInfo(QCoreApplication::applicationFilePath()).fileName().split(".").at(0);
			if (!allowed.contains(appfile)){
				return 4; //license does not support current product
			}
			if (key != license_key){
				return 5; //invalid
			}

			bool ok;
			int end = keyList.at(4).toLongLong(&ok, 16) / 86400;
			int current = (QDateTime::currentDateTime().currentMSecsSinceEpoch() / 1000) / 86400;
			days = end - current;

			if (days <= 0){
				return 1; //expired license
			}
			return 0; //valid license
		}
		return 3; //no license

	}

	QString LicensingDialog::GenerateLicenseKeyString(QString userId, QString type, QString timestamp) {
	
		bool ok;
		timestamp = QString::number(timestamp.toLongLong(&ok, 16));
		QString sum = userId + type + timestamp;
		QString sum_hash = QCryptographicHash::hash((sum.toStdString().c_str()), QCryptographicHash::Md5).toHex();
		QStringList seq;
		seq << sum_hash.left(8);
		seq << sum_hash.mid(8, 8);
		seq << sum_hash.mid(16, 8);
		seq << sum_hash.right(8);
		seq << QString::fromStdString(DecimalToHex(timestamp.toLongLong()));

		return seq.at(0) + "-" + seq.at(1) + "-" + seq.at(2) + "-" + seq.at(3) + "-" + seq.at(4);
	}

	std::string LicensingDialog::DecimalToHex(long long to_convert) {

		//char buffer[33];
		//itoa(to_convert, buffer, 16);
		std::stringstream ss;
		ss << std::hex << to_convert;
		//return std::string(buffer);
		return ss.str();
	}

	QString LicensingDialog::GetLicenseDirectory() {

		return SynGlyphX::Application::GetCommonDataLocation();
	}
}


#endif
