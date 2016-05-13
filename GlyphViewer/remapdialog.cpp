#include "remapdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include "groupboxsinglewidget.h"

RemapDialog::RemapDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Remap Root Position Bindings"));

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	m_saveFilenameEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileSave, this);
	m_saveFilenameEdit->setContentsMargins(4, 4, 4, 4);
	SynGlyphX::GroupBoxSingleWidget* saveFilenameGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Filename"), m_saveFilenameEdit, this);
	mainLayout->addWidget(saveFilenameGroupBox);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);

	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &RemapDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &RemapDialog::reject);

	setLayout(mainLayout);
}

RemapDialog::~RemapDialog()
{

}

void RemapDialog::SetSaveFilename(const QString& saveFilename) {

	m_saveFilenameEdit->SetText(QDir::toNativeSeparators(saveFilename));
}

QString RemapDialog::GetSaveFilename() const {

	return QDir::toNativeSeparators(m_saveFilenameEdit->GetText());
}

void RemapDialog::accept() {

	QString saveFilename = m_saveFilenameEdit->GetText();
	if (saveFilename.isEmpty()) {

		QMessageBox::warning(this, tr("Position Remapping"), tr("The remapped visualization must be given a filename that isn't empty so that it can be saved."));
		return;
	}

	if (QFileInfo::exists(saveFilename)) {

		if (QMessageBox::question(this, tr("Position Remapping"), tr("The filename already exists.  Do you wish to overwrite it?")) == QMessageBox::No) {

			return;
		}
	}

	QDialog::accept();
}