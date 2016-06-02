#include "changeimagefiledialog.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <algorithm>
#include "userdefinedbaseimageproperties.h"
#include "filesystem.h"
#include "browselineedit.h"
#include "baseimage.h"
#include "datatransformmapping.h"

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

	bool ChangeImageFileDialog::UpdateImageFiles(const std::vector<unsigned int>& images, const QString& sdtfilename, DataTransformMapping::SharedPtr mapping, QWidget* dialogParent) {

		QFileInfo fileInfo(sdtfilename);
		std::wstring sdtDir = QDir::toNativeSeparators(fileInfo.canonicalPath()).toStdWString();
		std::vector<std::pair<unsigned int, QString>> imagesThatNeedUpdate;

		for (unsigned int index : images) {

			const BaseImage& baseImage = mapping->GetBaseObjects()[index];
			if (baseImage.GetType() == BaseImage::Type::UserImage) {

				UserDefinedBaseImageProperties::ConstSharedPtr properties = std::dynamic_pointer_cast<const UserDefinedBaseImageProperties>(baseImage.GetProperties());
                std::wstring name = properties->GetFilename();
                std::replace(name.begin(), name.end(), L'\\', L'/');      // forward slash supported on all platforms
                
				std::wstring newImageFilename = Filesystem::IsFileInDirectory(name, sdtDir);
				if (!newImageFilename.empty()) {

					BaseImage newBaseImage = mapping->GetBaseObjects()[index];
					UserDefinedBaseImageProperties::SharedPtr newProperties = std::make_shared<UserDefinedBaseImageProperties>(newImageFilename);
					newBaseImage.SetProperties(newProperties);
					mapping->SetBaseObject(index, newBaseImage);
				}
				else {

					imagesThatNeedUpdate.push_back(std::pair<unsigned int, QString>(index, QString::fromStdWString(properties->GetFilename())));
				}
			}
		}

		for (const auto& imageAndIndex : imagesThatNeedUpdate) {

			QString acceptButtonText = tr("Next");
			if (imageAndIndex == *imagesThatNeedUpdate.rbegin()) {

				acceptButtonText = tr("Ok");
			}

			ChangeImageFileDialog dialog(imageAndIndex.second, acceptButtonText, dialogParent);
			dialog.setWindowTitle(tr("Replace Missing Base Image"));
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

	bool ChangeImageFileDialog::UpdateLegendFiles(const std::vector<unsigned int>& legends, const QString& sdtfilename, DataTransformMapping::SharedPtr mapping, QWidget* dialogParent) {

		QFileInfo fileInfo(sdtfilename);
		std::wstring sdtDir = QDir::toNativeSeparators(fileInfo.canonicalPath()).toStdWString();
		std::vector<std::pair<unsigned int, QString>> imagesThatNeedUpdate;

		for (unsigned int index : legends) {

			const Legend& legend = mapping->GetLegends()[index];
            std::wstring name = legend.GetFilename();
            std::replace(name.begin(), name.end(), L'\\', L'/');      // forward slash supported on all platforms

			std::wstring newLegendFilename = Filesystem::IsFileInDirectory(name, sdtDir);
			if (!newLegendFilename.empty()) {

				Legend newLegend = mapping->GetLegends()[index];
				newLegend.SetFilename(newLegendFilename);
				mapping->SetLegend(index, newLegend);
			}
			else {

				imagesThatNeedUpdate.push_back(std::pair<unsigned int, QString>(index, QString::fromStdWString(legend.GetFilename())));
			}
		}

		for (const auto& imageAndIndex : imagesThatNeedUpdate) {

			QString acceptButtonText = tr("Next");
			if (imageAndIndex == *imagesThatNeedUpdate.rbegin()) {

				acceptButtonText = tr("Ok");
			}

			ChangeImageFileDialog dialog(imageAndIndex.second, acceptButtonText, dialogParent);
			dialog.setWindowTitle(tr("Replace Missing Legend"));
			if (dialog.exec() == QDialog::Accepted) {

				Legend newLegend = mapping->GetLegends()[imageAndIndex.first];
				newLegend.SetFilename(dialog.GetNewFilename().toStdWString());
				mapping->SetLegend(imageAndIndex.first, newLegend);
			}
			else {

				return false;
			}
		}

		return true;
	}


} //namespace SynGlyphX