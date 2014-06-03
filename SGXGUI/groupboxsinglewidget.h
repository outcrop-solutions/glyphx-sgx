#ifndef GROUPBOXSINGLEWIDGET_H
#define GROUPBOXSINGLEWIDGET_H

#include "sgxgui_global.h"
#include <QtWidgets/QGroupBox>

namespace SynGlyphX {

    class SGXGUI_EXPORT GroupBoxSingleWidget : public QGroupBox
    {
        Q_OBJECT

    public:
        GroupBoxSingleWidget(const QString& title, QWidget* widget, QWidget *parent = 0);
        ~GroupBoxSingleWidget();

    private:

    };

} //namespace SynGlyphX

#endif // GROUPBOXSINGLEWIDGET_H
