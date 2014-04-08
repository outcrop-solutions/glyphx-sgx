#include "xyzwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

XYZWidget::XYZWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    QLabel* labelX = new QLabel(tr("X: "), this);
    m_xSpinBox = new QDoubleSpinBox(this);
    QLabel* labelY = new QLabel(tr("Y: "), this);
    m_ySpinBox = new QDoubleSpinBox(this);
    QLabel* labelZ = new QLabel(tr("Z: "), this);
    m_zSpinBox = new QDoubleSpinBox(this);

    layout->addWidget(labelX);
    layout->addWidget(m_xSpinBox);
    layout->addStretch(1);
    layout->addWidget(labelY);
    layout->addWidget(m_ySpinBox);
    layout->addStretch(1);
    layout->addWidget(labelZ);
    layout->addWidget(m_zSpinBox);

    setLayout(layout);
}

XYZWidget::~XYZWidget()
{

}

void XYZWidget::SetRange(double min, double max) {
    m_xSpinBox->setRange(min, max);
    m_ySpinBox->setRange(min, max);
    m_zSpinBox->setRange(min, max);
}

void XYZWidget::SetX(double value){
    m_xSpinBox->setValue(value);
}

double XYZWidget::GetX() const{
    return m_xSpinBox->value();
}

void XYZWidget::SetY(double value){
    m_ySpinBox->setValue(value);
}

double XYZWidget::GetY() const{
    return m_ySpinBox->value();
}

void XYZWidget::SetZ(double value){
    m_zSpinBox->setValue(value);
}

double XYZWidget::GetZ() const{
    return m_zSpinBox->value();
}

void XYZWidget::SetWrapping(bool wrapping) {
    m_xSpinBox->setWrapping(wrapping);
    m_ySpinBox->setWrapping(wrapping);
    m_zSpinBox->setWrapping(wrapping);
}