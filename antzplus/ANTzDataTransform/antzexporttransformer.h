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

#ifndef SYNGLYPHX_ANTZ_ANTZEXPORTTRANSFORMER_H
#define SYNGLYPHX_ANTZ_ANTZEXPORTTRANSFORMER_H

#include "antzdatatransform_global.h"
#include "antztransformer.h"

namespace SynGlyphXANTz {

	class ANTZDATATRANSFORM_EXPORT ANTzExportTransformer : public ANTzTransformer
	{
	public:
		ANTzExportTransformer(const QString& baseOutputDir, const QString& antzTemplateDir, ANTzCSVWriter::OutputPlatform platform, bool useOldANTzFilenames = false);
		virtual ~ANTzExportTransformer();

		static void SetLogoFilename(const QString& logoFilename);

	protected:
		virtual void Prepare();
		virtual void CreateGlyphsFromMapping(const SynGlyphX::DataTransformMapping& mapping);
		virtual void GenerateGrids(std::vector<ANTzGrid>& grids, const SynGlyphX::DataTransformMapping& mapping, const QString& baseImageFilenameDirectory);

		bool m_useOldANTzFilenames;
		QString m_antzTemplateDir;

		static QString s_logoFilename;
	};

} //namespace SynGlyphXANTz

#endif //SYNGLYPHX_ANTZ_ANTZEXPORTTRANSFORMER_H