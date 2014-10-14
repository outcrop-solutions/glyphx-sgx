#include "antzimportdialog.h"
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include "groupboxsinglewidget.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

const QString ANTzCSVFileFilter = "ANTz CSV File (*.csv)";

ANTzImportDialog::ANTzImportDialog(QWidget *parent)
	: QDialog(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QGroupBox* antzGroupBox = new QGroupBox(tr("ANTz CSV Files"), this);
	QFormLayout* antzFileLayout = new QFormLayout(antzGroupBox);
	m_antzCSVFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
	m_antzCSVFileLineEdit->SetFilters(ANTzCSVFileFilter);
	antzFileLayout->addRow(tr("CSV File:"), m_antzCSVFileLineEdit);
	m_antzTagFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
	m_antzTagFileLineEdit->SetFilters(ANTzCSVFileFilter);
	antzFileLayout->addRow(tr("Tag File:"), m_antzTagFileLineEdit);
	m_antzChannelFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
	m_antzChannelFileLineEdit->SetFilters(ANTzCSVFileFilter);
	antzFileLayout->addRow(tr("Channel File:"), m_antzChannelFileLineEdit);

	QObject::connect(m_antzCSVFileLineEdit, &SynGlyphX::BrowseLineEdit::TextChanged, this, &ANTzImportDialog::OnCSVFileChanged);

	antzGroupBox->setLayout(antzFileLayout);

	mainLayout->addWidget(antzGroupBox);

	QGroupBox* baseImageGroupBox = new QGroupBox(tr("Base Image"), this);
	QFormLayout* baseImageLayout = new QFormLayout(baseImageGroupBox);

	m_defaultBaseImageCheckBox = new QCheckBox(this);
	m_defaultBaseImageCheckBox->setChecked(true);
	baseImageLayout->addRow(tr("Use Default Image"), m_defaultBaseImageCheckBox);

	m_baseImageFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
	m_baseImageFileLineEdit->SetFilters("jpeg (*.jpg)");
	m_baseImageFileLineEdit->setEnabled(false);
	baseImageLayout->addRow(tr("File name:"), m_baseImageFileLineEdit);

	QObject::connect(m_defaultBaseImageCheckBox, &QCheckBox::stateChanged, this, &ANTzImportDialog::OnDefaultBaseImageCheckboxChanged);

	baseImageGroupBox->setLayout(baseImageLayout);

	mainLayout->addWidget(baseImageGroupBox);

	QGroupBox* transformFileGroupBox = new QGroupBox(tr("Output"), this);
	QFormLayout* transformFileLayout = new QFormLayout(transformFileGroupBox);

	m_dataTransformFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileSave, true, transformFileGroupBox);
	m_dataTransformFileLineEdit->SetFilters("SynGlyphX Data Transform (*.sdt)");
	transformFileLayout->addRow(tr("Data Transform File"), m_dataTransformFileLineEdit);

	transformFileGroupBox->setLayout(transformFileLayout);
	
	mainLayout->addWidget(transformFileGroupBox);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, this);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	mainLayout->addWidget(dialogButtonBox);

	setLayout(mainLayout);
}

ANTzImportDialog::~ANTzImportDialog()
{

}

QString ANTzImportDialog::GetANTzCSVFilename() const {

	return m_antzCSVFileLineEdit->GetText();
}

QString ANTzImportDialog::GetANTzTagFilename() const {

	return m_antzTagFileLineEdit->GetText();
}

QString ANTzImportDialog::GetANTzChannelFilename() const {

	return m_antzChannelFileLineEdit->GetText();
}

QString ANTzImportDialog::GetBaseImageFilename() const {

	if (m_defaultBaseImageCheckBox->isChecked()) {

		return "";
	}
	else {

		return m_baseImageFileLineEdit->GetText();
	}
}

QString ANTzImportDialog::GetDataTransformFilename() const {

	return m_dataTransformFileLineEdit->GetText();
}

void ANTzImportDialog::OnDefaultBaseImageCheckboxChanged(int newState) {

	m_baseImageFileLineEdit->setEnabled(newState != Qt::Checked);
}

void ANTzImportDialog::OnCSVFileChanged(const QString& newText) {

	//This code is designed to facilitate getting all the ANTz csv filenames.  When a filename of antz0001.csv is selected there will be a tag and a channel filename of 
	//antztag0001.csv and antzch0001.csv.  Since we know that is likely, test if those files exists.  If they do then fill out the dialog box so that the user doesn't
	//have to search for them

	QFileInfo fileInfo(newText);
	QString directory = fileInfo.canonicalPath();
	if (!directory.isEmpty()) {

		QString baseName = fileInfo.completeBaseName();
		bool isNumber = false;
		QString baseNameLast4 = baseName.right(4);
		baseNameLast4.toUInt(&isNumber);

		if (isNumber) {

			QString baseNameMinusLast4 = baseName.left(baseName.length() - 4);
			QString tagFile = directory + QDir::separator() + baseNameMinusLast4 + "tag" + baseNameLast4 + "." + fileInfo.suffix();
			QString channelFile = directory + QDir::separator() + baseNameMinusLast4 + "ch" + baseNameLast4 + "." + fileInfo.suffix();

			if (QFileInfo::exists(tagFile)) {

				m_antzTagFileLineEdit->SetText(tagFile);
			}

			if (QFileInfo::exists(channelFile)) {

				m_antzChannelFileLineEdit->SetText(channelFile);
			}
		}
	}
}