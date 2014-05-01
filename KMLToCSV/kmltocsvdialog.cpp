#include "kmltocsvdialog.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtGui/QCloseEvent>
#include "application.h"

KMLToCSVDialog::KMLToCSVDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout(this);

    m_inputKML = new SynGlyphX::FileLineEdit("KML/KMZ files (*.kml, *.kmz)", this);
    formLayout->addRow(tr("KML/KMZ file:"), m_inputKML);

    m_inputGlyph = new SynGlyphX::FileLineEdit("CSV files (*.csv)", this);
    formLayout->addRow(tr("Glyph file:"), m_inputGlyph);

    verticalLayout->addLayout(formLayout);
    verticalLayout->addStretch();

    QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, this);

    verticalLayout->addWidget(dialogButtonBox);

    setLayout(verticalLayout);

    QObject::connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));

    ReadSettings();
}

KMLToCSVDialog::~KMLToCSVDialog()
{

}

QString KMLToCSVDialog::GetKMLFilename() const {

    QFileInfo fileInfo(m_inputKML->GetFilename());
    return fileInfo.canonicalFilePath();
}

QString KMLToCSVDialog::GetCSVFilename() const {

    QFileInfo fileInfo(m_inputGlyph->GetFilename());
    return fileInfo.canonicalFilePath();
}

void KMLToCSVDialog::accept() {

    QString kmlFilename = m_inputKML->GetFilename();
    QString csvFilename = m_inputGlyph->GetFilename();

    if (kmlFilename.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("KML/KMZ file field is empty"));
        return;
    }

    if (csvFilename.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Glyph file field is empty"));
        return;
    }

    if ((kmlFilename.right(4) != ".kml") && (kmlFilename.right(4) != ".kmz")) {
        QMessageBox::warning(this, tr("Error"), tr("KML/KMZ file is not in either kml or kmz format"));
        return;
    }

    if (csvFilename.right(4) != ".csv") {
        QMessageBox::warning(this, tr("Error"), tr("CSV file is not in csv format"));
        return;
    }

    if (!QFileInfo::exists(kmlFilename)) {
        QMessageBox::warning(this, tr("Error"), tr("KML/KMZ file does not exist"));
        return;
    }

    if (!QFileInfo::exists(csvFilename)) {
        QMessageBox::warning(this, tr("Error"), tr("CSV file does not exist"));
        return;
    }

    QStringList args;
    args.append(GetKMLFilename());
    args.append(GetCSVFilename());
    int result = QProcess::execute("GPSToCSV.exe", args);

    if (result == -2) {
        QMessageBox::critical(this, tr("Error"), tr("Conversion failed to start"));
    }
    else if (result == -1) {
        QMessageBox::critical(this, tr("Error"), tr("Conversion crashed and failed"));
    }
    else {
        QMessageBox::information(this, tr("Success"), tr("File successfully converted"));
    }
}

void KMLToCSVDialog::ReadSettings() {

    QSettings settings;

    settings.beginGroup("Dialog");
    resize(settings.value("size", QSize(800, 120)).toSize());
    settings.endGroup();

    settings.beginGroup("params");
    m_inputGlyph->SetFilename(settings.value("glyph", QDir::toNativeSeparators(QDir::currentPath() + "/pin.csv")).toString());
    settings.endGroup();
}

void KMLToCSVDialog::WriteSettings() {

    QSettings settings;

    settings.beginGroup("Dialog");
    settings.setValue("size", size());
    settings.endGroup();

    settings.beginGroup("params");
    settings.setValue("glyph", m_inputGlyph->GetFilename());
    settings.endGroup();
}

void KMLToCSVDialog::reject() {

    WriteSettings();
    QDialog::reject();
}