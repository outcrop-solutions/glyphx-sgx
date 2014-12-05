#ifndef XYZWIDGET_H
#define XYZWIDGET_H

#include "sgxgui_global.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QCheckBox>
#include "foundationtypes.h"

class SGXGUI_EXPORT XYZWidget : public QWidget
{
    Q_OBJECT

public:
    XYZWidget(bool includeLockCheckbox = false, QWidget *parent = 0);
    ~XYZWidget();

    void SetRange(double min, double max);

    void SetX(double value);
    double GetX() const;
    void SetY(double value);
    double GetY() const;
    void SetZ(double value);
    double GetZ() const;

    void Set(const SynGlyphX::Vector3& vec);
	SynGlyphX::Vector3 Get() const;

    void SetWrapping(bool wrapping);
    void SetDecimal(int decimals);
    void SetSuffix(const QString& suffix);

public slots:
    void SetSpinBoxesLocked(bool lock);

signals:
    void ValuesChanged(double x, double y, double z);

private slots:
    void OnXSpinBoxChanged();
    void OnYSpinBoxChanged();
    void OnZSpinBoxChanged();
    void UpdateSpinBoxLock(bool lock);

private:
    void SetSpinBoxWithoutSignals(QDoubleSpinBox* spinBox, double value);

    QDoubleSpinBox* m_xSpinBox;
    QDoubleSpinBox* m_ySpinBox;
    QDoubleSpinBox* m_zSpinBox;
    QCheckBox* m_lockCheckBox;
    bool m_spinBoxesLocked;
};

#endif // XYZWIDGET_H
