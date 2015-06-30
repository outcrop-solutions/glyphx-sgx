#include "datamappingglyphgeometry.h"

namespace SynGlyphX {

	DataMappingGlyphGeometry::DataMappingGlyphGeometry() :
		GlyphStructuralProperties < GeometryShapeMappingProperty >() {
	
	}

	DataMappingGlyphGeometry::DataMappingGlyphGeometry(const DataMappingGlyphGeometry& glyphGeometry) :
		GlyphStructuralProperties < GeometryShapeMappingProperty >(glyphGeometry) {


	}

	DataMappingGlyphGeometry::DataMappingGlyphGeometry(const GlyphGeometry& glyphGeometry) :
		GlyphStructuralProperties < GeometryShapeMappingProperty >(GeometryShapeMappingProperty(glyphGeometry.GetGeometryShape()), glyphGeometry.GetGeometrySurface()) {

		SetTorusRatio(glyphGeometry.GetTorusRatio());
	}

	DataMappingGlyphGeometry::DataMappingGlyphGeometry(const boost::property_tree::wptree& propertyTree, bool useOldPropertyTree) :
		GlyphStructuralProperties < GeometryShapeMappingProperty >() {

		if (useOldPropertyTree) {

			m_geometryShape = GeometryShapeMappingProperty(propertyTree.get<GlyphGeometryInfo::Shape>(L"<xmlattr>.Shape"));
			m_geometrySurface = propertyTree.get<GlyphGeometryInfo::Surface>(L"<xmlattr>.Surface");
		}
		else {

			m_geometryShape = GeometryShapeMappingProperty(propertyTree.get_child(L"Shape"));
			m_geometrySurface = propertyTree.get<GlyphGeometryInfo::Surface>(L"Surface");
		}
			
		m_torusRatio = propertyTree.get_optional<double>(L"Ratio").get_value_or(0.1);
	}

	DataMappingGlyphGeometry::~DataMappingGlyphGeometry()
	{
	}

	DataMappingGlyphGeometry& DataMappingGlyphGeometry::operator=(const DataMappingGlyphGeometry& glyphGeometry) {

		GlyphStructuralProperties<GeometryShapeMappingProperty>::operator=(glyphGeometry);

		return *this;
	}

	bool DataMappingGlyphGeometry::operator==(const DataMappingGlyphGeometry& glyphGeometry) const {

		return GlyphStructuralProperties<GeometryShapeMappingProperty>::operator==(glyphGeometry);
	}

	bool DataMappingGlyphGeometry::operator!=(const DataMappingGlyphGeometry& glyphGeometry) const {

		return !operator==(glyphGeometry);
	}

	boost::property_tree::wptree& DataMappingGlyphGeometry::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& geometryPropertyTree = propertyTree.add(L"Geometry", L"");

		boost::property_tree::wptree& shapePropertyTree = geometryPropertyTree.add(L"Shape", L"");
		m_geometryShape.ExportToPropertyTree(shapePropertyTree);

		propertyTree.put<GlyphGeometryInfo::Surface>(L"Surface", m_geometrySurface);
		geometryPropertyTree.put<double>(L"Ratio", m_torusRatio);

		return geometryPropertyTree;
	}

	GlyphGeometry DataMappingGlyphGeometry::ExportGlyphGeometry() const {

		GlyphGeometry glyphGeometry;
		glyphGeometry.SetGeometryShape(m_geometryShape.GetValue());
		glyphGeometry.SetGeometrySurface(m_geometrySurface);
		glyphGeometry.SetTorusRatio(m_torusRatio);

		return glyphGeometry;
	}

} //namespace SynGlyphX