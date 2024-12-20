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
#ifndef SYNGLYPHX_GLYPH_H
#define SYNGLYPHX_GLYPH_H

#include "sgxfoundation.h"
#include "glyphtemplate.h"
#include "glyphcolor.h"
#include "utilitytypes.h"
#include "virtualtopology.h"

namespace SynGlyphX {

	class GlyphColor;

	class SGXFOUNDATION_API Glyph : public GlyphTemplate < double, GlyphColor, std::wstring, GlyphGeometry, VirtualTopology>
	{
	public:
		Glyph();
		Glyph(const Glyph& glyph);
		virtual ~Glyph();

		Vector3& GetTagOffset();
		const Vector3& GetTagOffset() const;

		Glyph& operator=(const Glyph& glyph);
		bool operator==(const Glyph& glyph) const;
		bool operator!=(const Glyph& glyph) const;

		static const Glyph s_defaultGlyph;
		static const Glyph s_defaultRootGlyph;

	protected:
		static Glyph CreateDefaultGlyph(GlyphGeometryInfo::Shape geometryShape, VirtualTopologyInfo::Type virtualTopologyType);

		Vector3 m_tagOffset;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPH_H