#include "glyphstructuralproperties.h"

namespace SynGlyphX {

	template <typename GeometryShapeType>
	GlyphStructuralProperties<GeometryShapeType>::GlyphStructuralProperties(const GeometryShapeType& shape, GlyphGeometryInfo::Surface surface) :
		m_geometryShape(shape),
		m_geometrySurface(surface),
		m_torusRatio(0.1)
	{
	}
	/*
	template <typename GeometryShapeType>
	GlyphStructuralProperties<GeometryShapeType>::GlyphStructuralProperties(const boost::property_tree::wptree& propertyTree) :
		m_geometryShape(s_shapeNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Shape"))),
		m_geometrySurface(s_surfaceNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Surface"))),
		//m_virtualTopology(s_virtualTopologyNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Topology"))),
		m_torusRatio(propertyTree.get_optional<double>(L"Ratio").get_value_or(0.1)) {

	}*/

	template <typename GeometryShapeType>
	GlyphStructuralProperties<GeometryShapeType>::GlyphStructuralProperties(const GlyphStructuralProperties& properties) :
		m_geometryShape(properties.m_geometryShape),
		m_geometrySurface(properties.m_geometrySurface),
		m_torusRatio(properties.m_torusRatio) {


	}

	template <typename GeometryShapeType>
	GlyphStructuralProperties<GeometryShapeType>::~GlyphStructuralProperties()
	{
	}

	template <typename GeometryShapeType>
	GlyphStructuralProperties<GeometryShapeType>& GlyphStructuralProperties<GeometryShapeType>::operator=(const GlyphStructuralProperties& properties) {

		m_geometryShape = properties.m_geometryShape;
		m_geometrySurface = properties.m_geometrySurface;
		m_torusRatio = properties.m_torusRatio;

		return *this;
	}

	template <typename GeometryShapeType>
	bool GlyphStructuralProperties<GeometryShapeType>::operator==(const GlyphStructuralProperties& properties) const {

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

	template <typename GeometryShapeType>
	bool GlyphStructuralProperties<GeometryShapeType>::operator!=(const GlyphStructuralProperties& properties) const {

		return !operator==(properties);
	}

	template <typename GeometryShapeType>
	void GlyphStructuralProperties<GeometryShapeType>::SetGeometryShape(const GeometryShapeType& shape) {

		m_geometryShape = shape;
	}

	template <typename GeometryShapeType>
	const GeometryShapeType& GlyphStructuralProperties<GeometryShapeType>::GetGeometryShape() const {

		return m_geometryShape;
	}

	template <typename GeometryShapeType>
	void GlyphStructuralProperties<GeometryShapeType>::SetGeometrySurface(GlyphGeometryInfo::Surface surface) {

		m_geometrySurface = surface;
	}

	template <typename GeometryShapeType>
	GlyphGeometryInfo::Surface GlyphStructuralProperties<GeometryShapeType>::GetGeometrySurface() const {

		return m_geometrySurface;
	}

	template <typename GeometryShapeType>
	void GlyphStructuralProperties<GeometryShapeType>::SetTorusRatio(double ratio) {

		m_torusRatio = ratio;
	}

	template <typename GeometryShapeType>
	double GlyphStructuralProperties<GeometryShapeType>::GetTorusRatio() const {

		return m_torusRatio;
	}
	/*
	template <typename GeometryShapeType>
	boost::property_tree::wptree& GlyphStructuralProperties<GeometryShapeType>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		propertyTree.put<std::wstring>(L"<xmlattr>.Shape", s_shapeNames.left.at(m_geometryShape));
		propertyTree.put<std::wstring>(L"<xmlattr>.Surface", s_surfaceNames.left.at(m_geometrySurface));
		propertyTree.put<std::wstring>(L"<xmlattr>.Topology", s_virtualTopologyNames.left.at(m_virtualTopology));
		propertyTree.put<double>(L"Ratio", m_torusRatio);

		return propertyTree;
	}*/

	template class GlyphStructuralProperties < GlyphGeometryInfo::Shape > ;

} //namespace SynGlyphX