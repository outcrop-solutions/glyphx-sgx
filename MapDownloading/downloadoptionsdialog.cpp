#include "downloadoptionsdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include "networkdownloader.h"
#include "notemptyvalidator.h"

DownloadOptionsDialog::DownloadOptionsDialog(QWidget *parent)
    : QDialog(parent)
{
    setMinimumWidth(500);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_mapquestKeyLineEdit = new QLineEdit(this);
	SynGlyphX::NotEmptyValidator* mapQuestValidator = new SynGlyphX::NotEmptyValidator(false, m_mapquestKeyLineEdit);
	m_mapquestKeyLineEdit->setValidator(mapQuestValidator);

    m_googleMapsKeyLineEdit = new QLineEdit(this);
	SynGlyphX::NotEmptyValidator* googleMapsValidator = new SynGlyphX::NotEmptyValidator(false, m_googleMapsKeyLineEdit);
	m_googleMapsKeyLineEdit->setValidator(googleMapsValidator);
    m_googleMapsKeyLineEdit->setEnabled(false);

    QFormLayout* formLayout = new QFormLayout(this);
    formLayout->addRow("MapQuest Open Key:", m_mapquestKeyLineEdit);
    formLayout->addRow("Google Maps Key:", m_googleMapsKeyLineEdit);

    layout->addLayout(formLayout);

    QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QObject::connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(dialogButtonBox);

    setLayout(layout);

    setWindowTitle(tr("Map Download Settings"));

    NetworkDownloader& networkDownloader = NetworkDownloader::Instance();
    m_mapquestKeyLineEdit->setText(networkDownloader.GetMapQuestOpenKey());
}

DownloadOptionsDialog::~DownloadOptionsDialog()
{

}

void DownloadOptionsDialog::accept() {

    NetworkDownloader& networkDownloader = NetworkDownloader::Instance();

    if (m_mapquestKeyLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("MapQuest Open must have a key.  The field can't be empty."));
        m_mapquestKeyLineEdit->setText(networkDownloader.GetMapQuestOpenKey());
        return;
    }

    networkDownloader.SetMapQuestOpenKey(m_mapquestKeyLineEdit->text());

    QDialog::accept();
}