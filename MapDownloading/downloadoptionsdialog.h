#ifndef DOWNLOADOPTIONSDIALOG_H
#define DOWNLOADOPTIONSDIALOG_H

#include "mapdownloading_global.h"
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>

class MAPDOWNLOADING_EXPORT DownloadOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    DownloadOptionsDialog(QWidget *parent = 0);
    ~DownloadOptionsDialog();

public slots:
    virtual void accept();

private:
    QLineEdit* m_mapquestKeyLineEdit;
    QLineEdit* m_googleMapsKeyLineEdit;
    QCheckBox* m_addPointsToMap;
};

#endif // DOWNLOADOPTIONSDIALOG_H
