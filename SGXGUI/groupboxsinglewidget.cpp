#include "groupboxsinglewidget.h"
#include <QtWidgets/QHBoxLayout>

namespace SynGlyphX {

    GroupBoxSingleWidget::GroupBoxSingleWidget(const QString& title, QWidget* widget, QWidget *parent)
        : QGroupBox(title, parent)
    {
        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->addWidget(widget);
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
    }

    GroupBoxSingleWidget::~GroupBoxSingleWidget()
    {

    }

} //namespace SynGlyphX