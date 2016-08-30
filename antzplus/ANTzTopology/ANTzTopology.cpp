#include "ANTzTopology.h"
#include "../../GUI/SGXSceneViewer/SceneViewer/glyphnode.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace SynGlyphXANTz {

	const float ANTzPlacementPolicy::s_offsetUnit = 1.0f;
	const float ANTzPlacementPolicy::s_offsetPin = 5.0f;
	const float ANTzPlacementPolicy::s_offsetRod = 10.0f;
	const float ANTzPlacementPolicy::s_offsetCube = 0.7071068f;
	const float ANTzPlacementPolicy::s_offsetTorus = 0.75f;

	void ANTzPlacementPolicy::ApplyFinalOffset(const SynGlyphX::Glyph3DNode& glyph, glm::mat4& transform) {

		SynGlyphX::GlyphShape shape = glyph.getGeometry();
		const ANTzPlacementPolicy* const placementPolicy = dynamic_cast<const ANTzPlacementPolicy* const>(glyph.getPlacementPolicy());
		bool isChildTopoPin = ((dynamic_cast<const ANTzPinPlacementPolicy* const>(placementPolicy) != nullptr) && (!placementPolicy->isRodTopology()));

		if ((shape == SynGlyphX::GlyphShape::Cylinder || shape == SynGlyphX::GlyphShape::Cone)
			|| (!isChildTopoPin && (shape == SynGlyphX::GlyphShape::Pin))) {

			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, -s_offsetUnit));
		}
	}

	void ANTzPlacementPolicy::ApplyNonPinRodOffset(const SynGlyphX::Glyph3DNode& glyph, const SynGlyphX::Glyph3DNode& parentGlyph, glm::mat4& transform) {

		if (dynamic_cast<const ANTzCubePlacementPolicy* const>(glyph.getPlacementPolicy()) != nullptr)
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, s_offsetCube));
		else if (dynamic_cast<const ANTzTorusPlacementPolicy* const>(glyph.getPlacementPolicy()) != nullptr)
		{
			if (dynamic_cast<const ANTzTorusPlacementPolicy* const>(parentGlyph.getPlacementPolicy()) == nullptr)
				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, s_offsetTorus));
		}
		else if (dynamic_cast<const ANTzPinPlacementPolicy* const>(glyph.getPlacementPolicy()) == nullptr)
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, s_offsetUnit));
	}

	void ANTzPlacementPolicy::ApplyFinalChildOffset(const SynGlyphX::Glyph3DNode& glyph, glm::mat4& transform, glm::mat4& visual_transform) {

		const ANTzPinPlacementPolicy* const placementPolicy = dynamic_cast<const ANTzPinPlacementPolicy* const>(glyph.getPlacementPolicy());

		if (placementPolicy != nullptr) {

			if (!placementPolicy->isRodTopology() && (glyph.getGeometry() != SynGlyphX::GlyphShape::Pin))
			{
				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, s_offsetPin));
			}
			else if (placementPolicy->isRodTopology())
			{					//width uses ratio, length uses scale 2 * 5 = 10
				visual_transform = glm::scale(visual_transform, glm::vec3(glyph.getTorusRatio() * 2.0f, glyph.getTorusRatio() * 2.0f, glyph.getLocalScale().z * s_offsetPin));
				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, s_offsetUnit));
			}
		}

		ApplyFinalOffset(glyph, transform);
	}

	void ANTzPlacementPolicy::ApplyRotation(const glm::vec3& rotation, glm::mat4& transform) {

		transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 0.0f, -1.0f));
		transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(-1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	void ANTzPinPlacementPolicy::repositionChildren(const SynGlyphX::Glyph3DNode& glyph, const glm::mat4& parentTransform) {

		SynGlyphX::GlyphShape shape = glyph.getGeometry();
		for (auto& childGlyph : glyph.getChildren())
		{
			const ANTzPinPlacementPolicy* childPlacementPolicy = dynamic_cast<const ANTzPinPlacementPolicy* const>(childGlyph->getPlacementPolicy());
			const glm::vec3& childScale = childGlyph->getLocalScale();
			const glm::vec3& childPos = childGlyph->getLocalPosition();
			glm::mat4 transform = parentTransform;

			if (m_isRod) {

				// Undo transforms done to draw rod since they aren't supposed to apply to children
				//transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, -s_offsetUnit));
				// TODO: this breaks if this glyph's local scale z is 0 (causing a divide by zero error in this line)
				//float z = glyph.getLocalScale().z;
				//transform = glm::scale(transform, glm::vec3(1.f / (glyph.getTorusRatio() * 2.0f), 1.f / (glyph.getTorusRatio() * 2.0f), 1.f / ( z * s_offsetPin)));
			}
			
			//position the node vertically on the pin, note using translate.x
			if (glyph.isRoot() && !m_isRod)
				//zzoff
				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, s_offsetPin + childPos.x / 37.22f));	//root pin workaround, zz debug
			else if (m_isRod)	//replace kNPoffsetPin with
				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, glyph.getLocalScale().z * (s_offsetRod + 2.0f * childPos.x / 37.22f)));
			else if ((shape == SynGlyphX::GlyphShape::Cone) || (shape == SynGlyphX::GlyphShape::Cylinder))
				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 1.f + childPos.x / 37.22f));
			else
				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, s_offsetPin + childPos.x / 37.22f));

			if ((childPlacementPolicy != nullptr) && (!childPlacementPolicy->isRodTopology()) && !m_isRod)
				transform = glm::scale(transform, glm::vec3(childScale.x * 0.5f, childScale.y * 0.5f, childScale.z * 0.5f));
			else if ((childPlacementPolicy == nullptr) || (!childPlacementPolicy->isRodTopology()))		 //rod is scaled at draw time and does not pass along scale to children
				transform = glm::scale(transform, childScale);	//rod does not scale child

			//offset from center after scaling, preserves ring offset when scaling
			transform = glm::translate(transform, glm::vec3(childPos.z * .008333333f, 0.0f, 0.0f)); // node->translate.y / -37.22f, 0.0f); //locked y, makes more sense when swtiching topos

			ApplyRotation(childGlyph->getLocalRotation(), transform);

			glm::mat4 visual_transform;
			ApplyFinalChildOffset(*childGlyph, transform, visual_transform);

			childGlyph->setCachedTransform(transform);
			childGlyph->setVisualTransform(visual_transform);
		}
	}

	void ANTzCubePlacementPolicy::repositionChildren(const SynGlyphX::Glyph3DNode& glyph, const glm::mat4& parentTransform) {

		for (auto& childGlyph : glyph.getChildren())
		{
			int side = childGlyph->getID() - 6 * (childGlyph->getID() / 6) + 1;
			const glm::vec3& childScale = childGlyph->getLocalScale();
			const glm::vec3& childPos = childGlyph->getLocalPosition();
			glm::mat4 transform = parentTransform;

			// rotate orthogonal to face based on facet then translate
			switch (side) // 
			{
			case 1:
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));		//pos x	front
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case 2:
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));		//neg x	back
				transform = glm::rotate(transform, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case 3:
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));		//pos y	right
				transform = glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case 4:
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));		//neg y	left
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case 5: 									//pos z top, no rotation
				//	glRotatef (-90.0f, 0.0f, 0.0f, 1.0f);
				break;
			case 6:
				//	glRotatef (-90.0f, 0.0f, 0.0f, 1.0f);
				transform = glm::rotate(transform, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	//neg z	bottom
				break;
			default: break;
			}
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, s_offsetCube));	//translate from center to cube surface, explain, debug zz
			transform = glm::scale(transform, glm::vec3(s_offsetCube / 180.0f, s_offsetCube / 180.0f, s_offsetCube / 180.0f)); //scale face +/- 180 deg

			//now position node specific coordinates
			transform = glm::translate(transform, childPos);

			ApplyRotation(childGlyph->getLocalRotation(), transform);

			//scale node up to half parent size
			transform = glm::scale(transform, glm::vec3(90.0f / s_offsetCube, 90.0f / s_offsetCube, 90.0f / s_offsetCube));

			const ANTzPlacementPolicy* const placementPolicy = dynamic_cast<const ANTzPlacementPolicy* const>(childGlyph->getPlacementPolicy());
			if (!placementPolicy->isRodTopology()) { //rod is scaled at draw time and does not pass along scale to children

				transform = glm::scale(transform, childScale);	//node scale
			}	 	

			ApplyNonPinRodOffset(*childGlyph, glyph, transform);
			glm::mat4 visual_transform;
			ApplyFinalChildOffset(*childGlyph, transform, visual_transform);

			childGlyph->setCachedTransform(transform);
			childGlyph->setVisualTransform(visual_transform);
		}
	}

	void ANTzSpherePlacementPolicy::repositionChildren(const SynGlyphX::Glyph3DNode& glyph, const glm::mat4& parentTransform) {

		SynGlyphX::GlyphShape shape = glyph.getGeometry();
		for (auto& childGlyph : glyph.getChildren())
		{
			const glm::vec3& childScale = childGlyph->getLocalScale();
			const glm::vec3& childPos = childGlyph->getLocalPosition();
			glm::mat4 transform = parentTransform;

			if ((shape == SynGlyphX::GlyphShape::Cone) || (shape == SynGlyphX::GlyphShape::Cylinder) || (shape == SynGlyphX::GlyphShape::Pin)) {

				transform = glm::translate(transform, glm::vec3(0.f, 0.f, 1.f));
			}

			transform = glm::rotate(transform, glm::radians(childPos.x + 180.0f), glm::vec3(0.0f, 0.0f, 1.0f));	//longitude, +180 so N = 0
			transform = glm::rotate(transform, glm::radians(childPos.y - 90.f), glm::vec3(-1.0f, 0.0f, 0.0f));		//latitude

			//translate 1 unit to surface for sphere, convert node z local units
			if (m_isPoint)
				transform = glm::translate(transform, glm::vec3(0.f, 0.f, childPos.z)); //point has no offset
			else
				transform = glm::translate(transform, glm::vec3(0.f, 0.f, 1.0f + childPos.z / 229.18312));

			//orientation
			ApplyRotation(childGlyph->getLocalRotation(), transform);

			//scale node * ratio based on topo type
			const ANTzPlacementPolicy* const placementPolicy = dynamic_cast<const ANTzPlacementPolicy* const>(childGlyph->getPlacementPolicy());
			if (m_isPoint)
				transform = glm::scale(transform, childScale);
			else if (placementPolicy->isRodTopology())		 //rod is scaled at draw time and does not pass along scale to children
				transform = glm::scale(transform, glm::vec3(0.25f, 0.25f, 0.25f));	//scale 1/4 size of parent
			else
				transform = glm::scale(transform, glm::vec3(childScale.x * 0.25f, childScale.y * 0.25f, childScale.z * 0.25f));	//node scale

			ApplyNonPinRodOffset(*childGlyph, glyph, transform);
			glm::mat4 visual_transform;
			ApplyFinalChildOffset(*childGlyph, transform, visual_transform );

			childGlyph->setCachedTransform(transform);
			childGlyph->setVisualTransform( visual_transform );
		}
	}

	void ANTzCylinderPlacementPolicy::repositionChildren(const SynGlyphX::Glyph3DNode& glyph, const glm::mat4& parentTransform) {

		SynGlyphX::GlyphShape shape = glyph.getGeometry();
		for (auto& childGlyph : glyph.getChildren())
		{
			const glm::vec3& childScale = childGlyph->getLocalScale();
			const glm::vec3& childPos = childGlyph->getLocalPosition();
			glm::mat4 transform = parentTransform;

			transform = glm::rotate(transform, glm::radians(childPos.x + 180.f), glm::vec3(0.0f, 0.0f, 1.0f));	//longitude, +180 so N = 0
			transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));	//orient perpendicular to cyl

			if ((shape == SynGlyphX::GlyphShape::Cone) || (shape == SynGlyphX::GlyphShape::Cylinder) || (shape == SynGlyphX::GlyphShape::Pin)) {

				transform = glm::translate(transform, glm::vec3(0.0, 1.f + childPos.y / 90.0f, 1.0f + childPos.z / 229.18312));
			}
			else {

				transform = glm::translate(transform, glm::vec3(0.0, childPos.y / 90.0f, 1.0f + childPos.z / 229.18312));
			}

			//orientation
			ApplyRotation(childGlyph->getLocalRotation(), transform);

			//scale 1/4 size of parent
			const ANTzPlacementPolicy* const placementPolicy = dynamic_cast<const ANTzPlacementPolicy* const>(childGlyph->getPlacementPolicy());
			if (placementPolicy->isRodTopology())		 //rod is scaled at draw time and does not pass along scale to children
				transform = glm::scale(transform, glm::vec3(0.25f, 0.25f, 0.25f));
			else
				transform = glm::scale(transform, glm::vec3(childScale.x * 0.25f, childScale.y * 0.25f, childScale.z * 0.25f));			//node scale

			ApplyNonPinRodOffset(*childGlyph, glyph, transform);
			glm::mat4 visual_transform;
			ApplyFinalChildOffset(*childGlyph, transform, visual_transform );

			childGlyph->setCachedTransform(transform);
			childGlyph->setVisualTransform( visual_transform );
		}
	}

	const float ANTzTorusPlacementPolicy::s_torusRadius = 1.5f;

	void ANTzTorusPlacementPolicy::repositionChildren(const SynGlyphX::Glyph3DNode& glyph, const glm::mat4& parentTransform) {

		SynGlyphX::GlyphShape shape = glyph.getGeometry();
		for (auto& childGlyph : glyph.getChildren())
		{
			const glm::vec3& childScale = childGlyph->getLocalScale();
			const glm::vec3& childPos = childGlyph->getLocalPosition();
			glm::mat4 transform = parentTransform;

			if ((shape == SynGlyphX::GlyphShape::Cone) || (shape == SynGlyphX::GlyphShape::Cylinder) || (shape == SynGlyphX::GlyphShape::Pin)) {

				transform = glm::translate(transform, glm::vec3(0.f, 0.f, 1.f));
			}

			//scale up 1.5X to match torus geometry size inner radius
			transform = glm::scale(transform, glm::vec3(s_torusRadius, s_torusRadius, s_torusRadius));

			//position at torus outer radius, inside center of tube
			transform = glm::rotate(transform, glm::radians(childPos.x + 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::translate(transform, glm::vec3(1.0f, 0.0f, 0.0f));	//translate to center of tube

			const ANTzPlacementPolicy* const placementPolicy = dynamic_cast<const ANTzPlacementPolicy* const>(childGlyph->getPlacementPolicy());

			//treat null as torus, later make this depend on the geometry
			if (dynamic_cast<const ANTzTorusPlacementPolicy* const>(childGlyph->getPlacementPolicy()) != nullptr)
			{
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				transform = glm::rotate(transform, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				transform = glm::rotate(transform, glm::radians(childPos.y), glm::vec3(0.0f, 0.0f, 1.0f));	//latitude
				//translate.z based on scale.x and is converted from deg 57.29...
				transform = glm::translate(transform, glm::vec3(0.0f, 0.1591f * childScale.x * childPos.z / 57.29578f, 0.0f));
			}
			else
			{
				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				transform = glm::rotate(transform, glm::radians(childPos.y), glm::vec3(0.0f, -1.0f, 0.0f));	//latitude

				transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));				//orient North
				//translate 1 unit to surface then convert node z local units
				//uses parent->ratio for torus inner radius and 1.5f for scaling factor
				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, glyph.getTorusRatio() + (childPos.z / 360.0f)));

				transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));	//results in 1/4 parent
			}
			//orientation
			ApplyRotation(childGlyph->getLocalRotation(), transform);

			//scale 1/2 size of parent, smaller is good for torus .5 / 1.5
			if (placementPolicy->isRodTopology())		 //rod is scaled at draw time and does not pass along scale to children
				transform = glm::scale(transform, glm::vec3(0.33333333f, 0.33333333f, 0.33333333f));
			else
				transform = glm::scale(transform, glm::vec3(childScale.x * 0.33333333f, childScale.y * 0.33333333f, childScale.z * 0.33333333f));	//node scale

			ApplyNonPinRodOffset(*childGlyph, glyph, transform);
			glm::mat4 visual_transform;
			ApplyFinalChildOffset(*childGlyph, transform, visual_transform );

			childGlyph->setCachedTransform(transform);
			childGlyph->setVisualTransform(visual_transform);
		}
	}
}