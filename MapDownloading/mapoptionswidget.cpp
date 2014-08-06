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
	m_mapTypeComboBox->addItem(tr("Map"), SynGlyphX::DownloadedMapProperties::Map);
	m_mapTypeComboBox->addItem(tr("Satellite"), SynGlyphX::DownloadedMapProperties::Satellite);
	m_mapTypeComboBox->addItem(tr("Hybrid"), SynGlyphX::DownloadedMapProperties::Hybrid);

    mapOptionsLayout->addWidget(m_mapquestRadioButton);
    mapOptionsLayout->addWidget(m_googleRadioButton);
    mapOptionsLayout->addStretch(1);
    mapOptionsLayout->addWidget(m_imageSizeWidget);
    mapOptionsLayout->addWidget(mapTypeLabel);
    mapOptionsLayout->addWidget(m_mapTypeComboBox);

    setLayout(mapOptionsLayout);

    //ReadSettings();

    //Disabling size widget until image size limitations in our viewer are removed
    m_imageSizeWidget->setEnabled(false);

    //Google Maps Download is not implemented yet so keep the option disabled
    m_googleRadioButton->setEnabled(false);

	SetRadioButtonsFromMapSource(SynGlyphX::DownloadedMapProperties::MapSource::MapQuestOpen);
	m_mapTypeComboBox->setCurrentIndex(2);
	m_imageSizeWidget->SetSize(QSize(2048, 1024));
}

MapOptionsWidget::~MapOptionsWidget()
{
   // WriteSettings();
}

void MapOptionsWidget::Set(SynGlyphX::DownloadedMapProperties::MapSource source, SynGlyphX::DownloadedMapProperties::MapType type, const QSize& size) {

	SetRadioButtonsFromMapSource(source);
	m_mapTypeComboBox->setCurrentIndex(static_cast<int>(type));
	m_imageSizeWidget->SetSize(size);
}

/*
void MapOptionsWidget::ReadSettings() {

    QSettings settings;
    settings.beginGroup("MapOptions");
	SetRadioButtonsFromMapSource(static_cast<SynGlyphX::DownloadedMapProperties::MapSource>(settings.value("MapSource", SynGlyphX::DownloadedMapProperties::Hybrid).toInt()));
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
}*/

SynGlyphX::DownloadedMapProperties::MapSource MapOptionsWidget::GetMapSource() const {

    if (m_googleRadioButton->isChecked()) {
		return SynGlyphX::DownloadedMapProperties::GoogleMaps;
    }
    else {
		return SynGlyphX::DownloadedMapProperties::MapQuestOpen;
    }
}

void MapOptionsWidget::SetRadioButtonsFromMapSource(SynGlyphX::DownloadedMapProperties::MapSource source) {

	if (source == SynGlyphX::DownloadedMapProperties::GoogleMaps) {
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

SynGlyphX::DownloadedMapProperties::MapType MapOptionsWidget::GetMapType() const {

	return static_cast<SynGlyphX::DownloadedMapProperties::MapType>(m_mapTypeComboBox->currentData().toInt());
}