#ifndef MAPGRABBERDIALOG_H
#define MAPGRABBERDIALOG_H

#include <QtWidgets/QDialog>
#include "browselineedit.h"
#include "mapoptionswidget.h"

class MapGrabberDialog : public QDialog
{
    Q_OBJECT

public:
    MapGrabberDialog(QWidget *parent = 0);
    ~MapGrabberDialog();

private slots:
    virtual void accept();
    virtual void reject();
    void OnMapServiceSettingsButtonClicked();

private:
    void ReadSettings();
    void WriteSettings();

    MapOptionsWidget* m_mapOptionsWidget;
    SynGlyphX::BrowseLineEdit* m_mapFilenameLineEdit;
    QLineEdit* m_mapBoundingBox;
    QLineEdit* m_initalBoundingBox;
};

#endif // MAPGRABBERDIALOG_H
