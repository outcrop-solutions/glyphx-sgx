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

#ifndef SYNGLYPHX_GLYPHGEOMETRY_H
#define SYNGLYPHX_GLYPHGEOMETRY_H

#include "sgxfoundation.h"
#include "glyphgeometryinfo.h"

namespace SynGlyphX {

	template <typename GeometryShapeType>
	class GlyphGeometryTemplate
	{
	public:
		GlyphGeometryTemplate(const GeometryShapeType& shape = GeometryShapeType(), GlyphGeometryInfo::Surface surface = GlyphGeometryInfo::Surface::Solid) :
			m_geometryShape(shape),
			m_geometrySurface(surface),
			m_torusRatio(0.1) {
		
		}

		GlyphGeometryTemplate(const GlyphGeometryTemplate& properties) :
			m_geometryShape(properties.m_geometryShape),
			m_geometrySurface(properties.m_geometrySurface),
			m_torusRatio(properties.m_torusRatio) {

		}

		~GlyphGeometryTemplate() {

		}

		GlyphGeometryTemplate& operator=(const GlyphGeometryTemplate& properties) {

			m_geometryShape = properties.m_geometryShape;
			m_geometrySurface = properties.m_geometrySurface;
			m_torusRatio = properties.m_torusRatio;

			return *this;
		}

		bool operator==(const GlyphGeometryTemplate& properties) const {

			if (m_geometryShape != properties.m_geometryShape) {

				return false;
			}

			if (m_geometrySurface != properties.m_geometrySurface) {

				return false;
			}

			if (m_torusRatio != properties.m_torusRatio) {

				return false;
			}

			return true;
		}

		bool operator!=(const GlyphGeometryTemplate& properties) const {

			return !operator==(properties);
		}

		void SetGeometryShape(const GeometryShapeType& shape) {

			m_geometryShape = shape;
		}

		const GeometryShapeType& GetGeometryShape() const {

			return m_geometryShape;
		}

		void SetGeometrySurface(GlyphGeometryInfo::Surface surface) {

			m_geometrySurface = surface;
		}

		GlyphGeometryInfo::Surface GetGeometrySurface() const {

			return m_geometrySurface;
		}

		void SetTorusRatio(double ratio) {

			m_torusRatio = ratio;
		}

		double GetTorusRatio() const {

			return m_torusRatio;
		}

	protected:
		GeometryShapeType m_geometryShape;
		GlyphGeometryInfo::Surface m_geometrySurface;
		double m_torusRatio;
	};

	typedef GlyphGeometryTemplate<GlyphGeometryInfo::Shape> GlyphGeometry;

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHGEOMETRY_H