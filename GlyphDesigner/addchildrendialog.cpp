#include "addchildrendialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>

AddChildrenDialog::AddChildrenDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    m_childrenGlyphWidget = new SingleGlyphWidget(SingleGlyphWidget::ShowOnTop | SingleGlyphWidget::EnabledSpinBox, this);
    m_childrenGlyphWidget->SetNumberOfChildren(1);
    layout->addWidget(m_childrenGlyphWidget);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttonBox);

    setLayout(layout);
    setWindowTitle(tr("Add Children"));
}

AddChildrenDialog::~AddChildrenDialog()
{

}

void AddChildrenDialog::SetGlyphFromDialog(boost::shared_ptr<SynGlyphX::GlyphProperties> glyph) {

    m_childrenGlyphWidget->SetGlyphFromWidget(glyph);
}

void AddChildrenDialog::SetDialogFromGlyph(boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph) {

    m_childrenGlyphWidget->SetWidgetFromGlyph(glyph, true);
    m_childrenGlyphWidget->SetNumberOfChildren(1);
}

unsigned int AddChildrenDialog::GetNumberOfChildren() const {

    return m_childrenGlyphWidget->GetNumberOfChildren();
}