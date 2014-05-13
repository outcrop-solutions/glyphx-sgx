#ifndef ADDCHILDRENDIALOG_H
#define ADDCHILDRENDIALOG_H

#include <QtWidgets/QDialog>
#include "singleglyphwidget.h"

class AddChildrenDialog : public QDialog
{
    Q_OBJECT

public:
    AddChildrenDialog(QWidget *parent = 0);
    ~AddChildrenDialog();

    void SetGlyphFromDialog(boost::shared_ptr<SynGlyphX::Glyph> glyph);

public slots:
    void SetDialogFromGlyph(boost::shared_ptr<const SynGlyphX::Glyph> glyph);

private:
    SingleGlyphWidget* m_childrenGlyphWidget;
};

#endif // ADDCHILDRENDIALOG_H
