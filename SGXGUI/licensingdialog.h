#ifndef SYNGLYPHX_LICENSINGDIALOG_H
#define SYNGLYPHX_LICENSINGDIALOG_H

#include <QtWidgets/QDialog>

namespace SynGlyphX {

	class LicensingDialog : public QDialog
	{
		Q_OBJECT

	public:
		LicensingDialog(QWidget *parent);
		~LicensingDialog();

		static bool CheckLicense();

	private:
		static QString LicenseStatusToString(int licenseStatus, int numberOfDaysLeft);
	};

} //namespace SynGlyphX

#endif // LICENSINGDIALOG_H
