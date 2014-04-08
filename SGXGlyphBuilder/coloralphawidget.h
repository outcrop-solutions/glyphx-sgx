#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

class ColorAlphaWidget : public QWidget
{
    Q_OBJECT

public:
    ColorAlphaWidget(QWidget *parent = 0);
    ~ColorAlphaWidget();

    void SetColor(const QColor& color);
    const QColor& GetColor() const;

private slots:
    void OnButtonClicked();
    void OnAlphaSpinBoxChanged(int value);

private:
    void UpdateButtonPalette();

    QColor m_color;
    QPushButton* m_button;
    QSpinBox* m_alphaSpinBox;
};

#endif // COLORBUTTON_H
