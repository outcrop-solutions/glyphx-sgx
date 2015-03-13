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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtGui/QTextDocument>

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

		QTextEdit* licenseAgreementBrowser = new QTextEdit(this);
		licenseAgreementBrowser->setReadOnly(true);
		QTextDocument* licenseAgreement = new QTextDocument(licenseAgreementBrowser);

#ifdef WIN32
		licenseAgreementBrowser->document()->setMetaInformation(QTextDocument::DocumentUrl, QDir::fromNativeSeparators(QDir::currentPath()) + "/license_agreement.html/");
#else
		licenseAgreement->setMetaInformation(QTextDocument::DocumentUrl, "file:" + QDir::fromNativeSeparators(QDir::currentPath()) + "/license_agreement.html/");
#endif
		//licenseAgreementBrowser->setDocument(licenseAgreement);
		//QString licenseHTML = licenseAgreement->toHtml();
		//licenseAgreementBrowser->setHtml(licenseHTML);

		SynGlyphX::GroupBoxSingleWidget* licenseAgreementGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("License Agreement"), licenseAgreementBrowser, this);
		layout->addWidget(licenseAgreementGroupBox);

		layout->addStretch(1);

		QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

		QPushButton* installNewLicenseButton = new QPushButton(tr("Install New License File"), this);
		QObject::connect(installNewLicenseButton, &QPushButton::clicked, this, &LicensingDialog::OnInstallNewLicense);
		buttonsLayout->addWidget(installNewLicenseButton);

		QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok, this);
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

		int numberOfDaysLeft = 0;
		char version[8];
		strcpy(version, SynGlyphX::Application::GetApplicationVersionMajorNumber().toStdString().c_str());
		int licenseStatus = rlmez_checkout(version, &numberOfDaysLeft);

		if (licenseStatus == 0) {

			return true;
		}
		else {

			QMessageBox::information(nullptr, tr("Licensing"), tr("License Status: ") + LicenseStatusToString(licenseStatus, numberOfDaysLeft) + 
				"\n\n" + tr("A valid license is needed to use SynGlyphX software.  Please go to http://www.synglyphx.com to contact SynGlyphX for a license"));

			LicensingDialog dialog(nullptr);
			if (dialog.exec() == QDialog::Accepted) {

				return (rlmez_checkout(version, &numberOfDaysLeft) == 0);
			}
		}

		return false;
	}

	QString LicensingDialog::LicenseStatusToString(int licenseStatus, int numberOfDaysLeft) {

		if (licenseStatus == 0) {

			if (numberOfDaysLeft == 0) {

				 return tr("Permanent");
			}
			else {

				return tr("Expiring in %1 days").arg(QString::number(numberOfDaysLeft - 1));
			}
		}
		else if (licenseStatus == RLM_EL_EXPIRED) {

			return tr("Expired");
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
			return (tr("Error: ") + error);
		}
	}

	void LicensingDialog::ResetStatusLabel() {

		int numberOfDaysLeft = 0;
		char version[8];
		strcpy(version, SynGlyphX::Application::GetApplicationVersionMajorNumber().toStdString().c_str());
		int licenseStatus = rlmez_checkout(version, &numberOfDaysLeft);
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
			QFile::copy(filename, QDir::currentPath() + QDir::separator() + "license.lic");
			ResetStatusLabel();
		}

		settings.endGroup();
	}
}