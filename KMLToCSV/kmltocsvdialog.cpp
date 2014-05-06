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

    m_inputKML = new SynGlyphX::FileLineEdit("KML & KMZ files (*.kmz *.kml)", this);
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

bool KMLToCSVDialog::ValidateInput() {

    QString kmlFilename = m_inputKML->GetFilename();
    QString glyphFilename = m_inputGlyph->GetFilename();

    if (kmlFilename.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("KML/KMZ file field is empty"));
        return false;
    }

    if (glyphFilename.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Glyph file field is empty"));
        return false;
    }

    if ((kmlFilename.right(4) != ".kml") && (kmlFilename.right(4) != ".kmz")) {
        QMessageBox::warning(this, tr("Error"), tr("KML/KMZ file is not in either kml or kmz format"));
        return false;
    }

    if (glyphFilename.right(4) != ".csv") {
        QMessageBox::warning(this, tr("Error"), tr("CSV file is not in csv format"));
        return false;
    }

    if (!QFileInfo::exists(kmlFilename)) {
        QMessageBox::warning(this, tr("Error"), tr("KML/KMZ file does not exist"));
        return false;
    }

    if (!QFileInfo::exists(glyphFilename)) {
        QMessageBox::warning(this, tr("Error"), tr("CSV file does not exist"));
        return false;
    }

    return true;
}

void KMLToCSVDialog::accept() {

    if (ValidateInput()) {

        QString kmlFilename = GetKMLFilename();
        QString tempKMLFilename;
        QStringList args;

        if (kmlFilename.right(4) == ".kml") {
            args.append("kml_parser.py");
            tempKMLFilename = QDir::toNativeSeparators(QDir::currentPath() + "/doc.kml");
        }
        else {
            args.append("kmz_parser.py");
            tempKMLFilename = QDir::toNativeSeparators(QDir::currentPath() + "/test.kmz");
        }
        QFile::copy(kmlFilename, tempKMLFilename);

        QString dataInFilename = QDir::toNativeSeparators(QDir::currentPath() + "/data_in.csv");

        if (!RunCommand("python.exe", args, dataInFilename)) {
            return;
        }

        args.clear();
        args.append("-p");
        args.append("-k");
        args.append("-G");
        args.append(GetCSVFilename());

        QString kmlPrefix = kmlFilename.left(kmlFilename.length() - 4);

        if (!RunCommand(SynGlyphX::Application::applicationDirPath() + "/gps2csv.exe", args, kmlPrefix + ".csv")) {
            return;
        }

        QFile::rename(QDir::toNativeSeparators(QDir::currentPath() + "/corners.txt"), kmlPrefix + "_corners.txt");
        QFile::rename(QDir::toNativeSeparators(QDir::currentPath() + "/corners.kml"), kmlPrefix + "_corners.kml");

        //Cleanup
        QFile::remove(tempKMLFilename);
        QFile::remove(dataInFilename);

        QMessageBox::information(this, tr("Success"), tr("File successfully converted"));
    }
}

bool KMLToCSVDialog::RunCommand(const QString& program, const QStringList& args, const QString& stdOutFile) {

    QProcess process;
    process.setProgram(program);
    process.setArguments(args);
    process.setStandardOutputFile(stdOutFile);
    process.start();

    if (!process.waitForFinished()) {
        QMessageBox::critical(this, tr("Error"), program + tr(" failed to finish"));
        return false;
    }

    if (process.exitStatus() != QProcess::NormalExit) {
        QMessageBox::critical(this, tr("Error"), program + tr(" crashed and failed"));
        return false;
    }
    
    return true;
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