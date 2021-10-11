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
#ifndef DOWNLOADEDMAPPROPERTIES_H
#define DOWNLOADEDMAPPROPERTIES_H

#include "GlyphEngine_Exports.h"
#include "BaseImageProperties.h"
#include <array>
SGX_PRAGMA_WARNING_PUSH
SGX_PRAGMA_WARNING_DISABLE(4512)
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/bimap.hpp>
SGX_PRAGMA_WARNING_POP
#include "UtilityTypes.h"

namespace GlyphEngine {

	class GLYPHENGINE DownloadedMapProperties : public BaseImageProperties
	{
	public:
		enum MapSource {
			MapQuestOpen, //MapQuestOpen gets data from OpenStreetMap
			GoogleMaps
		};

		enum MapType {
			Map,
			Satellite,
			Hybrid
		};

		typedef std::shared_ptr<DownloadedMapProperties> SharedPtr;
		typedef std::shared_ptr<const DownloadedMapProperties> ConstSharedPtr;

		static const unsigned int MaxSizeMapQuest = 3840;
		static const unsigned int MaxSizeGoogleMaps = 2048;

		DownloadedMapProperties(MapSource source = MapSource::MapQuestOpen, MapType type = MapType::Hybrid, bool useBestFit = true, bool invert = false, bool grayscale = false, IntSize size = { { 2048, 1024 } }, unsigned int margin = 20);
		DownloadedMapProperties(const boost::property_tree::wptree& propertyTree);
		DownloadedMapProperties(const DownloadedMapProperties& properties);
		virtual ~DownloadedMapProperties();

		bool operator==(const DownloadedMapProperties& properties) const;
		bool operator!=(const DownloadedMapProperties& properties) const;

		MapSource GetSource() const;
		void SetSource(MapSource source);

		MapType GetType() const;
		void SetType(MapType type);

		IntSize GetSize() const;
		void SetSize(const IntSize& size);

		bool GetInvert() const;
		void SetInvert(bool invert);

		bool GetGrayscale() const;
		void SetGrayscale(bool grayscale);

		bool GetUseBestFit() const;
		void SetUseBestFit(bool useBestFit);

		unsigned int GetMargin() const;
		void SetMargin(unsigned int margin);

		virtual bool IsGeographic() const;
		virtual void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		static const boost::bimap<MapSource, std::wstring> s_mapSourceStrings;
		static const boost::bimap<MapType, std::wstring> s_mapTypeStrings;

	private:
		MapSource m_source;
		MapType m_type;
		IntSize m_size;
		bool m_invert;
		bool m_grayscale;
		bool m_useBestFit;
		unsigned int m_margin;
	};

} 

#endif //DOWNLOADEDMAPPROPERTIES_H