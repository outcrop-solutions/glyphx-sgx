#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include "foundationtypes.h"

class ColorAlphaWidget : public QWidget
{
    Q_OBJECT

public:
    ColorAlphaWidget(QWidget *parent = 0);
    ~ColorAlphaWidget();

    void SetColor(const SynGlyphX::Color& color);
    void SetColor(const QColor& color);
    const QColor& GetColor() const;
    
    static SynGlyphX::Color ConvertQColorToColor(const QColor& qColor);

signals:
    void ColorChanged(const QColor& color);

private slots:
    void OnButtonClicked();
    void OnAlphaSpinBoxChanged(int value);

private:
    void UpdateButtonColor();

    QColor m_color;
    QPushButton* m_button;
    QSpinBox* m_alphaSpinBox;
};

#endif // COLORBUTTON_H
