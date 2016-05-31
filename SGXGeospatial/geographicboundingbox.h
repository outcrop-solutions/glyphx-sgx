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
#pragma once
#ifndef GEOGRAPHICBOUNDINGBOX_H
#define GEOGRAPHICBOUNDINGBOX_H

#include "sgxggeospatial.h"
#include <vector>
#include <string>
#include <boost/geometry/geometry.hpp>

typedef boost::geometry::model::point<double, 2, boost::geometry::cs::geographic<boost::geometry::degree>> GeographicPoint;

class SGXGGEOSPATIAL_API GeographicBoundingBox
{
public:
	GeographicBoundingBox();
    GeographicBoundingBox(const GeographicPoint& swCorner, const GeographicPoint& neCorner);
    GeographicBoundingBox(const GeographicPoint& center, double latRadius, double lonRadius);
	GeographicBoundingBox(const std::vector<GeographicPoint>& points);
	GeographicBoundingBox(const GeographicBoundingBox& boundingBox);
    ~GeographicBoundingBox();

	GeographicBoundingBox& operator=(const GeographicBoundingBox& boundingBox);

	const GeographicPoint& GetSWCorner() const;
	const GeographicPoint& GetNECorner() const;
    const GeographicPoint& GetCenter() const;
    const GeographicPoint& GetWestCenter() const;
    const GeographicPoint& GetEastCenter() const;
    const GeographicPoint& GetNorthCenter() const;
    const GeographicPoint& GetSouthCenter() const;

	double GetWidth() const;
	double GetHeight() const;

	bool IsValid() const;

	std::string ToString() const;

    void WriteToKMLFile(const std::string& filename) const;

private:
    GeographicPoint m_swCorner;
    GeographicPoint m_neCorner;
    GeographicPoint m_center;
    GeographicPoint m_westCenter;
    GeographicPoint m_eastCenter;
    GeographicPoint m_northCenter;
    GeographicPoint m_southCenter;
};

#endif //GEOGRAPHICBOUNDINGBOX_H