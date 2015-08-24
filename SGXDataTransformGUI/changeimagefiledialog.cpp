#include "changeimagefiledialog.h"
#include <QtCore/QFileInfo>

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

} //namespace SynGlyphX