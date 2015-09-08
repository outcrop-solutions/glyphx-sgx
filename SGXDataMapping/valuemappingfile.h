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

#ifndef SYNGLYPHX_VALUEMAPPINGFILE_H
#define SYNGLYPHX_VALUEMAPPINGFILE_H

#include "sgxdatamapping.h"
#include "xmlpropertytreefile.h"
#include "valuemappingfunction.h"
#include <memory>

namespace SynGlyphX {

	template<typename ValueMappingType>
	class SGXDATAMAPPING_API ValueMappingFile : public XMLPropertyTreeFile
	{
	public:
		ValueMappingFile(std::shared_ptr<ValueMappingType> valueMapping = nullptr);
		virtual ~ValueMappingFile();

		std::shared_ptr<ValueMappingType> GetValueMapping() const;

	protected:
		virtual void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree);
		virtual void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const;

		std::shared_ptr<ValueMappingType> m_valueMapping;
	};

	typedef ValueMappingFile<Numeric2NumericMappingData> Numeric2NumericMappingFile;
	typedef ValueMappingFile<Text2NumericMappingData> Text2NumericMappingFile;
	typedef ValueMappingFile<Numeric2ColorMappingData> Numeric2ColorMappingFile;
	typedef ValueMappingFile<Text2ColorMappingData> Text2ColorMappingFile;

	typedef ValueMappingFile<Numeric2ShapeMappingData> Numeric2ShapeMappingFile;
	typedef ValueMappingFile<Text2ShapeMappingData> Text2ShapeMappingFile;

	typedef ValueMappingFile<Numeric2VirtualTopologyMappingData> Numeric2VirtualTopologyMappingFile;
	typedef ValueMappingFile<Text2VirtualTopologyMappingData> Text2VirtualTopologyMappingFile;

	typedef ValueMappingFile<Range2NumericMappingData> Range2NumericMappingFile;
	typedef ValueMappingFile<Range2ColorMappingData> Range2ColorMappingFile;

	typedef ValueMappingFile<Range2ShapeMappingData> Range2ShapeMappingFile;
	typedef ValueMappingFile<Range2VirtualTopologyMappingData> Range2VirtualTopologyMappingFile;

} //namespace SynGlyphX

#endif //SYNGLYPHX_VALUEMAPPINGFILE_H