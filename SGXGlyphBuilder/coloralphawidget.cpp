#include "coloralphawidget.h"
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>

ColorAlphaWidget::ColorAlphaWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    m_button = new QPushButton(this);
    m_button->setAutoFillBackground(true);
    QObject::connect(m_button, SIGNAL(clicked()), this, SLOT(OnButtonClicked()));

    QLabel* alphaLabel = new QLabel(tr("Alpha: "), this);

    m_alphaSpinBox = new QSpinBox(this);
    m_alphaSpinBox->setRange(0, 255);
    QObject::connect(m_alphaSpinBox, SIGNAL(valueChanged(int)), this, SLOT(OnAlphaSpinBoxChanged(int)));

    layout->addWidget(m_button);
    layout->addStretch(1);
    layout->addWidget(alphaLabel);
    layout->addWidget(m_alphaSpinBox);

    setLayout(layout);

    SetColor(Qt::white);
}

ColorAlphaWidget::~ColorAlphaWidget()
{

}

void ColorAlphaWidget::SetColor(const QColor& color) {

    m_color = color;
    UpdateButtonPalette();
    m_alphaSpinBox->setValue(color.alpha());
}

void ColorAlphaWidget::OnButtonClicked() {

    QColorDialog colorDialog(this);
    colorDialog.setOption(QColorDialog::ShowAlphaChannel);
    colorDialog.setCurrentColor(m_color);
    if (colorDialog.exec() == QDialog::Accepted) {
        SetColor(colorDialog.currentColor());
    }
}

void ColorAlphaWidget::UpdateButtonPalette() {

    QColor color = m_color;
    color.setAlpha(255);
    QPalette palette(color);
    m_button->setPalette(palette);
}

void ColorAlphaWidget::OnAlphaSpinBoxChanged(int value) {
    m_color.setAlpha(value);
}