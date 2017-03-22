#include "Glyph3DSceneExport.h"
#include "SceneViewer/glyphscene.h"
#include "SceneViewer/glyphnode.h"
#include "datatransformmapping.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../../ANTzPlus/ANTzTopology/ANTzTopology.h"
#include "stringconvert.h"

namespace SynGlyphX {

	void Glyph3DSceneExport::ExportMaxGlyphTo3DScene(const DataMappingGlyphGraph& dataMappingGlyph, GlyphScene& scene, SynGlyphX::DataTransformMapping* mapping) {

		SynGlyphX::GlyphGraph::SharedPtr maxGlyph = dataMappingGlyph.GetMaxGlyphTree();
		ExportGlyphTo3DScene(dataMappingGlyph, maxGlyph, maxGlyph->GetRoot().constify(), nullptr, scene, mapping);
	}

	void Glyph3DSceneExport::ExportMinGlyphTo3DScene(const DataMappingGlyphGraph& dataMappingGlyph, GlyphScene& scene, SynGlyphX::DataTransformMapping* mapping) {

		SynGlyphX::GlyphGraph::SharedPtr minGlyph = dataMappingGlyph.GetMinGlyphTree();
		ExportGlyphTo3DScene(dataMappingGlyph, minGlyph, minGlyph->GetRoot().constify(), nullptr, scene, mapping);
	}

	void Glyph3DSceneExport::ExportLegendGlyphTo3DScene(const DataMappingGlyphGraph& dataMappingGlyph, GlyphScene& scene, SynGlyphX::DataTransformMapping* mapping) {

		SynGlyphX::GlyphGraph::SharedPtr legendGlyph = dataMappingGlyph.GetLegendGlyphTree();
		ExportGlyphTo3DScene(dataMappingGlyph, legendGlyph, legendGlyph->GetRoot().constify(), nullptr, scene, mapping);
	}

	void Glyph3DSceneExport::ExportGlyphTo3DScene(const DataMappingGlyphGraph& dataMappingGlyph, 
		GlyphGraph::ConstSharedPtr glyph,
		const GlyphGraph::ConstGlyphIterator& node,
		Glyph3DNode* parent,
		GlyphScene& scene,
		SynGlyphX::DataTransformMapping* mapping) {

		auto label = node->first;
		auto* glyphnode = scene.allocGlyph(node->first, parent == nullptr, SynGlyphX::Glyph3DNodeType::GlyphElement, -1, label);

		VirtualTopologyInfo::Type topo = node->second.GetVirtualTopology().GetType();

		SetupGeometry(node->second.GetStructure(), *glyphnode);
		glyphnode->setColor(GetColor(node->second.GetColor(), node->second.GetTransparency()));
		glyphnode->setPlacementPolicy(ChoosePlacementPolicy(topo));

		if (mapping)
		{
			auto _input_field = node->second.GetTag();
			if (!_input_field.empty())
			{
				SynGlyphX::InputTable table = mapping->GetInputTable(dataMappingGlyph);
				auto fieldToAliasMap = mapping->GetFieldToAliasMapForTable(table);
				SynGlyphX::InputField field = mapping->GetInputFieldManager()->GetInputField(_input_field);
				if (field.IsValid()) {

					auto fieldName = StringConvert::ToStdString(field.GetField());
					std::string tag;
					if (fieldToAliasMap.count(field.GetField()) == 0) {
						tag = fieldName;
					}
					else {
						tag = StringConvert::ToStdString(fieldToAliasMap[field.GetField()]);
					}
					glyphnode->setString(GlyphStringType::Tag, scene.createString(tag.c_str()));
				}
			}
		}

		glm::vec3 pos(0.f);
		if (parent != nullptr) {

			pos = glm::vec3(node->second.GetPosition()[0], node->second.GetPosition()[1], node->second.GetPosition()[2]);
		}
		glm::vec3 rot(node->second.GetRotation()[0], node->second.GetRotation()[1], node->second.GetRotation()[2]);
		glm::vec3 scale(node->second.GetScale()[0], node->second.GetScale()[1], node->second.GetScale()[2]);

		glyphnode->setLocalPosition(pos);
		glyphnode->setLocalRotation(rot);
		glyphnode->setLocalScale(scale);
		double ratio = node->second.GetStructure().GetTorusRatio();
		glyphnode->setTorusRatio(ratio);

		if (node->second.GetRotationRate()[0] != 0.f)
		{
			glyphnode->setAnimationAxis(glm::vec3(1.f, 0.f, 0.f));
			glyphnode->setAnimationRate(node->second.GetRotationRate()[0]);
		}
		else if (node->second.GetRotationRate()[1] != 0.f)
		{
			glyphnode->setAnimationAxis(glm::vec3(0.f, 1.f, 0.f));
			glyphnode->setAnimationRate(node->second.GetRotationRate()[1]);
		}
		else if (node->second.GetRotationRate()[2] != 0.f)
		{
			glyphnode->setAnimationAxis(glm::vec3(0.f, 0.f, 1.f));
			glyphnode->setAnimationRate(node->second.GetRotationRate()[2]);
		}

		if (parent == nullptr) { //if is root
		
			glm::mat4 transform = glm::translate(glm::mat4(), pos);
			glm::vec3 visual_scale(1.f, 1.f, 1.f);
			transform = glm::rotate(transform, glm::radians(rot.y), glm::vec3(0.0f, 0.0f, -1.0f));
			transform = glm::rotate(transform, glm::radians(rot.x), glm::vec3(-1.0f, 0.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, -1.0f));
			if (topo == VirtualTopologyInfo::Type::LineRod)
			{
				visual_scale = glm::vec3(ratio * 2.0f, ratio * 2.0f, scale.z * 5.f);
				transform = glm::translate(transform, glm::vec3(0.f, 0.f, 1.f));
			}
			else
			{
				transform = glm::scale(transform, scale);
			}

			GlyphGeometryInfo::Shape geom_type = node->second.GetStructure().GetGeometryShape();
			if ((geom_type == GlyphGeometryInfo::Shape::Cylinder || geom_type == GlyphGeometryInfo::Shape::Cone)
				|| (topo != VirtualTopologyInfo::Type::LinePin && (geom_type == GlyphGeometryInfo::Shape::Pin))) {

				transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, -1.f));
			}

			glyphnode->setCachedTransform(transform);
			glyphnode->setVisualScale(visual_scale);
		}
		else {
		
			parent->setChild(glyphnode);
		}
		scene.add(glyphnode);

		for (unsigned int i = 0; i < glyph->ChildCount(node); ++i) {

			ExportGlyphTo3DScene(dataMappingGlyph, glyph, glyph->GetChild(node, i), glyphnode, scene, mapping);
		}
	}

	void Glyph3DSceneExport::SetupGeometry(const GlyphGeometry& geometry, Glyph3DNode& node) {
	
		switch (geometry.GetGeometryShape()) {
		
			case GlyphGeometryInfo::Cube:
				node.setGeometry(GlyphShape::Cube, geometry.GetGeometrySurface() == GlyphGeometryInfo::Surface::Wireframe); break;
			case GlyphGeometryInfo::Sphere:
				node.setGeometry(GlyphShape::Sphere, geometry.GetGeometrySurface() == GlyphGeometryInfo::Surface::Wireframe); break;
			case GlyphGeometryInfo::Cone:
				node.setGeometry(GlyphShape::Cone, geometry.GetGeometrySurface() == GlyphGeometryInfo::Surface::Wireframe); break;
			case GlyphGeometryInfo::Torus:
				node.setGeometry(GlyphShape::Torus, geometry.GetGeometrySurface() == GlyphGeometryInfo::Surface::Wireframe); break;
			case GlyphGeometryInfo::Dodecahedron:
				node.setGeometry(GlyphShape::Dodecahedron, geometry.GetGeometrySurface() == GlyphGeometryInfo::Surface::Wireframe); break;
			case GlyphGeometryInfo::Octahedron:
				node.setGeometry(GlyphShape::Octahedron, geometry.GetGeometrySurface() == GlyphGeometryInfo::Surface::Wireframe); break;
			case GlyphGeometryInfo::Tetrahedron:
				node.setGeometry(GlyphShape::Tetrahedron, geometry.GetGeometrySurface() == GlyphGeometryInfo::Surface::Wireframe); break;
			case GlyphGeometryInfo::Icosahedron:
				node.setGeometry(GlyphShape::Icosahedron, geometry.GetGeometrySurface() == GlyphGeometryInfo::Surface::Wireframe); break;
			case GlyphGeometryInfo::Pin:
				node.setGeometry(GlyphShape::Pin, geometry.GetGeometrySurface() == GlyphGeometryInfo::Surface::Wireframe); break;
			case GlyphGeometryInfo::Cylinder:
				node.setGeometry(GlyphShape::Cylinder, geometry.GetGeometrySurface() == GlyphGeometryInfo::Surface::Wireframe); break;
			default:
				assert(false);	// unrecognized geometry type
		}
	}

	glm::vec4 Glyph3DSceneExport::GetColor(const GlyphColor& color, double alpha) {

		return glm::vec4(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f, alpha / 255.f);
	}

	PlacementPolicy* Glyph3DSceneExport::ChoosePlacementPolicy(const VirtualTopologyInfo::Type& topo) {
	
		if (topo == VirtualTopologyInfo::Type::CubePlane) {

			return new SynGlyphXANTz::ANTzCubePlacementPolicy();
		}
		else if ((topo == VirtualTopologyInfo::Type::SphereNonZeroRadius) ||
			(topo == VirtualTopologyInfo::Type::SphereZeroRadius)) {

			return new SynGlyphXANTz::ANTzSpherePlacementPolicy(topo == VirtualTopologyInfo::Type::SphereZeroRadius);
		}
		else if (topo == VirtualTopologyInfo::Type::Circle) {

			return new SynGlyphXANTz::ANTzTorusPlacementPolicy();
		}
		else if (topo == VirtualTopologyInfo::Type::CylinderSide) {

			return new SynGlyphXANTz::ANTzCylinderPlacementPolicy();
		}
		else if ((topo == VirtualTopologyInfo::Type::LinePin) ||
			(topo == VirtualTopologyInfo::Type::LineRod)) {

			return new SynGlyphXANTz::ANTzPinPlacementPolicy(topo == VirtualTopologyInfo::Type::LineRod);
		}
		else {

			return new TestPlacementPolicy();
		}
	}

} //namespace SynGlyphX
