#include "mapoptionswidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtCore/QSettings>

MapOptionsWidget::MapOptionsWidget(QWidget *parent)
    : QWidget(parent)
{
	QVBoxLayout* mapOptionsLayout = new QVBoxLayout(this);

	QHBoxLayout* serviceSizeLayout = new QHBoxLayout(this);

	m_mapServiceComboBox = new QComboBox(this);
	m_mapServiceComboBox->addItem("MapQuest Open (OpenStreetMap)");
	m_mapServiceComboBox->addItem("Google Maps");

	QObject::connect(m_mapServiceComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MapOptionsWidget::OnMapSourceChanged);

    m_imageSizeWidget = new SynGlyphX::IntSizeWidget(false, this);

	serviceSizeLayout->addWidget(new QLabel(tr("Service:")));
	serviceSizeLayout->addWidget(m_mapServiceComboBox);
	serviceSizeLayout->addWidget(m_imageSizeWidget);

	mapOptionsLayout->addLayout(serviceSizeLayout);

	QHBoxLayout* optionsLayout = new QHBoxLayout(this);

    QLabel* mapTypeLabel = new QLabel(tr("Type:"), this);
    m_mapTypeComboBox = new QComboBox(this);
	m_mapTypeComboBox->addItem(tr("Map"), SynGlyphX::DownloadedMapProperties::Map);
	m_mapTypeComboBox->addItem(tr("Satellite"), SynGlyphX::DownloadedMapProperties::Satellite);
	m_mapTypeComboBox->addItem(tr("Hybrid"), SynGlyphX::DownloadedMapProperties::Hybrid);

	optionsLayout->addWidget(mapTypeLabel);
	optionsLayout->addWidget(m_mapTypeComboBox);

	m_invertCheckbox = new QCheckBox(tr("Invert Colors"), this);
	optionsLayout->addWidget(m_invertCheckbox);

	m_grayscaleCheckbox = new QCheckBox(tr("Grayscale"), this);
	optionsLayout->addWidget(m_grayscaleCheckbox);

	optionsLayout->addStretch(1);

	mapOptionsLayout->addLayout(optionsLayout);

    setLayout(mapOptionsLayout);

	m_mapServiceComboBox->setCurrentIndex(0);
	OnMapSourceChanged();

    //ReadSettings();

    //Disabling size widget until image size limitations in our viewer are removed
    m_imageSizeWidget->setEnabled(false);

    //Google Maps Download is not implemented yet so keep the option disabled
	m_mapServiceComboBox->setEnabled(false);

	m_mapTypeComboBox->setCurrentIndex(2);
	m_imageSizeWidget->SetSize(QSize(2048, 1024));
}

MapOptionsWidget::~MapOptionsWidget()
{
   // WriteSettings();
}

void MapOptionsWidget::SetWidget(SynGlyphX::DownloadedMapProperties::ConstSharedPtr properties) {

	m_mapServiceComboBox->setCurrentIndex(static_cast<int>(properties->GetSource()));
	m_mapTypeComboBox->setCurrentIndex(static_cast<int>(properties->GetType()));
	m_imageSizeWidget->SetSize(QSize(properties->GetSize()[0], properties->GetSize()[1]));
	m_invertCheckbox->setChecked(properties->GetInvert());
	m_grayscaleCheckbox->setChecked(properties->GetGrayscale());
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

SynGlyphX::DownloadedMapProperties::SharedPtr MapOptionsWidget::GetProperties() const {

	SynGlyphX::DownloadedMapProperties::SharedPtr downloadedMapProperties = std::make_shared<SynGlyphX::DownloadedMapProperties>();

	downloadedMapProperties->SetSource(static_cast<SynGlyphX::DownloadedMapProperties::MapSource>(m_mapServiceComboBox->currentIndex()));
	downloadedMapProperties->SetType(static_cast<SynGlyphX::DownloadedMapProperties::MapType>(m_mapTypeComboBox->currentIndex()));
	downloadedMapProperties->SetSize({ { m_imageSizeWidget->GetSize().width(), m_imageSizeWidget->GetSize().height() } });
	downloadedMapProperties->SetInvert(m_invertCheckbox->isChecked());
	downloadedMapProperties->SetGrayscale(m_grayscaleCheckbox->isChecked());

	return downloadedMapProperties;
}

void MapOptionsWidget::OnMapSourceChanged() {

	if (m_mapServiceComboBox->currentIndex() == 1) {

        m_imageSizeWidget->SetRange(1, 2048);
    }
    else {

        m_imageSizeWidget->SetRange(1, 3840);
    }
}