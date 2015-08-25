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

		bool wereAnyImagesUpdated = false;
		for (unsigned int index : images) {

			QString acceptButtonText = tr("Next");
			if (index == *images.rbegin()) {

				acceptButtonText = tr("Ok");
			}

			BaseImage baseImage = mapping->GetBaseObjects()[index];
			UserDefinedBaseImageProperties::ConstSharedPtr properties = std::dynamic_pointer_cast<const UserDefinedBaseImageProperties>(baseImage.GetProperties());

			ChangeImageFileDialog dialog(QString::fromStdWString(properties->GetFilename()), acceptButtonText, dialogParent);
			if (dialog.exec() == QDialog::Accepted) {

				UserDefinedBaseImageProperties::SharedPtr newProperties = std::make_shared<UserDefinedBaseImageProperties>(dialog.GetNewFilename().toStdWString());
				baseImage.SetProperties(newProperties);
				mapping->SetBaseObject(index, baseImage);
				wereAnyImagesUpdated = true;
			}
			else {

				throw std::exception("One or more base images weren't found.");
			}
		}

		return wereAnyImagesUpdated;
	}

} //namespace SynGlyphX