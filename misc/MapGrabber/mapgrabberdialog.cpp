#include "mapgrabberdialog.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtWidgets/QGroupBox>
#include "downloadoptionsdialog.h"
#include "groupboxsinglewidget.h"

MapGrabberDialog::MapGrabberDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout(this);

    m_mapFilenameLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileSave, true, this);
    formLayout->addRow(tr("File name:"), m_mapFilenameLineEdit);

    m_mapOptionsWidget = new MapOptionsWidget(this);
    SynGlyphX::GroupBoxSingleWidget* optionsGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Map Options"), m_mapOptionsWidget, this);
    formLayout->addRow(optionsGroupBox);

    m_mapBoundingBox = new QLineEdit(this);
    m_mapBoundingBox->setReadOnly(true);
    formLayout->addRow(tr("Map bounding box:"), m_mapBoundingBox);

    layout->addLayout(formLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout(this);

    QPushButton* settingsButton = new QPushButton(tr("Map Service Settings"), this);
    QObject::connect(settingsButton, &QPushButton::clicked, this, &MapGrabberDialog::OnMapServiceSettingsButtonClicked);
    buttonLayout->addWidget(settingsButton);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, this);
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    buttonLayout->addWidget(buttonBox);

    layout->addLayout(buttonLayout);

    setLayout(layout);

    ReadSettings();

    setWindowTitle(tr("Map Grabber"));
}

MapGrabberDialog::~MapGrabberDialog()
{

}

void MapGrabberDialog::OnMapServiceSettingsButtonClicked() {

    DownloadOptionsDialog dialog(this);
    dialog.exec();
}

void MapGrabberDialog::ReadSettings() {

    QSettings settings;

    settings.beginGroup("Dialog");
    move(settings.value("pos", QPoint(220, 220)).toPoint());
    resize(settings.value("size", QSize(800, 120)).toSize());
    settings.endGroup();

    settings.beginGroup("params");
    m_mapFilenameLineEdit->SetText(settings.value("mapFilename", QDir::currentPath() + "/download.png").toString());
    settings.endGroup();
}

void MapGrabberDialog::WriteSettings() {

    QSettings settings;

    settings.beginGroup("Dialog");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.endGroup();

    settings.beginGroup("params");
    settings.setValue("mapFilename", m_mapFilenameLineEdit->GetText());
    settings.endGroup();
}

void MapGrabberDialog::reject() {

    WriteSettings();
    QDialog::reject();
}

void MapGrabberDialog::accept() {

    
}
