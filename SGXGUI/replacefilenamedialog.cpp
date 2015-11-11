#include "replacefilenamedialog.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>

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

		QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
		QPushButton* nextOkButton = dialogButtonBox->addButton(acceptButtonText, QDialogButtonBox::ButtonRole::AcceptRole);
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

		if (IsNewFileValid()) {

			QDialog::accept();
		}
		else {

			QMessageBox::warning(this, tr("Change Filename"), tr("New file is not a valid replacement for old file."));
		}
	}

	bool ReplaceFilenameDialog::IsNewFileValid() const {

		return false;
	}

} //namespace SynGlyphX