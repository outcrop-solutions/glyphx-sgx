#include "baseimagedialog.h"
#include <QtWidgets/QVBoxLayout>
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

const boost::bimap<int, SynGlyphX::Vector3> BaseImageDialog::s_orientationPresets = boost::assign::list_of < boost::bimap<int, SynGlyphX::Vector3>::relation >
	(0, SynGlyphX::Vector3({ { 0.0, 0.0, 0.0 } }))
	(1, SynGlyphX::Vector3({ { 180.0, 0.0, 0.0 } }))
	(2, SynGlyphX::Vector3({ { 90.0, 270.0, 180.0 } }))
	(3, SynGlyphX::Vector3({ { 90.0, 90.0, 180.0 } }))
	(4, SynGlyphX::Vector3({ { 90.0, 180.0, 180.0 } }));

const std::vector<QString> BaseImageDialog::s_orientationNames = { "Up", "Down", "Left", "Right", "Front" };

BaseImageDialog::BaseImageDialog(bool enablePositionAndOrientation, QWidget *parent)
	: QDialog(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	
	QLabel* comboBoxLabel = new QLabel(tr("Base Image Type: "), this);
	m_baseImageComboBox = new QComboBox(this);
	
	for (auto baseImageType : SynGlyphX::BaseImage::s_baseImageTypeStrings) {

		m_baseImageComboBox->addItem(QString::fromStdWString(baseImageType.get_right()));
	}

	QHBoxLayout* comboBoxLayout = new QHBoxLayout(this);
	comboBoxLayout->addWidget(comboBoxLabel);
	comboBoxLayout->addWidget(m_baseImageComboBox);
	comboBoxLayout->addStretch(1);

	layout->addLayout(comboBoxLayout);

	m_baseImageOptionsStackedWidget = new QStackedWidget(this);
	QWidget* defaultImageWidget = new QWidget(m_baseImageOptionsStackedWidget);
	m_baseImageOptionsStackedWidget->addWidget(defaultImageWidget);
	m_downloadedMapOptionsWidget = new MapOptionsWidget(this);
	SynGlyphX::GroupBoxSingleWidget* downloadedMapOptionsGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Map Options"), m_downloadedMapOptionsWidget, this);
	downloadedMapOptionsGroupBox->setContentsMargins(0, 0, 0, 0);
	m_baseImageOptionsStackedWidget->addWidget(downloadedMapOptionsGroupBox);

	QWidget* userImageWidget = new QWidget(m_baseImageOptionsStackedWidget);
	m_userDefinedImageLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileOpen, true, userImageWidget);

	//For now only allowing PNG files 
	m_userDefinedImageLineEdit->SetFilters("PNG files (*.png)");

	QVBoxLayout* userDefinedImageLayout = new QVBoxLayout(userImageWidget);
	userDefinedImageLayout->addStretch(1);
	userDefinedImageLayout->addWidget(m_userDefinedImageLineEdit);
	userDefinedImageLayout->addStretch(1);
	userImageWidget->setLayout(userDefinedImageLayout);
	m_baseImageOptionsStackedWidget->addWidget(userImageWidget);

	layout->addWidget(m_baseImageOptionsStackedWidget);

	QObject::connect(m_baseImageComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), m_baseImageOptionsStackedWidget, &QStackedWidget::setCurrentIndex);

	m_positionWidget = new XYZWidget(false, this);
	m_positionWidget->SetRange(-5000.0, 5000.0);
	m_positionWidget->SetWrapping(false);
	m_positionWidget->setEnabled(enablePositionAndOrientation);
	SynGlyphX::GroupBoxSingleWidget* positionGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Position"), m_positionWidget, this);
	layout->addWidget(positionGroupBox);

	QGroupBox* orientationGroupBox = new QGroupBox(tr("Orientation (towards 0,0,0)"), this);
	m_orientationGroup = new QButtonGroup(this);
	QHBoxLayout* orientationLayout = new QHBoxLayout(this);
	for (boost::bimap<int, SynGlyphX::Vector3>::const_iterator iT = s_orientationPresets.begin(); iT != s_orientationPresets.end(); ++iT) {

		QRadioButton* radioButton = new QRadioButton(s_orientationNames.at(iT->left), orientationGroupBox);
		orientationLayout->addWidget(radioButton);
		m_orientationGroup->addButton(radioButton, iT->left);
	}
	m_orientationGroup->button(0)->setChecked(true);
	orientationGroupBox->setLayout(orientationLayout);
	orientationGroupBox->setEnabled(enablePositionAndOrientation);
	layout->addWidget(orientationGroupBox);

	m_worldSizeWidget = new SynGlyphX::DoubleSizeWidget(true, this);
	m_worldSizeWidget->SetRange(0.1, 10000.0);
	m_worldSizeWidget->SetSize(QSizeF(360.0, 180.0));
	SynGlyphX::GroupBoxSingleWidget* worldSizeGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("World Space Size"), m_worldSizeWidget, this);
	layout->addWidget(worldSizeGroupBox);

	layout->addStretch(1);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	layout->addWidget(dialogButtonBox);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &BaseImageDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &BaseImageDialog::reject);

	setLayout(layout);
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

		const SynGlyphX::DownloadedMapProperties* const properties = dynamic_cast<const SynGlyphX::DownloadedMapProperties* const>(baseImage.GetProperties());
		m_downloadedMapOptionsWidget->Set(properties->GetSource(), properties->GetType(), QSize(properties->GetSize()[0], properties->GetSize()[1]));
	}
	else if (baseImageType == SynGlyphX::BaseImage::Type::UserImage) {

		const SynGlyphX::UserDefinedBaseImageProperties* const properties = dynamic_cast<const SynGlyphX::UserDefinedBaseImageProperties* const>(baseImage.GetProperties());
		m_userDefinedImageLineEdit->SetText(QString::fromStdWString(properties->GetFilename()));
	}
	m_positionWidget->Set(baseImage.GetPosition());

	m_orientationGroup->button(s_orientationPresets.right.at(baseImage.GetRotationAngles()))->setChecked(true);

	SynGlyphX::BaseImage::Size worldSize = baseImage.GetWorldSize();
	m_worldSizeWidget->SetSize(QSizeF(worldSize[0], worldSize[1]));
}

SynGlyphX::BaseImage BaseImageDialog::GetBaseImage() const {

	SynGlyphX::BaseImage newBaseImage(nullptr);

	SynGlyphX::BaseImage::Type baseImageType = SynGlyphX::BaseImage::s_baseImageTypeStrings.right.at(m_baseImageComboBox->currentText().toStdWString());
	if (baseImageType == SynGlyphX::BaseImage::Type::DownloadedMap) {

		SynGlyphX::DownloadedMapProperties::Size imageSize;
		imageSize[0] = m_downloadedMapOptionsWidget->GetMapSize().width();
		imageSize[1] = m_downloadedMapOptionsWidget->GetMapSize().height();
		SynGlyphX::DownloadedMapProperties properties(m_downloadedMapOptionsWidget->GetMapSource(), m_downloadedMapOptionsWidget->GetMapType(), imageSize);
		newBaseImage = SynGlyphX::BaseImage(&properties);
	}
	else if (baseImageType == SynGlyphX::BaseImage::Type::UserImage) {

		SynGlyphX::UserDefinedBaseImageProperties properties(m_userDefinedImageLineEdit->GetText().toStdWString());
		newBaseImage = SynGlyphX::BaseImage(&properties);
	}

	newBaseImage.SetPosition(m_positionWidget->Get());
	newBaseImage.SetRotation(s_orientationPresets.left.at(m_orientationGroup->id(m_orientationGroup->checkedButton())));
	QSizeF worldSizeF = m_worldSizeWidget->GetSize();
	SynGlyphX::BaseImage::Size worldSize;
	worldSize[0] = worldSizeF.width();
	worldSize[1] = worldSizeF.height();
	newBaseImage.SetWorldSize(worldSize);

	return newBaseImage;
}