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

#ifndef ANTZVISUALIZATIONFILELISTING_H
#define ANTZVISUALIZATIONFILELISTING_H

#include "ANTzBridge.h"
#include "xmlpropertytreefile.h"
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "uuid.h"

namespace SynGlyphXANTz {

	class ANTZBRIDGE_API ANTzVisualizationFileListing : public SynGlyphX::XMLPropertyTreeFile
	{
	public:
		ANTzVisualizationFileListing();
		ANTzVisualizationFileListing(const std::wstring& nodeFilename,
			const std::wstring& tagFilename,
			const std::wstring& channelFilename,
			const std::wstring& channelMapFilename,
			const std::wstring& baseImageFilename = L"");
		ANTzVisualizationFileListing(const ANTzVisualizationFileListing& fileListing);
		~ANTzVisualizationFileListing();

		ANTzVisualizationFileListing& operator=(const ANTzVisualizationFileListing& fileListing);
		
		bool UsesDefaultImage() const;
		bool IsValid() const;

		void Clear();

		const std::wstring& GetNodeFilename() const;
		const std::wstring& GetTagFilename() const;
		const std::wstring& GetChannelFilename() const;
		const std::wstring& GetChannelMapFilename() const;
		const std::wstring& GetBaseImageFilename() const;

		const boost::uuids::uuid& GetID() const;

	protected:
		virtual void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree);
		virtual void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const;

		std::wstring m_nodeFilename;
		std::wstring m_tagFilename;
		std::wstring m_channelFilename;
		std::wstring m_channelMapFilename;
		std::wstring m_baseImageFilename;
		boost::uuids::uuid m_id;
	};

} //namespace SynGlyphXANTz

#endif //ANTZVISUALIZATIONFILELISTING_H