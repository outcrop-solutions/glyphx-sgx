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

#include "../../GUI/SGXSceneViewer/SceneViewer/placementpolicy.h"

namespace SynGlyphXANTz {

	class ANTzPlacementPolicy : public SynGlyphX::PlacementPolicy {

	public:
		ANTzPlacementPolicy() {}
		virtual ~ANTzPlacementPolicy() {}

		static void ApplyFinalOffset(const SynGlyphX::Glyph3DNode& glyph, glm::mat4& transform);

		virtual bool isRodTopology() const { return false; }

		enum class TopologyType : int {

			Cube = 1,
			Sphere = 2,
			Torus = 3,
			Cylinder = 4,
			Pin = 5,
			Rod = 6,
			Point = 7
		};

	protected:
		static const float s_offsetUnit;
		static const float s_offsetPin;
		static const float s_offsetRod;
		static const float s_offsetCube;
		static const float s_offsetTorus;
		
		void ApplyNonPinRodOffset(const SynGlyphX::Glyph3DNode& glyph, const SynGlyphX::Glyph3DNode& parentGlyph, glm::mat4& transform);
		void ApplyFinalChildOffset(const SynGlyphX::Glyph3DNode& glyph, glm::mat4& transform, glm::vec3& visual_scale);
		void ApplyRotation(const glm::vec3& rotation, glm::mat4& transform);
	};

	class ANTzPinPlacementPolicy : public ANTzPlacementPolicy {

	public:
		ANTzPinPlacementPolicy(bool isRod) : m_isRod(isRod) {}
		virtual ~ANTzPinPlacementPolicy() {}

		bool isRodTopology() const override { return m_isRod; }

		void repositionChildren(const SynGlyphX::Glyph3DNode& glyph, const glm::mat4& parentTransform) override;

	protected:
		bool m_isRod;
	};

	class ANTzCubePlacementPolicy : public ANTzPlacementPolicy {

	public:
		ANTzCubePlacementPolicy() {}
		virtual ~ANTzCubePlacementPolicy() {}

		void repositionChildren(const SynGlyphX::Glyph3DNode& glyph, const glm::mat4& parentTransform) override;

	protected:
		
	};

	class ANTzSpherePlacementPolicy : public ANTzPlacementPolicy {

	public:
		ANTzSpherePlacementPolicy(bool isPoint) : m_isPoint(isPoint) {}
		virtual ~ANTzSpherePlacementPolicy() {}

		bool isPoint() const { return m_isPoint; }

		void repositionChildren(const SynGlyphX::Glyph3DNode& glyph, const glm::mat4& parentTransform) override;

	protected:
		bool m_isPoint;
	};

	class ANTzCylinderPlacementPolicy : public ANTzPlacementPolicy {

	public:
		ANTzCylinderPlacementPolicy() {}
		virtual ~ANTzCylinderPlacementPolicy() {}

		void repositionChildren(const SynGlyphX::Glyph3DNode& glyph, const glm::mat4& parentTransform) override;

	protected:

	};

	class ANTzTorusPlacementPolicy : public ANTzPlacementPolicy {

	public:
		ANTzTorusPlacementPolicy() {}
		virtual ~ANTzTorusPlacementPolicy() {}

		void repositionChildren(const SynGlyphX::Glyph3DNode& glyph, const glm::mat4& parentTransform) override;

	protected:
		static const float s_torusRadius;
	};

} //namespace SynGlyphXANTz

//#pragma once