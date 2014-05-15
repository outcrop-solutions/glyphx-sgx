#ifndef DOWNLOADEDMAP_H
#define DOWNLOADEDMAP_H

#include <QtCore/QObject>
#include "geographicboundingbox.h"

class DownloadedMap : public QObject
{
    Q_OBJECT

public:
    DownloadedMap(QObject *parent = NULL);
    ~DownloadedMap();

    const GeographicBoundingBox& GetGeographicBoundingBox();

private:
    GeographicBoundingBox m_geographicBoundingBox;
};

#endif // DOWNLOADEDMAP_H
