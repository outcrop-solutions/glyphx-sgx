#ifndef MAPOPTIONSWIDGET_H
#define MAPOPTIONSWIDGET_H

#include "mapdownloading_global.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include "sizewidget.h"
#include "networkdownloader.h"

class MAPDOWNLOADING_EXPORT MapOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    MapOptionsWidget(QWidget *parent = 0);
    ~MapOptionsWidget();

    NetworkDownloader::MapSource GetMapSource() const;
    QSize GetMapSize() const;
    NetworkDownloader::MapType GetMapType() const;

private slots:
    void OnMapSourceChanged();

private:
    void SetRadioButtonsFromMapSource(NetworkDownloader::MapSource source);
    void ReadSettings();
    void WriteSettings();

    QRadioButton* m_mapquestRadioButton;
    QRadioButton* m_googleRadioButton;
    QComboBox* m_mapTypeComboBox;
    SynGlyphX::SizeWidget* m_imageSizeWidget;
};

#endif // MAPOPTIONSWIDGET_H
