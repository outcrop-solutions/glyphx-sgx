#ifndef SIZEWIDGET_H
#define SIZEWIDGET_H

#include "sgxgui_global.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QSpinBox>

namespace SynGlyphX {

    class SGXGUI_EXPORT SizeWidget : public QWidget
    {
        Q_OBJECT

    public:
        SizeWidget(QWidget *parent = 0);
        ~SizeWidget();

        void SetSize(const QSize& size);
        QSize GetSize() const;

        void SetRange(int min, int max);

    private:
        QSpinBox* m_widthSpinBox;
        QSpinBox* m_heightSpinBox;
    };

} //namespace SynGlyphX

#endif // SIZEWIDGET_H
