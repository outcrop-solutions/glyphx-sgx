#include "GlyphObject.h"
#include <boost/property_tree/xml_parser.hpp>
#include <QDebug>

namespace GlyphEngine {

	GlyphObject::GlyphObject(const boost::property_tree::wptree& propertyTree)
	{
		boost::uuids::uuid treeID = propertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id");

		m_label = propertyTree.get_optional<int>(L"<xmlattr>.label").get_value_or(0);
		bool merge = propertyTree.get_optional<bool>(L"<xmlattr>.merge").get_value_or(false);

		ImportPosition(propertyTree.get_child(L"Position"));
		ImportScale(propertyTree.get_child(L"Scale"));
		ImportColor(propertyTree.get_child(L"Color"));
		ImportTag(propertyTree.get_child(L"Tag"));
		ImportGeometry(propertyTree.get_child(L"Geometry"));
		
	}

	void GlyphObject::ImportPosition(const boost::property_tree::wptree& positionPropertyTree)
	{
		const boost::property_tree::wptree x = positionPropertyTree.get_child(L"X");
		std::wstring x_min = x.get<std::wstring>(L"Min");
		std::wstring x_diff = x.get_optional<std::wstring>(L"Difference").get_value_or(L"0");
		std::wstring x_bind;
		boost::optional<const boost::property_tree::wptree&> x_binding = x.get_child_optional(L"Binding");
		if (x_binding.is_initialized()) {
			x_bind = x_binding.get().get<std::wstring>(L"<xmlattr>.fieldId");
		}
		std::wstring x_type = x.get_child(L"Function").get<std::wstring>(L"<xmlattr>.type");

		const boost::property_tree::wptree y = positionPropertyTree.get_child(L"Y");
		std::wstring y_min = y.get<std::wstring>(L"Min");
		std::wstring y_diff = y.get_optional<std::wstring>(L"Difference").get_value_or(L"0");
		std::wstring y_bind;
		boost::optional<const boost::property_tree::wptree&> y_binding = y.get_child_optional(L"Binding");
		if (y_binding.is_initialized()) {
			y_bind = y_binding.get().get<std::wstring>(L"<xmlattr>.fieldId");
		}
		std::wstring y_type = y.get_child(L"Function").get<std::wstring>(L"<xmlattr>.type");

		const boost::property_tree::wptree z = positionPropertyTree.get_child(L"Z");
		std::wstring z_min = z.get<std::wstring>(L"Min");
		std::wstring z_diff = z.get_optional<std::wstring>(L"Difference").get_value_or(L"0");
		std::wstring z_bind;
		boost::optional<const boost::property_tree::wptree&> z_binding = z.get_child_optional(L"Binding");
		if (z_binding.is_initialized()) {
			z_bind = z_binding.get().get<std::wstring>(L"<xmlattr>.fieldId");
		}
		std::wstring z_type = z.get_child(L"Function").get<std::wstring>(L"<xmlattr>.type");

		m_position.x.create(x_min, x_diff, x_bind, x_type);
		m_position.y.create(y_min, y_diff, y_bind, y_type);
		m_position.z.create(z_min, z_diff, z_bind, z_type);

		qDebug() << "PX Min:" << m_position.x.min;
		qDebug() << "PX Diff:" << m_position.x.diff;
		qDebug() << "PX Bind:" << m_position.x.bind;
		qDebug() << "PX Type:" << m_position.x.type;

		qDebug() << "PY Min:" << m_position.y.min;
		qDebug() << "PY Diff:" << m_position.y.diff;
		qDebug() << "PY Bind:" << m_position.y.bind;
		qDebug() << "PY Type:" << m_position.y.type;

		qDebug() << "PZ Min:" << m_position.z.min;
		qDebug() << "PZ Diff:" << m_position.z.diff;
		qDebug() << "PZ Bind:" << m_position.z.bind;
		qDebug() << "PZ Type:" << m_position.z.type;

	}

	void GlyphObject::ImportScale(const boost::property_tree::wptree& scalePropertyTree)
	{
		const boost::property_tree::wptree x = scalePropertyTree.get_child(L"X");
		std::wstring x_min = x.get<std::wstring>(L"Min");
		std::wstring x_diff = x.get_optional<std::wstring>(L"Difference").get_value_or(L"0");
		std::wstring x_bind;
		boost::optional<const boost::property_tree::wptree&> x_binding = x.get_child_optional(L"Binding");
		if (x_binding.is_initialized()) {
			x_bind = x_binding.get().get<std::wstring>(L"<xmlattr>.fieldId");
		}
		std::wstring x_type = x.get_child(L"Function").get<std::wstring>(L"<xmlattr>.type");

		const boost::property_tree::wptree y = scalePropertyTree.get_child(L"Y");
		std::wstring y_min = y.get<std::wstring>(L"Min");
		std::wstring y_diff = y.get_optional<std::wstring>(L"Difference").get_value_or(L"0");
		std::wstring y_bind;
		boost::optional<const boost::property_tree::wptree&> y_binding = y.get_child_optional(L"Binding");
		if (y_binding.is_initialized()) {
			y_bind = y_binding.get().get<std::wstring>(L"<xmlattr>.fieldId");
		}
		std::wstring y_type = y.get_child(L"Function").get<std::wstring>(L"<xmlattr>.type");

		const boost::property_tree::wptree z = scalePropertyTree.get_child(L"Z");
		std::wstring z_min = z.get<std::wstring>(L"Min");
		std::wstring z_diff = z.get_optional<std::wstring>(L"Difference").get_value_or(L"0");
		std::wstring z_bind;
		boost::optional<const boost::property_tree::wptree&> z_binding = z.get_child_optional(L"Binding");
		if (z_binding.is_initialized()) {
			z_bind = z_binding.get().get<std::wstring>(L"<xmlattr>.fieldId");
		}
		std::wstring z_type = z.get_child(L"Function").get<std::wstring>(L"<xmlattr>.type");

		m_scale.x.create(x_min, x_diff, x_bind, x_type);
		m_scale.y.create(y_min, y_diff, y_bind, y_type);
		m_scale.z.create(z_min, z_diff, z_bind, z_type);

		qDebug() << "SX Min:" << m_scale.x.min;
		qDebug() << "SX Diff:" << m_scale.x.diff;
		qDebug() << "SX Bind:" << m_scale.x.bind;
		qDebug() << "SX Type:" << m_scale.x.type;

		qDebug() << "SY Min:" << m_scale.y.min;
		qDebug() << "SY Diff:" << m_scale.y.diff;
		qDebug() << "SY Bind:" << m_scale.y.bind;
		qDebug() << "SY Type:" << m_scale.y.type;

		qDebug() << "SZ Min:" << m_scale.z.min;
		qDebug() << "SZ Diff:" << m_scale.z.diff;
		qDebug() << "SZ Bind:" << m_scale.z.bind;
		qDebug() << "SZ Type:" << m_scale.z.type;
	}

	void GlyphObject::ImportColor(const boost::property_tree::wptree& colorPropertyTree)
	{
		const boost::property_tree::wptree rgb = colorPropertyTree.get_child(L"RGB");
		std::wstring rgb_min = rgb.get<std::wstring>(L"Min");
		std::wstring rgb_diff = rgb.get<std::wstring>(L"Difference");
		std::wstring rgb_bind;
		boost::optional<const boost::property_tree::wptree&> rgb_binding = rgb.get_child_optional(L"Binding");
		if (rgb_binding.is_initialized()) {
			rgb_bind = rgb_binding.get().get<std::wstring>(L"<xmlattr>.fieldId");
		}
		std::wstring rgb_type = rgb.get_child(L"Function").get<std::wstring>(L"<xmlattr>.type");

		const boost::property_tree::wptree transparency = colorPropertyTree.get_child(L"Transparency");
		std::wstring t_min = transparency.get<std::wstring>(L"Min");
		std::wstring t_diff = transparency.get<std::wstring>(L"Difference");
		std::wstring t_bind;
		boost::optional<const boost::property_tree::wptree&> transparency_binding = transparency.get_child_optional(L"Binding");
		if (transparency_binding.is_initialized()) {
			t_bind = transparency_binding.get().get<std::wstring>(L"<xmlattr>.fieldId");
		}
		std::wstring t_type = rgb.get_child(L"Function").get<std::wstring>(L"<xmlattr>.type");

		m_color.rgb.create(rgb_min, rgb_diff, rgb_bind, rgb_type);
		m_color.tp.create(t_min, t_diff, t_bind, t_type);

		qDebug() << "RGB Min:" << m_color.rgb.min;
		qDebug() << "RGB Diff:" << m_color.rgb.diff;
		qDebug() << "RGB Bind:" << m_color.rgb.bind;
		qDebug() << "RGB Type:" << m_color.rgb.type;

		qDebug() << "T Min:" << m_color.tp.min;
		qDebug() << "T Diff:" << m_color.tp.diff;
		qDebug() << "T Bind:" << m_color.tp.bind;
		qDebug() << "T Type:" << m_color.tp.type;

	}

	void GlyphObject::ImportTag(const boost::property_tree::wptree& tagPropertyTree)
	{
		boost::optional<const boost::property_tree::wptree&> binding = tagPropertyTree.get_child_optional(L"Binding");
		if (binding.is_initialized()) {
			m_tag = binding.get().get<std::wstring>(L"<xmlattr>.fieldId");
			qDebug() << "Tag:" << m_tag;
		}
	}

	void GlyphObject::ImportGeometry(const boost::property_tree::wptree& geometryPropertyTree)
	{
		const boost::property_tree::wptree shape = geometryPropertyTree.get_child(L"Shape");
		std::wstring value = shape.get<std::wstring>(L"Value");
		std::wstring type = shape.get_child(L"Function").get<std::wstring>(L"<xmlattr>.type");
		std::wstring surface = geometryPropertyTree.get<std::wstring>(L"Surface");
		std::wstring ratio = geometryPropertyTree.get<std::wstring>(L"Ratio");

		m_geometry = new Geometry(value, type, surface, ratio);

		qDebug() << "Shape:" << m_geometry->value;
		qDebug() << "S Bind:" << m_geometry->type;
		qDebug() << "Surface:" << m_geometry->surface;
		qDebug() << "Ratio:" << m_geometry->ratio;
	}

	int GlyphObject::GetLabel()
	{
		return m_label;
	}

} 