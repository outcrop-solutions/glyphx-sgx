#include "changeimagefiledialog.h"
#include <QtCore/QFileInfo>
#include "userdefinedbaseimageproperties.h"

namespace SynGlyphX {

	ChangeImageFileDialog::ChangeImageFileDialog(const QString& oldFileName, const QString& acceptButtonText, QWidget *parent)
		: ReplaceFilenameDialog(oldFileName, acceptButtonText, parent)
	{
		QFileInfo fileInfo(oldFileName);
		m_fileExtension = fileInfo.suffix();
	}

	ChangeImageFileDialog::~ChangeImageFileDialog()
	{

	}

	bool ChangeImageFileDialog::IsNewFileValid() const {

		QFileInfo newFileInfo(m_newDatasourceFileLineEdit->GetText());
		return (m_fileExtension == newFileInfo.suffix());
	}

	bool ChangeImageFileDialog::UpdateImageFiles(const std::vector<unsigned int>& images, DataTransformMapping::SharedPtr mapping, QWidget* dialogParent) {

		std::vector<std::pair<unsigned int, QString>> imagesThatNeedUpdate;
		for (unsigned int index : images) {

			const BaseImage& baseImage = mapping->GetBaseObjects()[index];
			if (baseImage.GetType() == BaseImage::Type::UserImage) {

				UserDefinedBaseImageProperties::ConstSharedPtr properties = std::dynamic_pointer_cast<const UserDefinedBaseImageProperties>(baseImage.GetProperties());
				QString imageFilename = QString::fromStdWString(properties->GetFilename());
				if (!QFile::exists(imageFilename)) {

					imagesThatNeedUpdate.push_back(std::pair<unsigned int, QString>(index, imageFilename));
				}
			}
		}

		for (const auto& imageAndIndex : imagesThatNeedUpdate) {

			QString acceptButtonText = tr("Next");
			if (imageAndIndex == *imagesThatNeedUpdate.rbegin()) {

				acceptButtonText = tr("Ok");
			}

			ChangeImageFileDialog dialog(imageAndIndex.second, acceptButtonText, dialogParent);
			if (dialog.exec() == QDialog::Accepted) {

				BaseImage baseImage = mapping->GetBaseObjects()[imageAndIndex.first];
				UserDefinedBaseImageProperties::SharedPtr newProperties = std::make_shared<UserDefinedBaseImageProperties>(dialog.GetNewFilename().toStdWString());
				baseImage.SetProperties(newProperties);
				mapping->SetBaseObject(imageAndIndex.first, baseImage);
			}
			else {

				return false;
			}
		}

		return true;
	}

} //namespace SynGlyphX