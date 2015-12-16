///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

#ifndef NETWORKDOWNLOADER_H
#define NETWORKDOWNLOADER_H

#include "mapdownloading_global.h"
#include <QtCore/QObject>
#include <string>
#include <vector>
#include "geographicboundingbox.h"
#include <QtNetwork/QNetworkAccessManager>
#include "downloadedmapproperties.h"

typedef boost::geometry::strategy::distance::haversine<double> DistanceStrategy;

class MAPDOWNLOADING_EXPORT NetworkDownloader
{

public:
	typedef std::pair<GeographicBoundingBox, SynGlyphX::IntSize> BoundingBoxAndSize;

	static const char* ImageFormat;

	NetworkDownloader();
	~NetworkDownloader();

	static NetworkDownloader& Instance();
	
	BoundingBoxAndSize DownloadMap(const std::vector<GeographicPoint>& points, const std::string& filename, SynGlyphX::DownloadedMapProperties::ConstSharedPtr properties);

    void SetMapQuestOpenKey(const QString& key);
    const QString& GetMapQuestOpenKey() const;

    //void SetGoogleMapsKey(const QString& key);
    //const QString& GetGoogleMapsKey() const;

private:
	unsigned int GetZoomLevel(const GeographicBoundingBox& boundingBox, bool useBestFit, SynGlyphX::IntSize& imageSize);
	QString GenerateMapQuestOpenString(const GeographicPoint& center, unsigned int zoomLevel, SynGlyphX::DownloadedMapProperties::ConstSharedPtr properties, const SynGlyphX::IntSize& imageSize);
    void ReadSettings();
    void WriteSettings();

	static NetworkDownloader s_instance;

	DistanceStrategy m_distanceStrategy;

	QString m_mapQuestOpenKey;
    //QString m_googlemapsOpenKey;

	std::map<unsigned int, double> m_mapQuestMetersPerPixelAtEquator;
};

#endif //NETWORKDOWNLOADER_H