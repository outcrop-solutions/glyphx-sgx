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

#ifndef SYNGLYPHX_DOWNLOADEDMAPPROPERTIES_H
#define SYNGLYPHX_DOWNLOADEDMAPPROPERTIES_H

#include "baseimageproperties.h"
#include <array>
#include "sgxdatatransform_global.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/bimap.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DownloadedMapProperties : public BaseImageProperties
	{
	public:
		typedef std::array<unsigned int, 2> Size;

		enum MapSource {
			MapQuestOpen, //MapQuestOpen gets data from OpenStreetMap
			GoogleMaps
		};

		enum MapType {
			Map,
			Satellite,
			Hybrid
		};

		DownloadedMapProperties(MapSource source, MapType type, Size size = { { 2048, 1024 } });
		DownloadedMapProperties(const boost::property_tree::wptree& propertyTree);
		DownloadedMapProperties(const DownloadedMapProperties& properties);
		virtual ~DownloadedMapProperties();

		//virtual bool IsGeographic() const;

		MapSource GetSource() const;
		MapType GetType() const;
		Size GetSize() const;

		virtual void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		static const boost::bimap<MapSource, std::wstring> s_mapSourceStrings;
		static const boost::bimap<MapType, std::wstring> s_mapTypeStrings;

	private:
		MapSource m_source;
		MapType m_type;
		Size m_size;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DOWNLOADEDMAPPROPERTIES_H