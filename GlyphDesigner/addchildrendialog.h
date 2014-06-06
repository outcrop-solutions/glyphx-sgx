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

    void SetGlyphFromDialog(boost::shared_ptr<SynGlyphX::GlyphProperties> glyph);

    unsigned int GetNumberOfChildren() const;

public slots:
    void SetDialogFromGlyph(boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph);

private:
    SingleGlyphWidget* m_childrenGlyphWidget;
};

#endif // ADDCHILDRENDIALOG_H
