#ifndef MAPOPTIONSWIDGET_H
#define MAPOPTIONSWIDGET_H

#include "mapdownloading_global.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include "sizewidget.h"
#include "networkdownloader.h"
#include "downloadedmapproperties.h"

class MAPDOWNLOADING_EXPORT MapOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    MapOptionsWidget(QWidget *parent = 0);
    ~MapOptionsWidget();

	void Set(SynGlyphX::DownloadedMapProperties::MapSource source, SynGlyphX::DownloadedMapProperties::MapType type, const QSize& size);

    SynGlyphX::DownloadedMapProperties::MapSource GetMapSource() const;
    QSize GetMapSize() const;
	SynGlyphX::DownloadedMapProperties::MapType GetMapType() const;

private slots:
    void OnMapSourceChanged();

protected:
	void SetRadioButtonsFromMapSource(SynGlyphX::DownloadedMapProperties::MapSource source);
    //void ReadSettings();
    //void WriteSettings();

    QRadioButton* m_mapquestRadioButton;
    QRadioButton* m_googleRadioButton;
    QComboBox* m_mapTypeComboBox;
    SynGlyphX::SizeWidget* m_imageSizeWidget;
};

#endif // MAPOPTIONSWIDGET_H
