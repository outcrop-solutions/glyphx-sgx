#include "mapoptionswidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGroupBox>
#include <QtCore/QSettings>
#include <QtWidgets/QSpinBox>

const char* ArcGisEnv = "SGXARCGIS";

MapOptionsWidget::MapOptionsWidget(QWidget *parent)
    : QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* mapOptionsLayout = new QHBoxLayout(this);

	QLabel* mapServiceLabel = new QLabel(tr("Service:"), this);

	m_mapServiceComboBox = new QComboBox(this);
	if (qEnvironmentVariableIsSet(ArcGisEnv) && (qgetenv(ArcGisEnv).toInt() != 0)) {

		m_mapServiceComboBox->addItem("ArcGIS - OpenStreetMap layer");
	}
	else {

		m_mapServiceComboBox->addItem("MapQuest Open (OpenStreetMap)");
	}
	m_mapServiceComboBox->addItem("Google Maps");

	mapOptionsLayout->addWidget(mapServiceLabel);
	mapOptionsLayout->addWidget(m_mapServiceComboBox);

	QLabel* mapTypeLabel = new QLabel(tr("Type:"), this);
	m_mapTypeComboBox = new QComboBox(this);
	m_mapTypeComboBox->addItem(tr("Map"), SynGlyphX::DownloadedMapProperties::Map);
	m_mapTypeComboBox->addItem(tr("Satellite"), SynGlyphX::DownloadedMapProperties::Satellite);
	m_mapTypeComboBox->addItem(tr("Hybrid"), SynGlyphX::DownloadedMapProperties::Hybrid);

	mapOptionsLayout->addWidget(mapTypeLabel);
	mapOptionsLayout->addWidget(m_mapTypeComboBox);

	QObject::connect(m_mapServiceComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MapOptionsWidget::OnMapSourceChanged);

	m_invertCheckbox = new QCheckBox(tr("Invert Colors"), this);
	mapOptionsLayout->addWidget(m_invertCheckbox);

	m_grayscaleCheckbox = new QCheckBox(tr("Grayscale"), this);
	mapOptionsLayout->addWidget(m_grayscaleCheckbox);

	QGroupBox* mapOptionsGroupBox = new QGroupBox(tr("Map Options"), this);
	mapOptionsGroupBox->setLayout(mapOptionsLayout);
	mainLayout->addWidget(mapOptionsGroupBox);

	QHBoxLayout* mapSizeLayout = new QHBoxLayout(this);
	mapSizeLayout->setContentsMargins(0, 0, 0, 0);

	m_imageSizeWidget = new SynGlyphX::IntSizeWidget(false, this);
	mapSizeLayout->addWidget(m_imageSizeWidget);
	
	QStringList bestFitOptions;
	bestFitOptions.push_back(tr("Fixed Size"));
	bestFitOptions.push_back(tr("Best Fit Closest To Size"));
	//m_bestFitRadioButtonWidget = new SynGlyphX::RadioButtonGroupWidget(bestFitOptions, Qt::Horizontal, this);
	//mapSizeLayout->addWidget(m_bestFitRadioButtonWidget);
	//QObject::connect(m_bestFitRadioButtonWidget, &SynGlyphX::RadioButtonGroupWidget::ButtonClicked, this, &MapOptionsWidget::OnSizeOptionChanged);

	QLabel *marginLabel = new QLabel(tr("Margin:"), this);
	mapSizeLayout->addWidget(marginLabel);

	m_marginSpinBox = new QSpinBox(this);
	m_marginSpinBox->setMinimum(0);
	m_marginSpinBox->setMaximum(999);
	m_marginSpinBox->setValue(50);
	mapSizeLayout->addWidget(m_marginSpinBox);

	mapSizeLayout->addStretch(1);

	QGroupBox* mapSizeGroupBox = new QGroupBox(tr("Map Image Size"), this);
	mapSizeGroupBox->setLayout(mapSizeLayout);
	mainLayout->addWidget(mapSizeGroupBox);

	setLayout(mainLayout);

	m_mapServiceComboBox->setCurrentIndex(0);
	OnMapSourceChanged();
	//m_bestFitRadioButtonWidget->SetCheckedButton(1);

    //ReadSettings();

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
	//m_bestFitRadioButtonWidget->SetCheckedButton(properties->GetUseBestFit() ? 1 : 0);
	m_marginSpinBox->setValue(properties->GetMargin());
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
	downloadedMapProperties->SetSize({ { static_cast<unsigned int>(m_imageSizeWidget->GetSize().width()), static_cast<unsigned int>(m_imageSizeWidget->GetSize().height()) } });
	downloadedMapProperties->SetInvert(m_invertCheckbox->isChecked());
	downloadedMapProperties->SetGrayscale(m_grayscaleCheckbox->isChecked());
	//downloadedMapProperties->SetUseBestFit(m_bestFitRadioButtonWidget->GetCheckedButton() == 1);
	downloadedMapProperties->SetMargin(static_cast<unsigned int>(m_marginSpinBox->value()));

	return downloadedMapProperties;
}

void MapOptionsWidget::OnMapSourceChanged() {

	if (m_mapServiceComboBox->currentIndex() == 1) {

        m_imageSizeWidget->SetRange(1, SynGlyphX::DownloadedMapProperties::MaxSizeGoogleMaps);
    }
    else {

		m_imageSizeWidget->SetRange(1, SynGlyphX::DownloadedMapProperties::MaxSizeMapQuest);
    }
}

void MapOptionsWidget::OnSizeOptionChanged() {

	//m_marginSpinBox->setEnabled(m_bestFitRadioButtonWidget->GetCheckedButton() == 1);
}