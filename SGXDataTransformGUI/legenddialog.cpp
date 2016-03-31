#include "legenddialog.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>

namespace SynGlyphX {

	LegendDialog::LegendDialog(QWidget *parent)
		: QDialog(parent)
	{
		QFormLayout* mainLayout = new QFormLayout(this);

		m_titleLineEdit = new QLineEdit(this);
		mainLayout->addRow(tr("Name") + ":", m_titleLineEdit);

		m_legendLineEdit = new BrowseLineEdit(BrowseLineEdit::FileDialogType::FileOpen, true, this);
		m_legendLineEdit->SetFilters("PNG files (*.png)");
		mainLayout->addRow(tr("File") + ":", m_legendLineEdit);

		QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
		mainLayout->addWidget(dialogButtonBox);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &LegendDialog::accept);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &LegendDialog::reject);

		setLayout(mainLayout);
	}

	LegendDialog::~LegendDialog()
	{

	}

	void LegendDialog::accept() {

		if (m_titleLineEdit->text().isEmpty()) {

			QMessageBox::warning(this, tr("Legend Error"), tr("Legend name must not be empty."));
			return;
		}

		QString legendFilename = m_legendLineEdit->GetText();
		if (ValidateLegendFilename(legendFilename)) {

			QImage image(legendFilename);
			if (image.isNull()) {

				QMessageBox::warning(this, tr("Legend Error"), tr("File name listed for legend is an invalid image."));
				return;
			}
		}

		QDialog::accept();
	}

	bool LegendDialog::ValidateLegendFilename(const QString& legendFilename) {

		if (legendFilename.isEmpty()) {

			QMessageBox::warning(this, tr("Legend Error"), tr("File name listed for legend is empty."));
			return false;
		}
		if (legendFilename.right(4).toLower() != ".png") {

			QMessageBox::warning(this, tr("Legend Error"), tr("File name listed for legend is not a supported file type."));
			return false;
		}
		if (!QFile::exists(legendFilename)) {

			QMessageBox::warning(this, tr("Legend Error"), tr("File name listed for legend does not exist."));
			return false;
		}

		return true;
	}

	void LegendDialog::SetLegend(const Legend& legend) {

		m_titleLineEdit->setText(QString::fromStdWString(legend.GetTitle()));
		m_legendLineEdit->SetText(QString::fromStdWString(legend.GetFilename()));
	}

	Legend LegendDialog::GetLegend() const {

		Legend legend;
		legend.SetTitle(m_titleLineEdit->text().toStdWString());
		legend.SetFilename(m_legendLineEdit->GetText().toStdWString());
		return legend;
	}

} //namespace SynGlyphX