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
#ifndef SYNGLYPHX_DATAMAPPINGGLYPHGEOMETRY_H
#define SYNGLYPHX_DATAMAPPINGGLYPHGEOMETRY_H

#include "sgxdatamapping.h"
#include "glyphgeometry.h"
#include "datamappingproperty.h"
#include <boost/property_tree/ptree_fwd.hpp>

namespace SynGlyphX {

	class SGXDATAMAPPING_API DataMappingGlyphGeometry : public GlyphGeometryTemplate < GeometryShapeMappingProperty >
	{
	public:
		DataMappingGlyphGeometry();
		DataMappingGlyphGeometry(const DataMappingGlyphGeometry& glyphGeometry);
		DataMappingGlyphGeometry(const GlyphGeometry& glyphGeometry);
		DataMappingGlyphGeometry(const boost::property_tree::wptree& propertyTree, bool useOldPropertyTree = false);
		~DataMappingGlyphGeometry();

		DataMappingGlyphGeometry& operator=(const DataMappingGlyphGeometry& glyphGeometry);
		bool operator==(const DataMappingGlyphGeometry& glyphGeometry) const;
		bool operator!=(const DataMappingGlyphGeometry& glyphGeometry) const;

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;
		GlyphGeometry ExportGlyphGeometry() const;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGGLYPHGEOMETRY_H