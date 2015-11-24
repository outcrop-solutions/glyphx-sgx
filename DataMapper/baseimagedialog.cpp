#include "baseimagedialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <QtGui/QImage>
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include "groupboxsinglewidget.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"
#include "colorconverter.h"

const BaseImageDialog::PresetMap BaseImageDialog::s_presets = 
	{ PositionOrientation(SynGlyphX::Vector3({ { 0.0, 0.0, 0.0 } }), SynGlyphX::Vector3({ { 0.0, 0.0, 0.0 } })),
	PositionOrientation(SynGlyphX::Vector3({ { 0.0, 0.0, 180.0 } }), SynGlyphX::Vector3({ { 180.0, 0.0, 0.0 } })),
	PositionOrientation(SynGlyphX::Vector3({ { -180.0, 0.0, 90.0 } }), SynGlyphX::Vector3({ { 90.0, 90.0, 180.0 } })),
	PositionOrientation(SynGlyphX::Vector3({ { 180.0, 0.0, 90.0 } }), SynGlyphX::Vector3({ { 90.0, 270.0, 180.0 } })),
	PositionOrientation(SynGlyphX::Vector3({ { 0.0, 90.0, 90.0 } }), SynGlyphX::Vector3({ { 90.0, 180.0, 180.0 } })) };

const std::vector<std::string> BaseImageDialog::s_presetNames = { "Up", "Down", "Left", "Right", "Front" };

BaseImageDialog::BaseImageDialog(bool enablePositionAndOrientation, bool showDownloadMapOptions, QWidget *parent)
	: QDialog(parent),
	m_downloadedMapOptionsWidget(nullptr)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	
	QLabel* comboBoxLabel = new QLabel(tr("Base Image Type: "), this);
	m_baseImageComboBox = new QComboBox(this);
	
	for (auto baseImageType : SynGlyphX::BaseImage::s_baseImageTypeStrings) {

		if ((baseImageType.get_left() != SynGlyphX::BaseImage::DownloadedMap) || (showDownloadMapOptions)) {

			m_baseImageComboBox->addItem(QString::fromStdWString(baseImageType.get_right()));
		}
	}

	QHBoxLayout* comboBoxLayout = new QHBoxLayout(this);
	comboBoxLayout->addWidget(comboBoxLabel);
	comboBoxLayout->addWidget(m_baseImageComboBox);
	comboBoxLayout->addStretch(1);

	mainLayout->addLayout(comboBoxLayout);

	m_baseImageOptionsStackedWidget = new QStackedWidget(this);
	QWidget* defaultImageWidget = new QWidget(m_baseImageOptionsStackedWidget);

	m_defaultImagesComboBox = new SynGlyphX::DefaultBaseImagesComboBox(defaultImageWidget);
	QHBoxLayout* defaultImageLayout = new QHBoxLayout(defaultImageWidget);
	defaultImageLayout->addStretch(1);
	defaultImageLayout->addWidget(new QLabel(tr("Default Image:"), defaultImageWidget));
	defaultImageLayout->addWidget(m_defaultImagesComboBox);
	defaultImageLayout->addStretch(1);
	defaultImageWidget->setLayout(defaultImageLayout);
	m_baseImageOptionsStackedWidget->addWidget(defaultImageWidget);

	if (showDownloadMapOptions) {

		m_downloadedMapOptionsWidget = new MapOptionsWidget(this);
		SynGlyphX::GroupBoxSingleWidget* downloadedMapOptionsGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Map Options"), m_downloadedMapOptionsWidget, this);
		downloadedMapOptionsGroupBox->setContentsMargins(0, 0, 0, 0);
		m_baseImageOptionsStackedWidget->addWidget(downloadedMapOptionsGroupBox);
	}

	m_userDefinedImageLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileOpen, true, m_baseImageOptionsStackedWidget);
	m_userDefinedImageLineEdit->setContentsMargins(4, 4, 4, 4);

	//For now only allowing PNG files 
	m_userDefinedImageLineEdit->SetFilters("PNG files (*.png)");

	SynGlyphX::GroupBoxSingleWidget* userDefinedGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Image File"), m_userDefinedImageLineEdit, m_baseImageOptionsStackedWidget);

	m_baseImageOptionsStackedWidget->addWidget(userDefinedGroupBox);

	mainLayout->addWidget(m_baseImageOptionsStackedWidget);

	QObject::connect(m_baseImageComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), m_baseImageOptionsStackedWidget, &QStackedWidget::setCurrentIndex);

	m_positionWidget = new SynGlyphX::XYZWidget(false, this);
	m_positionWidget->SetRange(-5000.0, 5000.0);
	m_positionWidget->SetWrapping(false);
	m_positionWidget->setEnabled(enablePositionAndOrientation);
	SynGlyphX::GroupBoxSingleWidget* positionGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Position"), m_positionWidget, this);
	mainLayout->addWidget(positionGroupBox);

	m_orientationWidget = new SynGlyphX::XYZWidget(false, this);
	m_orientationWidget->SetRange(0.0, 360.0);
	m_orientationWidget->SetWrapping(false);
	m_orientationWidget->setEnabled(false);
	SynGlyphX::GroupBoxSingleWidget* orientationGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Orientation"), m_orientationWidget, this);
	mainLayout->addWidget(orientationGroupBox);

	m_presetButtonSignalMapper = new QSignalMapper(this);

	QGroupBox* presetGroupBox = new QGroupBox(tr("Position/Orientation Presets"), this);
	QHBoxLayout* presetGroupBoxLayout = new QHBoxLayout(this);
	for (int i = 0; i < s_presetNames.size(); ++i) {

		QPushButton* presetButton = new QPushButton(tr(s_presetNames[i].c_str()), presetGroupBox);
		presetGroupBoxLayout->addWidget(presetButton);
		QObject::connect(presetButton, &QPushButton::clicked, m_presetButtonSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
		m_presetButtonSignalMapper->setMapping(presetButton, i);
	}
	QObject::connect(m_presetButtonSignalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &BaseImageDialog::PresetButtonClicked);
	presetGroupBox->setLayout(presetGroupBoxLayout);
	presetGroupBox->setEnabled(enablePositionAndOrientation);
	mainLayout->addWidget(presetGroupBox);

	m_worldSizeWidget = new SynGlyphX::DoubleSizeWidget(true, this);
	m_worldSizeWidget->SetRange(0.1, 10000.0);
	m_worldSizeWidget->SetSize(QSizeF(360.0, 180.0));
	SynGlyphX::GroupBoxSingleWidget* worldSizeGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("World Space Size"), m_worldSizeWidget, this);
	mainLayout->addWidget(worldSizeGroupBox);

	QGroupBox* gridLinesGroupBox = new QGroupBox(tr("Grid Lines"), this);
	QHBoxLayout* gridLinesBoxLayout = new QHBoxLayout(this);

	QPushButton* hideGridLinesButton = new QPushButton(tr("Hide"), this);
	QObject::connect(hideGridLinesButton, &QPushButton::pressed, this, [&, this](){ m_gridLinesCountsWidget->SetSize(QSize(0, 0)); });
	gridLinesBoxLayout->addWidget(hideGridLinesButton);

	m_gridLinesCountsWidget = new SynGlyphX::IntSizeWidget(true, tr("Horizontal:"), tr("Vertical:"), this);
	m_gridLinesCountsWidget->SetRange(0, 255);
	gridLinesBoxLayout->addWidget(m_gridLinesCountsWidget);
	
	QLabel* colorLabel = new QLabel(tr("Color:"), this);
	gridLinesBoxLayout->addWidget(colorLabel);
	m_gridLinesColorButton = new SynGlyphX::ColorButton(this);
	m_gridLinesColorButton->SetColor(Qt::black);
	gridLinesBoxLayout->addWidget(m_gridLinesColorButton);

	gridLinesGroupBox->setLayout(gridLinesBoxLayout);

	mainLayout->addWidget(gridLinesGroupBox);

	mainLayout->addStretch(1);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &BaseImageDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &BaseImageDialog::reject);

	setLayout(mainLayout);
}

BaseImageDialog::~BaseImageDialog()
{

}

void BaseImageDialog::accept() {

	SynGlyphX::BaseImage::Type baseImageType = SynGlyphX::BaseImage::s_baseImageTypeStrings.right.at(m_baseImageComboBox->currentText().toStdWString());
	if (baseImageType == SynGlyphX::BaseImage::Type::UserImage) {

		QString userImageFile = m_userDefinedImageLineEdit->GetText();
		if (userImageFile.isEmpty()) {

			QMessageBox::warning(this, tr("Base Image Error"), tr("File name listed for local image is empty."));
			return;
		}
		if (userImageFile.right(4).toLower() != ".png") {

			QMessageBox::warning(this, tr("Base Image Error"), tr("File name listed for local image is not a png file."));
			return;
		}
		if (!QFile::exists(userImageFile)) {

			QMessageBox::warning(this, tr("Base Image Error"), tr("File name listed for local image does not exist."));
			return;
		}

		QImage image(userImageFile);
		if (image.isNull()) {

			QMessageBox::warning(this, tr("Base Image Error"), tr("File name listed for local image is an invalid image."));
			return;
		}

		if (image.isNull()) {

			QMessageBox::warning(this, tr("Base Image Error"), tr("File name listed for local image is an invalid image."));
			return;
		}

		//This is here because ANTz can't handle images of sizes other than 2048 x 1024
		if (image.size() != QSize(2048, 1024)) {

			QMessageBox::warning(this, tr("Base Image Error"), tr("File name listed for local image needs to be a size of 2048 x 1024."));
			return;
		}
	}
	QDialog::accept();
}

void BaseImageDialog::SetBaseImage(const SynGlyphX::BaseImage& baseImage) {

	SynGlyphX::BaseImage::Type baseImageType = baseImage.GetType();
	m_baseImageComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::BaseImage::s_baseImageTypeStrings.left.at(baseImageType)));
	if (baseImageType == SynGlyphX::BaseImage::Type::DownloadedMap) {

		auto properties = std::dynamic_pointer_cast<const SynGlyphX::DownloadedMapProperties>(baseImage.GetProperties());
		m_downloadedMapOptionsWidget->SetWidget(properties);
	}
	else if (baseImageType == SynGlyphX::BaseImage::Type::UserImage) {

		auto properties = std::dynamic_pointer_cast<const SynGlyphX::UserDefinedBaseImageProperties>(baseImage.GetProperties());
		m_userDefinedImageLineEdit->SetText(QString::fromStdWString(properties->GetFilename()));
	}
	else {

		auto properties = std::dynamic_pointer_cast<const SynGlyphX::DefaultBaseImageProperties>(baseImage.GetProperties());
		m_defaultImagesComboBox->SetDefaultBaseImage(properties->GetDefaultBaseImageType());
	}
	m_positionWidget->Set(baseImage.GetPosition());
	m_orientationWidget->Set(baseImage.GetRotationAngles());

	m_worldSizeWidget->SetSize(SynGlyphX::DoubleSizeWidget::ConvertToQSizeF(baseImage.GetWorldSize()));

	SynGlyphX::IntSize gridLinesCounts = baseImage.GetGridLineCounts();
	m_gridLinesCountsWidget->SetSize(QSize(gridLinesCounts[0], gridLinesCounts[1]));
	m_gridLinesColorButton->SetColor(SynGlyphX::ColorConverter::GlyphColor2QColor(baseImage.GetGridLinesColor()));
}

SynGlyphX::BaseImage BaseImageDialog::GetBaseImage() const {

	SynGlyphX::BaseImage newBaseImage(nullptr);

	SynGlyphX::BaseImage::Type baseImageType = SynGlyphX::BaseImage::s_baseImageTypeStrings.right.at(m_baseImageComboBox->currentText().toStdWString());
	if (baseImageType == SynGlyphX::BaseImage::Type::DownloadedMap) {

		newBaseImage.SetProperties(m_downloadedMapOptionsWidget->GetProperties());
	}
	else if (baseImageType == SynGlyphX::BaseImage::Type::UserImage) {

		auto properties = std::make_shared<SynGlyphX::UserDefinedBaseImageProperties>(m_userDefinedImageLineEdit->GetText().toStdWString());
		newBaseImage.SetProperties(properties);
	}
	else {

		auto properties = std::make_shared<SynGlyphX::DefaultBaseImageProperties>(m_defaultImagesComboBox->GetDefaultBaseImage());
		newBaseImage.SetProperties(properties);
	}

	newBaseImage.SetPosition(m_positionWidget->Get());
	newBaseImage.SetRotation(m_orientationWidget->Get());
	newBaseImage.SetWorldSize(SynGlyphX::DoubleSizeWidget::ConvertFromQSizeF(m_worldSizeWidget->GetSize()));

	newBaseImage.SetGridLineCounts(SynGlyphX::IntSizeWidget::ConvertFromQSize(m_gridLinesCountsWidget->GetSize()));
	newBaseImage.SetGridLinesColor(SynGlyphX::ColorConverter::QColor2GlyphColor(m_gridLinesColorButton->GetColor()));

	return newBaseImage;
}

void BaseImageDialog::PresetButtonClicked(int id) {

	const PositionOrientation& preset = s_presets[id];
	m_positionWidget->Set(preset.first);
	m_orientationWidget->Set(preset.second);
}