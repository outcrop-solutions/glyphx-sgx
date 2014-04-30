#ifndef KMLTOCSVDIALOG_H
#define KMLTOCSVDIALOG_H

#include <QtWidgets/QDialog>
#include "filelineedit.h"

class KMLToCSVDialog : public QDialog
{
    Q_OBJECT

public:
    KMLToCSVDialog(QWidget *parent = 0);
    ~KMLToCSVDialog();

    QString GetKMLFilename() const;
    QString GetCSVFilename() const;

private slots:
    virtual void accept();

private:
    SynGlyphX::FileLineEdit* m_inputKML;
    SynGlyphX::FileLineEdit* m_inputGlyph;
};

#endif // KMLTOCSVDIALOG_H
