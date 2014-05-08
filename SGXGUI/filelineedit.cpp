#include "filelineedit.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFileDialog>

namespace SynGlyphX {

FileLineEdit::FileLineEdit(const QString& filters, QWidget *parent)
    : QWidget(parent),
    m_filters(filters)
{
    m_lineEdit = new QLineEdit(this);

    m_fileBrowserButton = new QPushButton("Browse", this);

    QObject::connect(m_fileBrowserButton, SIGNAL(clicked()), this, SLOT(FileBrowserButtonActivated()));

    QHBoxLayout* layout = new QHBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(m_lineEdit, 1);
    layout->addWidget(m_fileBrowserButton, 0);

    setLayout(layout);
}

FileLineEdit::~FileLineEdit()
{

}

void FileLineEdit::SetFilename(const QString& filename) {
    
    m_lineEdit->setText(filename);
}

QString FileLineEdit::GetFilename() const {

    return m_lineEdit->text();
}

void FileLineEdit::FileBrowserButtonActivated() {

    QString filename = QFileDialog::getOpenFileName(this, tr("Find File"), m_lineEdit->text(), m_filters);
    if (!filename.isEmpty()) {
        m_lineEdit->setText(QDir::toNativeSeparators(filename));
    }
}

} //namespace SynGlyphX
