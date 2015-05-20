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

#ifndef SYNGLYPHX_ANTZ_ANTZTRANSFORMER_H
#define SYNGLYPHX_ANTZ_ANTZTRANSFORMER_H

#include "antzdatatransform_global.h"
#include "transformer.h"
#include "datatransformmapping.h"
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace SynGlyphXANTz {

	class ANTZDATATRANSFORM_EXPORT ANTzTransformer : public SynGlyphX::Transformer

	{
	public:
		//typedef std::unordered_map<QString, unsigned int> BaseImageTextureMap;

		ANTzTransformer(const QString& baseOutputDir);
		virtual ~ANTzTransformer();

		const QStringList& GetCSVFilenames() const;
		const QStringList& GetBaseImageFilenames() const;

		unsigned int GetTextureID(unsigned int index) const;

	protected:
		const unsigned int NumberOfDefaultBaseImages = 1;

		virtual QString GenerateBaseImageFilename(unsigned int index) const;
		void GenerateCache(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilenameDirectory);
		void Clear();

		QStringList m_csvFilenames;
		QStringList m_baseImageFilenames;
		QString m_baseOutputDir;
		std::vector<unsigned int> m_textureIDs;
	};

} //namespace SynGlyphXANTz

#endif //SYNGLYPHX_ANTZ_ANTZTRANSFORMER_H