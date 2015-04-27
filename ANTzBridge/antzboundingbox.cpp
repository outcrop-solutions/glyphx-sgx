#include "antzboundingbox.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <cmath>

#define kNPinvertDeg 0.005555555555f	//equals 1/180

namespace SynGlyphXANTz {

	const ANTzBoundingBox ANTzBoundingBox::s_pin = CreatePinBB();
	const ANTzBoundingBox ANTzBoundingBox::s_cube = CreateCubeBB();
	const ANTzBoundingBox ANTzBoundingBox::s_cylinder = CreateCylinderBB();
	const ANTzBoundingBox ANTzBoundingBox::s_sphere = CreateSphereBB();

	ANTzBoundingBox::ANTzBoundingBox() :
		m_minCorner(-1.0),
		m_maxCorner(1.0),
		m_matrixTransform(1.0)
	{
	}

	ANTzBoundingBox::ANTzBoundingBox(const ANTzBoundingBox& boundingBox) :
		m_minCorner(boundingBox.m_minCorner),
		m_maxCorner(boundingBox.m_maxCorner),
		m_matrixTransform(boundingBox.m_matrixTransform) {


	}


	ANTzBoundingBox::~ANTzBoundingBox()
	{
	}

	ANTzBoundingBox& ANTzBoundingBox::operator=(const ANTzBoundingBox& boundingBox) {

		m_minCorner = boundingBox.m_minCorner;
		m_maxCorner = boundingBox.m_maxCorner;
		m_matrixTransform = boundingBox.m_matrixTransform;

		return *this;
	}

	void ANTzBoundingBox::SetTransform(const glm::mat4& matrix) {

		m_matrixTransform = matrix;
	}

	const glm::mat4& ANTzBoundingBox::GetTransform() const {

		return m_matrixTransform;
	}

	const glm::vec3& ANTzBoundingBox::GetMinCorner() const {

		return m_minCorner;
	}

	const glm::vec3& ANTzBoundingBox::GetMaxCorner() const {

		return m_maxCorner;
	}

	ANTzBoundingBox::Intersection ANTzBoundingBox::DoesLineIntersect(const Line& line) {

		// Intersection method from Real-Time Rendering and Essential Mathematics for Games

		glm::vec3 ray_origin = line.first;
		glm::vec3 ray_direction = glm::normalize(line.second - line.first);

		float tMin = 0.0f;
		float tMax = 100000.0f;

		glm::vec3 OBBposition_worldspace(m_matrixTransform[3].x, m_matrixTransform[3].y, m_matrixTransform[3].z);

		glm::vec3 delta = OBBposition_worldspace - ray_origin;

		// Test intersection with the 2 planes perpendicular to the OBB's X axis
		{
			glm::vec3 xaxis(m_matrixTransform[0].x, m_matrixTransform[0].y, m_matrixTransform[0].z);
			float e = glm::dot(xaxis, delta);
			float f = glm::dot(ray_direction, xaxis);

			if (fabs(f) > 0.001f){ // Standard case

				float t1 = (e + m_minCorner.x) / f; // Intersection with the "left" plane
				float t2 = (e + m_maxCorner.x) / f; // Intersection with the "right" plane
				// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

				// We want t1 to represent the nearest intersection, 
				// so if it's not the case, invert t1 and t2
				if (t1>t2){
					float w = t1; t1 = t2; t2 = w; // swap t1 and t2
				}

				// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
				if (t2 < tMax)
					tMax = t2;
				// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
				if (t1 > tMin)
					tMin = t1;

				// And here's the trick :
				// If "far" is closer than "near", then there is NO intersection.
				// See the images in the tutorials for the visual explanation.
				if (tMax < tMin)
					return boost::none;

			}
			else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
				if (-e + m_minCorner.x > 0.0f || -e + m_maxCorner.x < 0.0f)
					return boost::none;
			}
		}


		// Test intersection with the 2 planes perpendicular to the OBB's Y axis
		// Exactly the same thing than above.
		{
			glm::vec3 yaxis(m_matrixTransform[1].x, m_matrixTransform[1].y, m_matrixTransform[1].z);
			float e = glm::dot(yaxis, delta);
			float f = glm::dot(ray_direction, yaxis);

			if (fabs(f) > 0.001f){

				float t1 = (e + m_minCorner.y) / f;
				float t2 = (e + m_maxCorner.y) / f;

				if (t1>t2){ float w = t1; t1 = t2; t2 = w; }

				if (t2 < tMax)
					tMax = t2;
				if (t1 > tMin)
					tMin = t1;
				if (tMin > tMax)
					return boost::none;

			}
			else{
				if (-e + m_minCorner.y > 0.0f || -e + m_maxCorner.y < 0.0f)
					return boost::none;
			}
		}


		// Test intersection with the 2 planes perpendicular to the OBB's Z axis
		// Exactly the same thing than above.
		{
			glm::vec3 zaxis(m_matrixTransform[2].x, m_matrixTransform[2].y, m_matrixTransform[2].z);
			float e = glm::dot(zaxis, delta);
			float f = glm::dot(ray_direction, zaxis);

			if (fabs(f) > 0.001f){

				float t1 = (e + m_minCorner.z) / f;
				float t2 = (e + m_maxCorner.z) / f;

				if (t1>t2){ float w = t1; t1 = t2; t2 = w; }

				if (t2 < tMax)
					tMax = t2;
				if (t1 > tMin)
					tMin = t1;
				if (tMin > tMax)
					return boost::none;

			}
			else{
				if (-e + m_minCorner.z > 0.0f || -e + m_maxCorner.z < 0.0f)
					return boost::none;
			}
		}

		return tMin;
	}

	ANTzBoundingBox ANTzBoundingBox::GetDefaultBB(pNPnode node) {

		ANTzBoundingBox bb;

		if ((node->geometry == kNPgeoSphereWire) || (node->geometry == kNPgeoSphere)) {

			bb = s_sphere;
		}
		else if ((node->geometry == kNPgeoConeWire) || (node->geometry == kNPgeoCone) ||
			(node->geometry == kNPgeoCylinderWire) || (node->geometry == kNPgeoCylinder)) {

			bb = s_cylinder;
		}
		else if ((node->geometry == kNPgeoPinWire) || (node->geometry == kNPgeoPin)) {

			bb = s_pin;
		}
		else if ((node->geometry == kNPgeoTorusWire) || (node->geometry == kNPgeoTorus)) {

			bb = CreateTorusBB(node->ratio);
		}
		else {

			bb = s_cube;
		}

		return bb;
	}

	ANTzBoundingBox ANTzBoundingBox::CreateBoundingBox(pNPnode node, float rootGridZScale) {

		ANTzBoundingBox bb = GetDefaultBB(node);

		bb.SetTransform(CreateTransform(node, rootGridZScale));

		return bb;
	}

	ANTzBoundingBox ANTzBoundingBox::CreateBoundingBox(pNPnode node, const glm::mat4& previousTransform) {

		ANTzBoundingBox bb = GetDefaultBB(node);

		bb.SetTransform(CreateTransform(node, previousTransform));

		return bb;
	}

	ANTzBoundingBox ANTzBoundingBox::CreatePinBB() {

		ANTzBoundingBox bb;

		bb.m_minCorner = glm::vec3(-0.5, -0.5, 0.0);
		bb.m_maxCorner = glm::vec3(0.5, 0.5, 5.5);

		return bb;
	}

	ANTzBoundingBox ANTzBoundingBox::CreateTorusBB(double ratio) {

		ANTzBoundingBox bb;

		bb.m_minCorner = glm::vec3(-kNPtorusRadius - 2 * ratio, -kNPtorusRadius - 2 * ratio, -kNPtorusRadius * ratio);
		bb.m_maxCorner = glm::vec3(kNPtorusRadius + 2 * ratio, kNPtorusRadius + 2 * ratio, kNPtorusRadius * ratio);

		return bb;
	}

	ANTzBoundingBox ANTzBoundingBox::CreateCubeBB() {

		ANTzBoundingBox bb;
		double radius = std::sqrt(2.0) / 2.0;

		bb.m_minCorner = glm::vec3(-radius, -radius, -radius);
		bb.m_maxCorner = glm::vec3(radius, radius, radius);

		return bb;
	}

	ANTzBoundingBox ANTzBoundingBox::CreateCylinderBB() {

		ANTzBoundingBox bb;

		bb.m_minCorner = glm::vec3(-1, -1, 0.0);
		bb.m_maxCorner = glm::vec3(1, 1, 2.0);

		return bb;
	}

	ANTzBoundingBox ANTzBoundingBox::CreateSphereBB() {

		ANTzBoundingBox bb;

		bb.m_minCorner = glm::vec3(-1.0, -1.0, -1.0);
		bb.m_maxCorner = glm::vec3(1.0, 1.0, 1.0);

		return bb;
	}

	glm::mat4 ANTzBoundingBox::CreateTransform(pNPnode node, float rootGridZScale) {

		glm::mat4 transform(1.0);

		transform = glm::translate(transform, glm::vec3(node->translate.x, node->translate.y, node->translate.z * rootGridZScale));
		ApplyRotation(node, transform);

		if (node->topo == kNPtopoRod) {

			transform = glm::scale(transform, glm::vec3(node->ratio * 2.0f, node->ratio * 2.0f, node->scale.z * kNPoffsetRod * 0.5f));
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, kNPoffsetUnit));
		}
		else {

			transform = glm::scale(transform, glm::vec3(node->scale.x, node->scale.y, node->scale.z));
		}

		AddFinalTransform(node, transform);

		return transform;
	}

	glm::mat4 ANTzBoundingBox::CreateTransform(pNPnode node, const glm::mat4& previousTransform) {

		glm::mat4 transform = previousTransform;
		pNPnode parent = node->parent;

		if (parent->topo == kNPtopoCube) {
		
			ApplyCubeTransform(node, transform);
		}
		else if (parent->topo == kNPtopoSphere || parent->topo == kNPtopoPoint) {

			ApplySphereTransform(node, transform);
		}
		else if (parent->topo == kNPtopoCylinder) {

			ApplyCylinderTransform(node, transform);
		}
		else if (parent->topo == kNPtopoPin || parent->topo == kNPtopoRod) {

			ApplyPinTransform(node, transform);
		}
		else if (parent->topo == kNPtopoTorus) {

			ApplyTorusTransform(node, transform);
		}

		//offset height to keep object at the surface, several exceptions...
		if (parent->topo != kNPtopoPin && parent->topo != kNPtopoRod)
		{
			if (node->topo == kNPtopoCube)
				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, kNPoffsetCube));
			else if (node->topo == kNPtopoTorus)
			{
				if (parent->topo != kNPtopoTorus)
					transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, kNPoffsetTorus));
			}
			else if (node->topo != kNPtopoPin && node->topo != kNPtopoRod)
				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, kNPoffsetUnit));
		}

		if (node->topo == kNPtopoPin && !(node->geometry == kNPgeoPin || node->geometry == kNPgeoPinWire))
		{
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, kNPoffsetPin));
		}
		else if (node->topo == kNPtopoRod)
		{					//width uses ratio, length uses scale 2 * 5 = 10
			transform = glm::scale(transform, glm::vec3(node->ratio * 2.0f, node->ratio * 2.0f, node->scale.z * kNPoffsetPin));
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, kNPoffsetUnit));
		}

		AddFinalTransform(node, transform);

		return transform;
	}

	void ANTzBoundingBox::ApplyCubeTransform(pNPnode node, glm::mat4& transform) {

		int i = 0;
		//if zero then calculate using id/6 + 1		//update to use parent->childIndex, debug zz
		if (node->facet == 0)
			i = node->id - 6 * (node->id / 6) + 1;	// +1 to start at 1
		else
			i = node->facet;

		// rotate orthogonal to face based on facet then translate
		switch (i) // 
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
		transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, kNPoffsetCube));	//translate from center to cube surface, explain, debug zz
		transform = glm::scale(transform, glm::vec3(kNPinvertDeg * kNPoffsetCube, kNPinvertDeg * kNPoffsetCube, kNPinvertDeg * kNPoffsetCube)); //scale face +/- 180 deg

		//now position node specific coordinates
		transform = glm::translate(transform, glm::vec3(node->translate.x, node->translate.y, node->translate.z));

		ApplyRotation(node, transform);

		//scale node up to half parent size
		transform = glm::scale(transform, glm::vec3(90.0f / kNPoffsetCube, 90.0f / kNPoffsetCube, 90.0f / kNPoffsetCube));

		if (node->topo != kNPtopoRod)		 //rod is scaled at draw time and does not pass along scale to children
			transform = glm::scale(transform, glm::vec3(node->scale.x, node->scale.y, node->scale.z));	//node scale
	}

	void ANTzBoundingBox::ApplySphereTransform(pNPnode node, glm::mat4& transform) {

		pNPnode parent = node->parent;

		transform = glm::rotate(transform, glm::radians(node->translate.x + 180.0f), glm::vec3(0.0f, 0.0f, 1.0f));	//longitude, +180 so N = 0
		transform = glm::rotate(transform, glm::radians(node->translate.y - 90.f), glm::vec3(-1.0f, 0.0f, 0.0f));		//latitude

		//translate 1 unit to surface for sphere, convert node z local units
		if (parent->topo == kNPtopoPoint)
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, node->translate.z)); //point has no offset
		else
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 1.0f + node->translate.z / (4.0f*kRADtoDEG)));

		//orientation
		ApplyRotation(node, transform);

		//scale node * ratio based on topo type
		if (parent->topo == kNPtopoPoint)
			transform = glm::scale(transform, glm::vec3(node->scale.x, node->scale.y, node->scale.z));
		else if (node->topo == kNPtopoRod)		 //rod is scaled at draw time and does not pass along scale to children
			transform = glm::scale(transform, glm::vec3(0.25f, 0.25f, 0.25f));	//scale 1/4 size of parent
		else
			transform = glm::scale(transform, glm::vec3(node->scale.x * 0.25f, node->scale.y * 0.25f, node->scale.z * 0.25f));	//node scale
	}

	void ANTzBoundingBox::ApplyCylinderTransform(pNPnode node, glm::mat4& transform) {

		transform = glm::rotate(transform, glm::radians(node->translate.x + 180.0f), glm::vec3(0.0f, 0.0f, 1.0f));	//longitude, +180 so N = 0
		transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));	//orient perpendicular to cyl

		//latitude plus translate 1 unit to surface adding converted node z local units
		transform = glm::translate(transform, glm::vec3(0.0, node->translate.y / 90.0f, 1.0f + node->translate.z / (4.0f*kRADtoDEG)));

		//orientation
		ApplyRotation(node, transform);

		//scale 1/4 size of parent
		if (node->topo == kNPtopoRod)		 //rod is scaled at draw time and does not pass along scale to children
			transform = glm::scale(transform, glm::vec3(0.25f, 0.25f, 0.25f));
		else
			transform = glm::scale(transform, glm::vec3(node->scale.x * 0.25f, node->scale.y * 0.25f, node->scale.z * 0.25f));			//node scale
	}

	void ANTzBoundingBox::ApplyPinTransform(pNPnode node, glm::mat4& transform) {

		pNPnode parent = node->parent;

		//position the node vertically on the pin, note using translate.x
		if (node->branchLevel == 1 && parent->topo != kNPtopoRod)
			//zzoff
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, kNPoffsetPin + node->translate.x / 37.22f));	//root pin workaround, zz debug
		else if (parent->topo == kNPtopoRod)	//replace kNPoffsetPin with
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, parent->scale.z * (kNPoffsetRod + 2.0f * node->translate.x / 37.22f)));
		else
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, kNPoffsetPin + node->translate.x / 37.22f));

		if (node->topo == kNPtopoPin && parent->topo == kNPtopoPin)
			transform = glm::scale(transform, glm::vec3(node->scale.x * 0.5f, node->scale.y * 0.5f, node->scale.z * 0.5f));
		else if (node->topo != kNPtopoRod)		 //rod is scaled at draw time and does not pass along scale to children
			transform = glm::scale(transform, glm::vec3(node->scale.x, node->scale.y, node->scale.z));	//rod does not scale child

		//offset from center after scaling, preserves ring offset when scaling
		transform = glm::translate(transform, glm::vec3(node->translate.z * .008333333f, 0.0f, 0.0f)); // node->translate.y / -37.22f, 0.0f); //locked y, makes more sense when swtiching topos

		ApplyRotation(node, transform);
	}

	void ANTzBoundingBox::ApplyTorusTransform(pNPnode node, glm::mat4& transform) {

		pNPnode parent = node->parent;

		//scale up 1.5X to match torus geometry size inner radius
		transform = glm::scale(transform, glm::vec3(kNPtorusRadius, kNPtorusRadius, kNPtorusRadius));

		//position at torus outer radius, inside center of tube
		transform = glm::rotate(transform, glm::radians(node->translate.x + 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::translate(transform, glm::vec3(1.0f, 0.0f, 0.0f));	//translate to center of tube

		//treat null as torus, later make this depend on the geometry
		if (node->topo == kNPtopoTorus)
		{
			transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::rotate(transform, glm::radians(node->translate.y), glm::vec3(0.0f, 0.0f, 1.0f));	//latitude
			//translate.z based on scale.x and is converted from deg 57.29...
			transform = glm::translate(transform, glm::vec3(0.0f, 0.1591f * node->scale.x * node->translate.z / kRADtoDEG, 0.0f));
		}
		else
		{
			transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(node->translate.y), glm::vec3(0.0f, -1.0f, 0.0f));	//latitude

			transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));				//orient North
			//translate 1 unit to surface then convert node z local units
			//uses parent->ratio for torus inner radius and 1.5f for scaling factor
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, parent->ratio + (node->translate.z / (k2PI * kRADtoDEG))));

			transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));	//results in 1/4 parent
		}
		//orientation
		ApplyRotation(node, transform);

		//scale 1/2 size of parent, smaller is good for torus .5 / 1.5
		if (node->topo == kNPtopoRod)		 //rod is scaled at draw time and does not pass along scale to children
			transform = glm::scale(transform, glm::vec3(0.33333333f, 0.33333333f, 0.33333333f));
		else
			transform = glm::scale(transform, glm::vec3(node->scale.x * 0.33333333f, node->scale.y * 0.33333333f, node->scale.z * 0.33333333f));	//node scale
	}

	void ANTzBoundingBox::ApplyRotation(pNPnode node, glm::mat4& transform) {

		transform = glm::rotate(transform, glm::radians(node->rotate.y), glm::vec3(0.0f, 0.0f, -1.0f));
		transform = glm::rotate(transform, glm::radians(node->rotate.x), glm::vec3(-1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(node->rotate.z), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	void ANTzBoundingBox::AddFinalTransform(pNPnode node, glm::mat4& transform) {

		if ((node->geometry == kNPgeoCylinder || node->geometry == kNPgeoCylinderWire
			|| node->geometry == kNPgeoCone || node->geometry == kNPgeoConeWire)
			|| (node->topo != kNPtopoPin
			&& (node->geometry == kNPgeoPin
			|| node->geometry == kNPgeoPinWire))) {

			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, -kNPoffsetUnit));
		}
	}

} //namespace SynGlyphXANTz