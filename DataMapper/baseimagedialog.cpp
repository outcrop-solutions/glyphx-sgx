#include "baseimagedialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <QtGui/QImage>
#include "groupboxsinglewidget.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"

BaseImageDialog::BaseImageDialog(QWidget *parent)
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
}

SynGlyphX::BaseImage BaseImageDialog::GetBaseImage() const {

	SynGlyphX::BaseImage::Type baseImageType = SynGlyphX::BaseImage::s_baseImageTypeStrings.right.at(m_baseImageComboBox->currentText().toStdWString());
	if (baseImageType == SynGlyphX::BaseImage::Type::DownloadedMap) {

		SynGlyphX::DownloadedMapProperties::Size imageSize;
		imageSize[0] = m_downloadedMapOptionsWidget->GetMapSize().width();
		imageSize[1] = m_downloadedMapOptionsWidget->GetMapSize().height();
		SynGlyphX::DownloadedMapProperties properties(m_downloadedMapOptionsWidget->GetMapSource(), m_downloadedMapOptionsWidget->GetMapType(), imageSize);
		return SynGlyphX::BaseImage(&properties);
	}
	else if (baseImageType == SynGlyphX::BaseImage::Type::UserImage) {

		SynGlyphX::UserDefinedBaseImageProperties properties(m_userDefinedImageLineEdit->GetText().toStdWString());
		return SynGlyphX::BaseImage(&properties);
	}

	return SynGlyphX::BaseImage(nullptr);
}
