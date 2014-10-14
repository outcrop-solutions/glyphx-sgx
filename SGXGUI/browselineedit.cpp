#include "browselineedit.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFileDialog>

namespace SynGlyphX {

BrowseLineEdit::BrowseLineEdit(FileDialogType type, bool convertToNativeSeparators, QWidget *parent)
    : QWidget(parent),
    m_dialogType(type),
    m_convertToNativeSeparators(convertToNativeSeparators),
    m_filters(QString())
{
    m_lineEdit = new QLineEdit(this);
	m_lineEdit->setMinimumWidth(350);
    QObject::connect(m_lineEdit, &QLineEdit::editingFinished, this, &BrowseLineEdit::OnEditingFinished);

    m_browseButton = new QPushButton("Browse", this);
    QObject::connect(m_browseButton, &QPushButton::clicked, this, &BrowseLineEdit::BrowseButtonActivated);

    QHBoxLayout* layout = new QHBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(m_lineEdit, 1);
    layout->addWidget(m_browseButton, 0);

    setLayout(layout);
}

BrowseLineEdit::~BrowseLineEdit()
{

}

void BrowseLineEdit::SetText(const QString& text) {
    
    if (m_convertToNativeSeparators) {
        m_lineEdit->setText(QDir::toNativeSeparators(text));
    }
    else {
        m_lineEdit->setText(text);
    }
}

QString BrowseLineEdit::GetText() const {

    return m_lineEdit->text();
}

void BrowseLineEdit::SetInitalBrowseDirectory(const QString& dir) {

     m_initialBrowseDirectory = dir;
 }

void BrowseLineEdit::BrowseButtonActivated() {

    QString text;
    QString initialDir;
    if (m_lineEdit->text().isEmpty()) {
        initialDir = m_initialBrowseDirectory;
    }
    else {
        initialDir = m_lineEdit->text();
    }

    if (m_dialogType == FileOpen) {
        text = QFileDialog::getOpenFileName(this, tr("Open File"), initialDir, m_filters);
    }
    else if (m_dialogType == FileSave) {
        text = QFileDialog::getSaveFileName(this, tr("Save File"), initialDir, m_filters);
    } 
    else if (m_dialogType == Directory) {
        text = QFileDialog::getExistingDirectory(this, tr("Select Directory"), initialDir);
    }

    if (!text.isEmpty()) {

        if (m_convertToNativeSeparators) {

            text = QDir::toNativeSeparators(text);
        }
        if (text != m_lineEdit->text()) {
            
            m_lineEdit->setText(text);
            emit TextChanged(text);
        }
    }
}

void BrowseLineEdit::OnEditingFinished() {

    if (m_lineEdit->isModified()) {

        m_lineEdit->setModified(false);
        emit TextChanged(m_lineEdit->text());
    }
}

void BrowseLineEdit::SetFilters(const QString& filters) {

    m_filters = filters;
}

} //namespace SynGlyphX
