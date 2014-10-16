#include "antzimportdialog.h"
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include "groupboxsinglewidget.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>
#include "antzcsvwriter.h"

const QString ANTzCSVFileFilter = "ANTz CSV File (*.csv)";

ANTzImportDialog::ANTzImportDialog(QWidget *parent)
	: QDialog(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QGroupBox* antzGroupBox = new QGroupBox(tr("ANTz CSV Files"), this);
	QFormLayout* antzFileLayout = new QFormLayout(antzGroupBox);
	m_antzNodeFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
	m_antzNodeFileLineEdit->SetFilters(ANTzCSVFileFilter);
	antzFileLayout->addRow(tr("Node File:"), m_antzNodeFileLineEdit);
	m_antzTagFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
	m_antzTagFileLineEdit->SetFilters(ANTzCSVFileFilter);
	antzFileLayout->addRow(tr("Tag File:"), m_antzTagFileLineEdit);
	m_antzChannelFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
	m_antzChannelFileLineEdit->SetFilters(ANTzCSVFileFilter);
	antzFileLayout->addRow(tr("Channel File:"), m_antzChannelFileLineEdit);
	m_antzChannelMapFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
	m_antzChannelMapFileLineEdit->SetFilters(ANTzCSVFileFilter);
	antzFileLayout->addRow(tr("Channel Map File:"), m_antzChannelMapFileLineEdit);

	QObject::connect(m_antzNodeFileLineEdit, &SynGlyphX::BrowseLineEdit::TextChanged, this, &ANTzImportDialog::OnCSVFileChanged);

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

	m_outputFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileSave, true, transformFileGroupBox);
	m_outputFileLineEdit->SetFilters("SynGlyphX ANTz Visualization (*.sav)");
	transformFileLayout->addRow(tr("Project File:"), m_outputFileLineEdit);

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

QString ANTzImportDialog::GetANTzNodeFilename() const {

	return m_antzNodeFileLineEdit->GetText();
}

QString ANTzImportDialog::GetANTzTagFilename() const {

	return m_antzTagFileLineEdit->GetText();
}

QString ANTzImportDialog::GetANTzChannelFilename() const {

	return m_antzChannelFileLineEdit->GetText();
}

QString ANTzImportDialog::GetANTzChannelMapFilename() const {

	return m_antzChannelMapFileLineEdit->GetText();
}


QString ANTzImportDialog::GetBaseImageFilename() const {

	if (m_defaultBaseImageCheckBox->isChecked()) {

		return "";
	}
	else {

		return m_baseImageFileLineEdit->GetText();
	}
}

QString ANTzImportDialog::GetOutputFilename() const {

	return m_outputFileLineEdit->GetText();
}

void ANTzImportDialog::OnDefaultBaseImageCheckboxChanged(int newState) {

	m_baseImageFileLineEdit->setEnabled(newState != Qt::Checked);
}

void ANTzImportDialog::OnNodeFileChanged(const QString& newText) {

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

void ANTzImportDialog::accept() {

	SynGlyphX::ANTzCSVWriter& antzCSVWriter = SynGlyphX::ANTzCSVWriter::GetInstance();
	if (!ValidateANTzCSVFile(m_antzNodeFileLineEdit->GetText(), antzCSVWriter.GetNodeHeaders(), "Node file")) {

		return;
	}

	if (!ValidateANTzCSVFile(m_antzTagFileLineEdit->GetText(), antzCSVWriter.GetTagHeaders(), "Tag file", true)) {

		return;
	}

	if (m_antzChannelFileLineEdit->GetText().isEmpty() != m_antzChannelMapFileLineEdit->GetText().isEmpty()) {

		QMessageBox::warning(this, tr("Import error"), tr("Both the Channel File and the Channel Map file either must be empty or must have valid files."));
		return;
	}

	if (!ValidateANTzCSVFile(m_antzChannelFileLineEdit->GetText(), antzCSVWriter.GetChannelHeaders(), "Channel file", true)) {

		return;
	}

	if (!ValidateANTzCSVFile(m_antzChannelMapFileLineEdit->GetText(), antzCSVWriter.GetChannelMapHeaders(), "Channel Map file", true)) {

		return;
	}

	if (!m_defaultBaseImageCheckBox->isChecked()) {

		if (!ValidateBaseImageFile(m_baseImageFileLineEdit->GetText())) {

			return;
		}
	}

	if (!ValidateDataTransformFile(m_outputFileLineEdit->GetText())) {

		return;
	}

	QDialog::accept();
}

bool ANTzImportDialog::ValidateANTzCSVFile(const QString& filename, const SynGlyphX::CSVFileReader::CSVValues& headers, const QString& fieldName, bool filenameCanBeEmpty) {

	if (filename.isEmpty()) {

		if (filenameCanBeEmpty) {

			return true;
		}
		else {

			QMessageBox::warning(this, tr("Import error"), fieldName + tr(" field can't be empty."));
			return false;
		}
	}

	if (filename.right(4).toLower() == ".csv") {

		QMessageBox::warning(this, tr("Import error"), fieldName + tr(" field does not have a csv file."));
		return false;
	}

	if (!QFileInfo::exists(filename)) {

		QMessageBox::warning(this, tr("Import error"), fieldName + tr(" field does not have a csv that exists."));
		return false;
	}

	try {

		SynGlyphX::CSVFileReader csvFileReader(filename.toStdString());
		if (csvFileReader.GetHeaders() != headers) {

			QMessageBox::warning(this, tr("Import error"), tr("Headers do not match for ") + fieldName);
			return false;
		}

	}
	catch (const std::exception& e) {

		QMessageBox::warning(this, tr("Import error"), fieldName + tr(" error: ") + e.what());
		return false;
	}


	return true;
}

bool ANTzImportDialog::ValidateBaseImageFile(const QString& filename) {

	if (filename.isEmpty()) {

		QMessageBox::warning(this, tr("Import error"), tr("Base image filename field can't be empty unless Use Default Image is checked."));
		return false;
	}

	if (filename.right(4).toLower() == ".jpg") {

		QMessageBox::warning(this, tr("Import error"), tr("Base image filename does not contain a valid base image."));
		return false;
	}

	if (!QFileInfo::exists(filename)) {

		QMessageBox::warning(this, tr("Import error"), tr("Base image file does not exist."));
		return false;
	}

	return true;
}

bool ANTzImportDialog::ValidateDataTransformFile(const QString& filename) {

	if (filename.isEmpty()) {

		QMessageBox::warning(this, tr("Import error"), tr("Data Transform filename field can't be empty."));
		return false;
	}

	if (filename.right(4).toLower() == ".sav") {

		QMessageBox::warning(this, tr("Import error"), tr("Project filename must have an sav extension."));
		return false;
	}

	if (QFileInfo::exists(filename)) {

		if (QMessageBox::question(this, tr("Data transform file exists"), tr("The data transform file listed already exists.  Do you want to overwrite it?")) == QMessageBox::No) {
		
			return false;
		}
	}

	return true;
}