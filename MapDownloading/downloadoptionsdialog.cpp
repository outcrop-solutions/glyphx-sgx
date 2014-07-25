#include "downloadoptionsdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include "networkdownloader.h"

DownloadOptionsDialog::DownloadOptionsDialog(QWidget *parent)
    : QDialog(parent)
{
    setMinimumWidth(500);

    QVBoxLayout* layout = new QVBoxLayout(this);
    m_addPointsToMap = new QCheckBox(tr("Add Points To Map"), this);
    layout->addWidget(m_addPointsToMap);

    m_mapquestKeyLineEdit = new QLineEdit(this);
    m_googleMapsKeyLineEdit = new QLineEdit(this);
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

    setWindowTitle(tr("Map Download Options"));

    NetworkDownloader& networkDownloader = NetworkDownloader::Instance();
    m_mapquestKeyLineEdit->setText(networkDownloader.GetMapQuestOpenKey());
    m_addPointsToMap->setChecked(networkDownloader.GetShowPointsInMap());
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
    networkDownloader.SetShowPointsInMap(m_addPointsToMap->isChecked());

    QDialog::accept();
}