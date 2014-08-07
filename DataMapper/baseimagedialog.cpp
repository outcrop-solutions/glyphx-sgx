#include "baseimagedialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include "groupboxsinglewidget.h"
#include "downloadedmapproperties.h"

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

	//Replace with BrowseLineEdit when UserImage type is implemented
	QWidget* userImageWidget = new QWidget(m_baseImageOptionsStackedWidget);
	m_baseImageOptionsStackedWidget->addWidget(userImageWidget);

	layout->addWidget(m_baseImageOptionsStackedWidget);

	QObject::connect(m_baseImageComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), m_baseImageOptionsStackedWidget, &QStackedWidget::setCurrentIndex);

	layout->addStretch(1);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	layout->addWidget(dialogButtonBox);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &BaseImageDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &BaseImageDialog::reject);

	setLayout(layout);
	setWindowTitle(tr("Choose Base Image"));
}

BaseImageDialog::~BaseImageDialog()
{

}

void BaseImageDialog::SetBaseImage(const SynGlyphX::BaseImage& baseImage) {

	SynGlyphX::BaseImage::Type baseImageType = baseImage.GetType();
	m_baseImageComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::BaseImage::s_baseImageTypeStrings.left.at(baseImageType)));
	if (baseImageType == SynGlyphX::BaseImage::Type::DownloadedMap) {

		const SynGlyphX::DownloadedMapProperties* const properties = dynamic_cast<const SynGlyphX::DownloadedMapProperties* const>(baseImage.GetProperties());
		m_downloadedMapOptionsWidget->Set(properties->GetSource(), properties->GetType(), QSize(properties->GetSize()[0], properties->GetSize()[1]));
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

	return SynGlyphX::BaseImage(nullptr);
}
