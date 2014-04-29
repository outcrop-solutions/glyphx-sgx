#ifndef KMLTOCSVDIALOG_H
#define KMLTOCSVDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>

class KMLToCSVDialog : public QDialog
{
    Q_OBJECT

public:
    KMLToCSVDialog(QWidget *parent = 0);
    ~KMLToCSVDialog();

private:
    QLineEdit* m_inputKML;
    QLineEdit* m_inputGlyph;
};

#endif // KMLTOCSVDIALOG_H
