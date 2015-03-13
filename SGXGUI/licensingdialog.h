#ifndef SYNGLYPHX_LICENSINGDIALOG_H
#define SYNGLYPHX_LICENSINGDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>

namespace SynGlyphX {

	class LicensingDialog : public QDialog
	{
		Q_OBJECT

	public:
		LicensingDialog(QWidget *parent);
		~LicensingDialog();

		static bool CheckLicense();

	private slots:
		void OnInstallNewLicense();

	private:
		static QString LicenseStatusToString(int licenseStatus, int numberOfDaysLeft);
		void ResetStatusLabel();

		QLabel* m_licenseLabel;
	};

} //namespace SynGlyphX

#endif // LICENSINGDIALOG_H
