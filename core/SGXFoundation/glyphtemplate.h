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
#ifndef SYNGLYPHX_GLYPHTEMPLATE_H
#define SYNGLYPHX_GLYPHTEMPLATE_H

#include "sgxfoundation.h"
#include <array>
#include "glyphgeometry.h"

namespace SynGlyphX {

	template <typename NumericType, typename ColorType, typename TextType, typename StructureType, typename VirtualTopologyType>
	class GlyphTemplate {

	public:
		typedef std::array<NumericType, 3> NumericTypeXYZ;

		GlyphTemplate(const StructureType& structure = StructureType(), const VirtualTopologyType& virtualTopology = VirtualTopologyType()) :
			m_position(NumericTypeXYZ()),
			m_rotation(NumericTypeXYZ()),
			m_scale(NumericTypeXYZ()),
			m_color(ColorType()),
			m_transparency(NumericType()),
			m_tag(TextType()),
			m_description(TextType()),
			m_rotationRate(NumericTypeXYZ()),
			m_structure(structure),
			m_virtualTopology(virtualTopology),
			m_url(TextType()) {

		}

		GlyphTemplate(const GlyphTemplate& glyph) :
			m_position(glyph.m_position),
			m_rotation(glyph.m_rotation),
			m_scale(glyph.m_scale),
			m_color(glyph.m_color),
			m_transparency(glyph.m_transparency),
			m_tag(glyph.m_tag),
			m_description(glyph.m_description),
			m_rotationRate(glyph.m_rotationRate),
			m_structure(glyph.m_structure),
			m_virtualTopology(glyph.m_virtualTopology),
			m_url(glyph.m_url) {

		}

		virtual ~GlyphTemplate() {

		}

		StructureType& GetStructure() {

			return m_structure;
		}

		const StructureType& GetStructure() const {

			return m_structure;
		}

		VirtualTopologyType& GetVirtualTopology() {

			return m_virtualTopology;
		}

		const VirtualTopologyType& GetVirtualTopology() const {

			return m_virtualTopology;
		}

		NumericTypeXYZ& GetPosition() {

			return m_position;
		}

		const NumericTypeXYZ& GetPosition() const {

			return m_position;
		}

		NumericTypeXYZ& GetRotation() {

			return m_rotation;
		}

		const NumericTypeXYZ& GetRotation() const {

			return m_rotation;
		}

		NumericTypeXYZ& GetScale() {

			return m_scale;
		}

		const NumericTypeXYZ& GetScale() const {

			return m_scale;
		}

		ColorType& GetColor() {

			return m_color;
		}

		const ColorType& GetColor() const {

			return m_color;
		}

		NumericType& GetTransparency() {

			return m_transparency;
		}

		const NumericType& GetTransparency() const {

			return m_transparency;
		}

		TextType& GetTag() {

			return m_tag;
		}

		const TextType& GetTag() const {

			return m_tag;
		}

		TextType& GetDescription() {

			return m_description;
		}

		const TextType& GetDescription() const {

			return m_description;
		}

		NumericTypeXYZ& GetRotationRate() {

			return m_rotationRate;
		}

		const NumericTypeXYZ& GetRotationRate() const {

			return m_rotationRate;
		}

		TextType& GetURL() {

			return m_url;
		}

		const TextType& GetURL() const {

			return m_url;
		}

		GlyphTemplate& operator=(const GlyphTemplate& glyph) {

			m_position = glyph.m_position;
			m_rotation = glyph.m_rotation;
			m_scale = glyph.m_scale;
			m_color = glyph.m_color;
			m_transparency = glyph.m_transparency;
			m_tag = glyph.m_tag;
			m_description = glyph.m_description;
			m_rotationRate = glyph.m_rotationRate;
			m_structure = glyph.m_structure;
			m_virtualTopology = glyph.m_virtualTopology;
			m_url = glyph.m_url;

			return *this;
		}

		bool operator==(const GlyphTemplate& glyph) const {

			if (m_position != glyph.m_position) {

				return false;
			}

			if (m_rotation != glyph.m_rotation) {

				return false;
			}

			if (m_scale != glyph.m_scale) {

				return false;
			}

			if (m_color != glyph.m_color) {

				return false;
			}

			if (m_transparency != glyph.m_transparency) {

				return false;
			}

			if (m_tag != glyph.m_tag) {

				return false;
			}

			if (m_description != glyph.m_description) {

				return false;
			}

			if (m_rotationRate != glyph.m_rotationRate) {

				return false;
			}

			if (m_structure != glyph.m_structure) {

				return false;
			}

			if (m_virtualTopology != glyph.m_virtualTopology) {

				return false;
			}
			
			if (m_url != glyph.m_url) {

				return false;
			}

			return true;
		}

		bool operator!=(const GlyphTemplate& glyph) const {

			return !operator==(glyph);
		}

	protected:
		//Appearance Properties
		NumericTypeXYZ m_position;
		NumericTypeXYZ m_rotation;
		NumericTypeXYZ m_scale;
		ColorType m_color;
		NumericType m_transparency;

		//Tag & Description Properties
		TextType m_tag;
		TextType m_description;

		//Animation Properties
		NumericTypeXYZ m_rotationRate;

		//Glyph Structure & Virtual Topology
		StructureType m_structure;
		VirtualTopologyType m_virtualTopology;

		TextType m_url;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHTEMPLATE_H