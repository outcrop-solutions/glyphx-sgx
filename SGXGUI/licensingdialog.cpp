#include "licensingdialog.h"
#include "rlmez.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include "application.h"
#include <string>

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
		if (licenseStatus == 0) {

			if (numberOfDaysLeft == 0) {

				licenseLabel->setText(tr("License: Permanent"));
			}
			else {

				licenseLabel->setText(tr("License: Expiring in %1 days").arg(QString::number(numberOfDaysLeft - 1)));
			}
		}
		else if (licenseStatus == RLM_EH_EVAL_EXPIRED) {

			licenseLabel->setText(tr("License: Demo license expired"));
		}
		else if (licenseStatus == RLM_EH_UNLICENSED) {

			licenseLabel->setText(tr("License: No license installed"));
		}
		else {

			char error[RLM_ERRSTRING_MAX];
			rlmez_errstring(licenseStatus, error);
			licenseLabel->setText(tr("License Error: ") + error);
		}
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

		return false;
	}
}