#include "directorylineedit.h"
#include <QtWidgets/QFileDialog>

namespace SynGlyphX {

DirectoryLineEdit::DirectoryLineEdit(QWidget *parent)
    : BrowseLineEdit(true, parent)
{
    
}

DirectoryLineEdit::~DirectoryLineEdit()
{

}

QString DirectoryLineEdit::GetFromDialog() {

    return QFileDialog::getExistingDirectory(this, tr("Find Directory"), m_lineEdit->text());
}

} //namespace SynGlyphX
