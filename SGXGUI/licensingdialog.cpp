#include "licensingdialog.h"
#include "rlmez.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include "application.h"
#include <string>
#include <QtWidgets/QMessageBox>

namespace SynGlyphX {

	LicensingDialog::LicensingDialog(QWidget *parent)
		: QDialog(parent)
	{
		setWindowTitle(tr("Licensing"));

		QVBoxLayout* layout = new QVBoxLayout(this);
		QLabel* licenseLabel = new QLabel(this);

		int numberOfDaysLeft = 0;
		char version[8];
		strcpy(version, SynGlyphX::Application::GetApplicationVersionMajorNumber().toStdString().c_str());
		int licenseStatus = rlmez_checkout(version, &numberOfDaysLeft);
		licenseLabel->setText(tr("License Status: ") + LicenseStatusToString(licenseStatus, numberOfDaysLeft));

		layout->addWidget(licenseLabel);

		layout->addStretch(1);

		QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok, this);
		layout->addWidget(dialogButtonBox);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

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
		else if (licenseStatus == RLM_EH_UNLICENSED) {

			return tr("No license installed");
		}
		else {

			char error[RLM_ERRSTRING_MAX];
			rlmez_errstring(licenseStatus, error);
			return (tr("Error: ") + error);
		}
	}
}