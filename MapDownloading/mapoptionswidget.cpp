#include "mapoptionswidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtCore/QSettings>

MapOptionsWidget::MapOptionsWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* mapOptionsLayout = new QHBoxLayout(this);

    m_mapquestRadioButton = new QRadioButton("MapQuest Open (OpenStreetMap)", this);
    m_googleRadioButton = new QRadioButton("Google Maps", this);
    QObject::connect(m_mapquestRadioButton, SIGNAL(clicked()), this, SLOT(OnMapSourceChanged()));
    QObject::connect(m_googleRadioButton, SIGNAL(clicked()), this, SLOT(OnMapSourceChanged()));

    m_imageSizeWidget = new SynGlyphX::SizeWidget(this);

    QLabel* mapTypeLabel = new QLabel(tr("Type:"), this);
    m_mapTypeComboBox = new QComboBox(this);
    m_mapTypeComboBox->addItem(tr("Map"), NetworkDownloader::Map);
    m_mapTypeComboBox->addItem(tr("Satellite"), NetworkDownloader::Satellite);
    m_mapTypeComboBox->addItem(tr("Hybrid"), NetworkDownloader::Hybrid);

    mapOptionsLayout->addWidget(m_mapquestRadioButton);
    mapOptionsLayout->addWidget(m_googleRadioButton);
    mapOptionsLayout->addStretch(1);
    mapOptionsLayout->addWidget(m_imageSizeWidget);
    mapOptionsLayout->addWidget(mapTypeLabel);
    mapOptionsLayout->addWidget(m_mapTypeComboBox);

    setLayout(mapOptionsLayout);

    ReadSettings();

    //Disabling size widget until image size limitations in our viewer are removed
    m_imageSizeWidget->setEnabled(false);

    //Google Maps Download is not implemented yet so keep the option disabled
    m_googleRadioButton->setEnabled(false);
}

MapOptionsWidget::~MapOptionsWidget()
{
    WriteSettings();
}

void MapOptionsWidget::ReadSettings() {

    QSettings settings;
    settings.beginGroup("MapOptions");
    SetRadioButtonsFromMapSource(static_cast<NetworkDownloader::MapSource>(settings.value("MapSource", NetworkDownloader::Hybrid).toInt()));
    m_mapTypeComboBox->setCurrentIndex(settings.value("MapType").toInt());
    m_imageSizeWidget->SetSize(settings.value("MapSize", QSize(2048, 1024)).toSize());
    settings.endGroup();
}

void MapOptionsWidget::WriteSettings() {

    QSettings settings;
    settings.beginGroup("MapOptions");
    settings.setValue("MapSource", GetMapSource());
    settings.setValue("MapType", m_mapTypeComboBox->currentIndex());
    settings.setValue("MapSize", m_imageSizeWidget->GetSize());
    settings.endGroup();
}

NetworkDownloader::MapSource MapOptionsWidget::GetMapSource() const {

    if (m_googleRadioButton->isChecked()) {
        return NetworkDownloader::GoogleMaps;
    }
    else {
        return NetworkDownloader::MapQuestOpen;
    }
}

void MapOptionsWidget::SetRadioButtonsFromMapSource(NetworkDownloader::MapSource source) {

    if (source == NetworkDownloader::GoogleMaps) {
        m_googleRadioButton->setChecked(true);
    }
    else {
        m_mapquestRadioButton->setChecked(true);
    }
    OnMapSourceChanged();
}

void MapOptionsWidget::OnMapSourceChanged() {

    if (m_googleRadioButton->isChecked()) {
        m_imageSizeWidget->SetRange(1, 2048);
    }
    else {
        m_imageSizeWidget->SetRange(1, 3840);
    }
}

QSize MapOptionsWidget::GetMapSize() const {

    return m_imageSizeWidget->GetSize();
}

NetworkDownloader::MapType MapOptionsWidget::GetMapType() const {

    return static_cast<NetworkDownloader::MapType>(m_mapTypeComboBox->currentData().toInt());
}