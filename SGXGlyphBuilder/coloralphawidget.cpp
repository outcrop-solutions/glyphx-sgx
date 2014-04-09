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

    QLabel* alphaLabel = new QLabel(tr("Alpha:"), this);

    m_alphaSpinBox = new QSpinBox(this);
    m_alphaSpinBox->setRange(0, 255);
    QObject::connect(m_alphaSpinBox, SIGNAL(valueChanged(int)), this, SLOT(OnAlphaSpinBoxChanged(int)));

    layout->addWidget(m_button);
    layout->addWidget(alphaLabel);
    layout->addWidget(m_alphaSpinBox);
    layout->addStretch(1);

    setLayout(layout);

    SetColor(Qt::gray);
}

ColorAlphaWidget::~ColorAlphaWidget()
{

}

void ColorAlphaWidget::SetColor(const QColor& color) {

    m_color = color;
    UpdateButtonColor();
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

void ColorAlphaWidget::UpdateButtonColor() {

    QColor color = m_color;
    color.setAlpha(255);
    QPixmap pixmap(m_button->iconSize());
    pixmap.fill(color);
    m_button->setIcon(QIcon(pixmap));
}

void ColorAlphaWidget::OnAlphaSpinBoxChanged(int value) {
    m_color.setAlpha(value);
}

void ColorAlphaWidget::SetColor(const SynGlyphX::Color& color) {
    
    m_color.setRed(color[0]);
    m_color.setGreen(color[1]);
    m_color.setBlue(color[2]);
    //m_color.setAlpha(color[3]);

    UpdateButtonColor();
    m_alphaSpinBox->setValue(color[3]);
}

const QColor& ColorAlphaWidget::GetColor() const {
    return m_color;
}

SynGlyphX::Color ColorAlphaWidget::ConvertQColorToColor(const QColor& qColor) {

    SynGlyphX::Color color;

    color[0] = qColor.red();
    color[1] = qColor.green();
    color[2] = qColor.blue();
    color[3] = qColor.alpha();

    return color;
}