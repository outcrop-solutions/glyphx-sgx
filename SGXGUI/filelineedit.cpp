#include "filelineedit.h"
#include <QtWidgets/QFileDialog>

namespace SynGlyphX {

FileLineEdit::FileLineEdit(const QString& filters, QWidget *parent)
    : BrowseLineEdit(true, parent),
    m_filters(filters)
{
    
}

FileLineEdit::~FileLineEdit()
{

}

QString FileLineEdit::GetFromDialog() {

    QString dir = m_initialBrowseDirectory;
    if (!m_lineEdit->text().isEmpty()) {
        dir = m_lineEdit->text();
    }
    return QFileDialog::getOpenFileName(this, tr("Find File"), dir, m_filters);
}

void FileLineEdit::SetInitalBrowseDirectory(const QString& dir) {

    m_initialBrowseDirectory = dir;
}

} //namespace SynGlyphX
