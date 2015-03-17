#include "licensingdialog.h"
#include "rlmez.h"
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
#include "filesystem.h"

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

		QFile eulaFile(QDir::fromNativeSeparators(SynGlyphX::Application::applicationDirPath() + "/docs/eula.html"));
		eulaFile.open(QFile::ReadOnly | QFile::Text);
		QTextStream eulaStream(&eulaFile);
		eulaBrowser->setHtml(eulaStream.readAll());

		SynGlyphX::GroupBoxSingleWidget* eulaGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("EULA (End User License Agreement)"), eulaBrowser, this);
		layout->addWidget(eulaGroupBox, 1);

		QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

		QPushButton* installNewLicenseButton = new QPushButton(tr("Install New License File"), this);
		QObject::connect(installNewLicenseButton, &QPushButton::clicked, this, &LicensingDialog::OnInstallNewLicense);
		buttonsLayout->addWidget(installNewLicenseButton);

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
		int licenseStatus = rlmez_checkout(version, &numberOfDaysLeft);

		if (licenseStatus == 0) {

			result = true;
		}
		else {

			QMessageBox::information(nullptr, tr("Licensing"), tr("License Status: ") + LicenseStatusToString(licenseStatus, numberOfDaysLeft) + 
				"\n\n" + tr("A valid license is needed to use SynGlyphX software.  Please install your license using the next dialog, or contact SynGlyphX (http://www.synglyphx.com) for a license."));

			LicensingDialog dialog(nullptr);
			if (dialog.exec() == QDialog::Accepted) {

				if (rlmez_checkout(version, &numberOfDaysLeft) == 0) {

					result = true;
				}
				else {

					QMessageBox::information(nullptr, tr("Licensing"), tr("License is invalid or expired.  Please obtain a valid license."));
				}
			}
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

				return tr("License expiring in %1 days").arg(QString::number(numberOfDaysLeft - 1));
			}
		}
		else if (licenseStatus == RLM_EL_EXPIRED) {

			return tr("Expired license");
		}
		else if (licenseStatus == RLM_EH_EVAL_EXPIRED) {

			return tr("Demo license expired");
		}
		else if ((licenseStatus == RLM_ACT_BP_NOLIC) || (licenseStatus == RLM_EH_UNLICENSED)) {

			return tr("No license installed");
		}
		else {

			char error[RLM_ERRSTRING_MAX];
			rlmez_errstring(licenseStatus, error);
			return (tr("Error with licensing: ") + error);
		}
	}

	void LicensingDialog::ResetStatusLabel() {

		QString previousCurrentDir = QDir::currentPath();
		QDir::setCurrent(GetLicenseDirectory());

		int numberOfDaysLeft = 0;
		char version[8];
		strcpy(version, SynGlyphX::Application::GetApplicationVersionMajorNumber().toStdString().c_str());
		int licenseStatus = rlmez_checkout(version, &numberOfDaysLeft);

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

	QString LicensingDialog::GetLicenseDirectory() {

		return SynGlyphX::Application::GetCommonDataLocation();
	}
}