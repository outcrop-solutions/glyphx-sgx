#include "downloadedmap.h"

DownloadedMap::DownloadedMap(QObject *parent)
    : QObject(parent),
    m_geographicBoundingBox(GeographicPoint(0.0, 0.0), 0.0, 0.0)
{

}

DownloadedMap::~DownloadedMap()
{

}

const GeographicBoundingBox& DownloadedMap::GetGeographicBoundingBox() {

    return m_geographicBoundingBox;
}
