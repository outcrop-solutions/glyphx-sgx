#ifndef SINGLEGLYPHDIALOG_H
#define SINGLEGLYPHDIALOG_H

#include <QtWidgets/QDialog>
#include "ui_singleglyphdialog.h"

class SingleGlyphDialog : public QDialog
{
    Q_OBJECT

public:
    SingleGlyphDialog(QWidget *parent = 0);
    ~SingleGlyphDialog();

private:
    Ui::SingleGlyphDialog ui;
};

#endif // SINGLEGLYPHDIALOG_H
