#include "replacefilenamedialog.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <boost/filesystem.hpp>
#include "browselineedit.h"

namespace SynGlyphX {

	ReplaceFilenameDialog::ReplaceFilenameDialog(const QString& oldFileName, const QString& acceptButtonText, QWidget *parent)
		: QDialog(parent)
	{
		QFormLayout* layout = new QFormLayout(this);

		QLabel* oldDatasourceLabel = new QLabel(oldFileName, this);
		layout->addRow(tr("Old:"), oldDatasourceLabel);
		m_newDatasourceFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
		m_newDatasourceFileLineEdit->SetInitalBrowseDirectory(oldFileName);
		m_newDatasourceFileLineEdit->SetText(oldFileName);
		layout->addRow(tr("New:"), m_newDatasourceFileLineEdit);

		QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
		layout->addWidget(dialogButtonBox);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ReplaceFilenameDialog::accept);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ReplaceFilenameDialog::reject);

		setLayout(layout);
	}

	ReplaceFilenameDialog::~ReplaceFilenameDialog()
	{

	}

	QString ReplaceFilenameDialog::GetNewFilename() const {

		return m_newDatasourceFileLineEdit->GetText();
	}

	void ReplaceFilenameDialog::accept() {

		if (QFile::exists(m_newDatasourceFileLineEdit->GetText())) {

			if (IsNewFileValid()) {

				QDialog::accept();
			}
			else {

				QMessageBox::warning(this, tr("Change Filename"), tr("New file is not a valid replacement for old file."));
			}
		}
		else {

			QMessageBox::warning(this, tr("Change Filename"), tr("New file does not exist so it is not a valid replacement for old file."));
		}
	}

} //namespace SynGlyphX
