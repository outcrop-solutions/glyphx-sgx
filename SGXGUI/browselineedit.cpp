#include "browselineedit.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFileDialog>

namespace SynGlyphX {

BrowseLineEdit::BrowseLineEdit(bool convertToNativeSeparators, QWidget *parent)
    : QWidget(parent),
    m_convertToNativeSeparators(convertToNativeSeparators)
{
    m_lineEdit = new QLineEdit(this);

    m_browseButton = new QPushButton("Browse", this);

    QObject::connect(m_browseButton, SIGNAL(clicked()), this, SLOT(BrowseButtonActivated()));

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

 void BrowseLineEdit::BrowseButtonActivated() {

    QString text;
    if (m_convertToNativeSeparators) {
        text = QDir::toNativeSeparators(GetFromDialog());
    }
    else {
        text = GetFromDialog();
    }
    
    if ((!text.isEmpty()) && (text != m_lineEdit->text())) {
        m_lineEdit->setText(text);

        emit TextChanged(text);
    }
}

} //namespace SynGlyphX
