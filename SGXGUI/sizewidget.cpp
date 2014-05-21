#include "sizewidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

namespace SynGlyphX {

    SizeWidget::SizeWidget(QWidget *parent)
        : QWidget(parent)
    {
        QHBoxLayout* layout = new QHBoxLayout(this);

        QLabel* widthLabel = new QLabel(tr("Width:"), this);
        m_widthSpinBox = new QSpinBox(this);
        QLabel* heightLabel = new QLabel(tr("Height:"), this);
        m_heightSpinBox = new QSpinBox(this);

        layout->addWidget(widthLabel);
        layout->addWidget(m_widthSpinBox);
        layout->addWidget(heightLabel);
        layout->addWidget(m_heightSpinBox);

        setContentsMargins(0, 0, 0, 0);
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
    }

    SizeWidget::~SizeWidget()
    {

    }

    void SizeWidget::SetSize(const QSize& size) {

        m_widthSpinBox->setValue(size.width());
        m_heightSpinBox->setValue(size.height());
    }

    QSize SizeWidget::GetSize() const {

        QSize size(m_widthSpinBox->value(), m_heightSpinBox->value());
        return size;
    }

    void SizeWidget::SetRange(int min, int max) {

        m_widthSpinBox->setRange(min, max);
        m_heightSpinBox->setRange(min, max);
    }

} //namespace SynGlyphX