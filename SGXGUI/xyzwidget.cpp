#include "xyzwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

XYZWidget::XYZWidget(bool includeLockCheckbox, QWidget *parent)
    : QWidget(parent),
    m_lockCheckBox(NULL),
    m_spinBoxesLocked(false)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    if (includeLockCheckbox) {
        m_lockCheckBox = new QCheckBox(tr("Lock"), this);
        layout->addWidget(m_lockCheckBox);
        QObject::connect(m_lockCheckBox, &QCheckBox::toggled, this, &XYZWidget::UpdateSpinBoxLock);
    }

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

    setLayout(layout);

    QObject::connect(m_xSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &XYZWidget::OnXSpinBoxChanged);
    QObject::connect(m_ySpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &XYZWidget::OnYSpinBoxChanged);
    QObject::connect(m_zSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &XYZWidget::OnZSpinBoxChanged);
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

SynGlyphX::Vector3 XYZWidget::Get() const {

	SynGlyphX::Vector3 vec;
	vec[0] = m_xSpinBox->value();
	vec[1] = m_ySpinBox->value();
	vec[2] = m_zSpinBox->value();

	return vec;
}

void XYZWidget::SetSuffix(const QString& suffix) {

    m_xSpinBox->setSuffix(suffix);
    m_ySpinBox->setSuffix(suffix);
    m_zSpinBox->setSuffix(suffix);
}

void XYZWidget::OnXSpinBoxChanged() {
    
    //If locked keep all spin boxes the same value
    if (m_spinBoxesLocked) {
        SetSpinBoxWithoutSignals(m_ySpinBox, m_xSpinBox->value());
        SetSpinBoxWithoutSignals(m_zSpinBox, m_xSpinBox->value());
    }

    emit ValuesChanged(m_xSpinBox->value(), m_ySpinBox->value(), m_zSpinBox->value());
}

void XYZWidget::OnYSpinBoxChanged() {

    //If locked keep all spin boxes the same value
    if (m_spinBoxesLocked) {
        SetSpinBoxWithoutSignals(m_xSpinBox, m_ySpinBox->value());
        SetSpinBoxWithoutSignals(m_zSpinBox, m_ySpinBox->value());
    }
    
    emit ValuesChanged(m_xSpinBox->value(), m_ySpinBox->value(), m_zSpinBox->value());
}

void XYZWidget::OnZSpinBoxChanged() {

    //If locked keep all spin boxes the same value
    if (m_spinBoxesLocked) {
        SetSpinBoxWithoutSignals(m_xSpinBox, m_zSpinBox->value());
        SetSpinBoxWithoutSignals(m_ySpinBox, m_zSpinBox->value());
    }
    
    emit ValuesChanged(m_xSpinBox->value(), m_ySpinBox->value(), m_zSpinBox->value());
}

void XYZWidget::SetSpinBoxWithoutSignals(QDoubleSpinBox* spinBox, double value) {

    spinBox->blockSignals(true);
    spinBox->setValue(value);
    spinBox->blockSignals(false);
}

void XYZWidget::SetSpinBoxesLocked(bool lock) {

    if (m_lockCheckBox != NULL) {
        m_lockCheckBox->setChecked(lock);
    }
    else {
        UpdateSpinBoxLock(lock);
    }
}

void XYZWidget::UpdateSpinBoxLock(bool lock) {

    m_spinBoxesLocked = lock;

    if (m_spinBoxesLocked) {

        OnXSpinBoxChanged();
    }
}