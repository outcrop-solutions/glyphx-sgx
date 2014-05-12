#include "xyzwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

XYZWidget::XYZWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    QLabel* labelX = new QLabel(tr("X:"), this);
    m_xSpinBox = new QDoubleSpinBox(this);
    m_xSpinBox->setKeyboardTracking(false);
    
    QLabel* labelY = new QLabel(tr("Y:"), this);
    m_ySpinBox = new QDoubleSpinBox(this);
    m_ySpinBox->setKeyboardTracking(false);
    
    QLabel* labelZ = new QLabel(tr("Z:"), this);
    m_zSpinBox = new QDoubleSpinBox(this);
    m_zSpinBox->setKeyboardTracking(false);

    layout->addWidget(labelX);
    layout->addWidget(m_xSpinBox, 1);
    
    layout->addWidget(labelY);
    layout->addWidget(m_ySpinBox, 1);
    
    layout->addWidget(labelZ);
    layout->addWidget(m_zSpinBox, 1);

    //layout->setSizeConstraint(QLayout::SetFixedSize);
    //setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    setLayout(layout);

    QObject::connect(m_xSpinBox, SIGNAL(valueChanged(double)), this, SLOT(OnSpinBoxesChanged()));
    QObject::connect(m_ySpinBox, SIGNAL(valueChanged(double)), this, SLOT(OnSpinBoxesChanged()));
    QObject::connect(m_zSpinBox, SIGNAL(valueChanged(double)), this, SLOT(OnSpinBoxesChanged()));
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

void XYZWidget::SetDecimal(int decimals) {
    m_xSpinBox->setDecimals(decimals);
    m_ySpinBox->setDecimals(decimals);
    m_zSpinBox->setDecimals(decimals);
}

void XYZWidget::Set(const SynGlyphX::Vector3& vec) {

    m_xSpinBox->setValue(vec[0]);
    m_ySpinBox->setValue(vec[1]);
    m_zSpinBox->setValue(vec[2]);
}

void XYZWidget::SetSuffix(const QString& suffix) {

    m_xSpinBox->setSuffix(suffix);
    m_ySpinBox->setSuffix(suffix);
    m_zSpinBox->setSuffix(suffix);
}

void XYZWidget::OnSpinBoxesChanged() {

    emit ValuesChanged(m_xSpinBox->value(), m_ySpinBox->value(), m_zSpinBox->value());
}