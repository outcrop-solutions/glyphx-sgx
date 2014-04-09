#ifndef XYZWIDGET_H
#define XYZWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QDoubleSpinBox>
#include "typedefs.h"

class XYZWidget : public QWidget
{
    Q_OBJECT

public:
    XYZWidget(QWidget *parent = 0);
    ~XYZWidget();

    void SetRange(double min, double max);

    void SetX(double value);
    double GetX() const;
    void SetY(double value);
    double GetY() const;
    void SetZ(double value);
    double GetZ() const;

    void Set(const SynGlyphX::Vector3& vec);

    void SetWrapping(bool wrapping);
    void SetDecimal(int decimals);

private:
    QDoubleSpinBox* m_xSpinBox;
    QDoubleSpinBox* m_ySpinBox;
    QDoubleSpinBox* m_zSpinBox;
};

#endif // XYZWIDGET_H
