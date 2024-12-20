
#include "scenereader.h"
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <endian.h>
#include <hal/hal.h>
#include <render/grid_renderer.h>
#include "baseimagerenderer.h"
//#include "legacyglyphplacement.h"
#include "glyphscene.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <algorithm>
//#include "glm/ext.hpp"
//#include <hal/debug.h>
#include "AwsLogger.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <stdexcept>
namespace SynGlyphX
{
	namespace
	{
		const int32_t FORMAT_VERSION = 2;
		const int32_t SCENE_FILE_MAGIC_NUMBER = 0xa042bc3f;
		const int32_t COUNT_FILE_MAGIC_NUMBER = 0x294ee1ac;

		template<typename T> T read32_endian( FILE* f )
		{
			static_assert( sizeof( T ) == 4, "read32_endian template requires 32-bit parameter type" );
			T data;
			fread( &data, 4, 1, f );
			return endian32( data );
		}

		template<typename T> T read16_endian( FILE* f )
		{
			static_assert( sizeof( T ) == 2, "read16_endian template requires 32-bit parameter type" );
			T data;
			fread( &data, 2, 1, f );
			return endian16( data );
		}

		std::string read_str( FILE* f )
		{
			std::string result;
			auto len = read16_endian<short>( f );
			result.resize( len + 1 );
			fread( &result[0], 1, len, f );
			return result;
		}

		glm::mat4 get_rotation_matrix( const glm::vec3& rot )
		{
			// from antz. no idea.
			auto mat = glm::rotate( glm::mat4(), rot.y, glm::vec3( 0.f, 0.f, -1.f ) );
			mat = glm::rotate( mat, rot.x, glm::vec3( -1.f, 0.f, 0.f ) );
			mat = glm::rotate( mat, rot.z, glm::vec3( 0.f, 0.f, -1.f ) );
			return mat;
		}
		enum class GeomType : int
		{
			WIRE_CUBE = 0,
			CUBE = 1,
			WIRE_SPHERE = 2,
			SPHERE = 3,
			WIRE_CONE = 4,
			CONE = 5,
			WIRE_TORUS = 6,
			TORUS = 7,
			WIRE_DODECAHEDRON = 8,
			DODECAHEDRON = 9,
			WIRE_OCTAHEDRON = 10,
			OCTAHEDRON = 11,
			WIRE_TETRAHEDRON = 12,
			TETRAHEDRON = 13,
			WIRE_ICOSAHDRON = 14,
			ICOSAHEDRON = 15,
			PIN = 16,		// really?
			WIRE_PIN = 17,	// I mean...really?
			WIRE_CYLINDER = 18,
			CYLINDER = 19,
		};

		// copied from nptypes.h to avoid ANTz dependency 
		enum LegacyTopo
		{
			kNPtopoNull = 0,
			kNPtopoCube,
			kNPtopoSphere,
			kNPtopoTorus,
			kNPtopoCylinder,
			kNPtopoPin,
			kNPtopoRod,
			kNPtopoPoint,
			kNPtopoCount,
			kNPtopoCone,
			kNPtopoPlot,
			kNPtopoSurface,
			kNPtopoMesh
		};
	}

	void SetupGeometry( int id, Glyph3DNode& node )
	{
		switch ( GeomType( id ) )
		{
			case GeomType::CUBE:
				node.setGeometry( GlyphShape::Cube, false ); break;
			case GeomType::WIRE_CUBE:
				node.setGeometry( GlyphShape::Cube, true ); break;
			case GeomType::SPHERE:
				node.setGeometry( GlyphShape::Sphere, false ); break;
			case GeomType::WIRE_SPHERE:
				node.setGeometry( GlyphShape::Sphere, true ); break;
			case GeomType::CONE:
				node.setGeometry( GlyphShape::Cone, false ); break;
			case GeomType::WIRE_CONE:
				node.setGeometry( GlyphShape::Cone, true ); break;
			case GeomType::TORUS:
				node.setGeometry( GlyphShape::Torus, false ); break;
			case GeomType::WIRE_TORUS:
				node.setGeometry( GlyphShape::Torus, true ); break;
			case GeomType::DODECAHEDRON:
				node.setGeometry( GlyphShape::Dodecahedron, false ); break;
			case GeomType::WIRE_DODECAHEDRON:
				node.setGeometry( GlyphShape::Dodecahedron, true ); break;
			case GeomType::OCTAHEDRON:
				node.setGeometry( GlyphShape::Octahedron, false ); break;
			case GeomType::WIRE_OCTAHEDRON:
				node.setGeometry( GlyphShape::Octahedron, true ); break;
			case GeomType::TETRAHEDRON:
				node.setGeometry( GlyphShape::Tetrahedron, false ); break;
			case GeomType::WIRE_TETRAHEDRON:
				node.setGeometry( GlyphShape::Tetrahedron, true ); break;
			case GeomType::ICOSAHEDRON:
				node.setGeometry( GlyphShape::Icosahedron, false ); break;
			case GeomType::WIRE_ICOSAHDRON:
				node.setGeometry( GlyphShape::Icosahedron, true ); break;
			case GeomType::PIN:
				node.setGeometry( GlyphShape::Pin, false ); break;
			case GeomType::WIRE_PIN:
				node.setGeometry( GlyphShape::Pin, true ); break;
			case GeomType::CYLINDER:
				node.setGeometry( GlyphShape::Cylinder, false ); break;
			case GeomType::WIRE_CYLINDER:
				node.setGeometry( GlyphShape::Cylinder, true ); break;
			default:
				assert( false );	// unrecognized geometry type
		}
	}

	uint8_t SceneReader::read_byte()
	{
		uint8_t byte;
		fread( &byte, 1, 1, file );
		return byte;
	}

	int SceneReader::read_int()
	{
		return read32_endian<int>( file );
	}

	float SceneReader::read_float()
	{
		return read32_endian<float>( file );
	}

	std::string SceneReader::read_string()
	{
		return read_str( file );
	}

	render::packed_color SceneReader::read_packed_color()
	{
		render::packed_color c;
		fread( &c, 4, 1, file );
		return c;
	}

	render::packed_color SceneReader::read_packed_solid_color()
	{
		render::packed_color c = render::pack_color( 0, 0, 0, 255 );
		fread( &c, 3, 1, file );
		return c;
	}

	glm::vec3 SceneReader::read_vec3()
	{
		// colors are stored as integers, read and normalize to float
		glm::vec3 result;
		result.r = static_cast<float>( read_float() );
		result.g = static_cast<float>( read_float() );
		result.b = static_cast<float>( read_float() );
		return result;
	}

	void SceneReader::read_base_image( BaseImageRenderer& base_images, const std::vector<hal::texture*>& base_image_textures, hal::texture* default_base_texture, render::grid_renderer& grids )
	{
		auto tex_id = read_byte();
		glm::vec3 pos, rot;
		pos.x = read_float(); pos.y = read_float(); pos.z = read_float();
		rot.x = glm::radians( read_float() ); rot.y = glm::radians( read_float() ); rot.z = glm::radians( read_float() );
		auto color = read_packed_solid_color();
		float grid_cell_w = read_float(), grid_cell_h = read_float();
		int grid_cells_x = read_int(), grid_cells_y = read_int();
		//AwsLogger::getInstance()->localLogger("Read base images bytes");
		
		if ( tex_id != 0 )
		{
			auto transform = glm::translate( glm::mat4(), pos ) * get_rotation_matrix( rot );
			glm::vec2 size = glm::vec2( grid_cell_w * grid_cells_x, grid_cell_h * grid_cells_y );
			//AwsLogger::getInstance()->localLogger("transform and size");
			hal::texture* tex = default_base_texture;
			if ( tex_id > 1u )	// 1 = use default texture
			{
				auto offset_tex_id = tex_id - 2u;	// offset to account for 0 = none, 1 = default
				if ( offset_tex_id < base_image_textures.size() ) tex = base_image_textures[offset_tex_id];
				//AwsLogger::getInstance()->localLogger("tex_id > 1u");
			}
			base_images.add( tex, transform, size );
			//AwsLogger::getInstance()->localLogger("base_image add");

			if ( grid_cells_x >= 2 || grid_cells_y >= 2 )
				grids.add( transform, size, grid_cells_x, grid_cells_y, render::unpack_color( color ) );
			//AwsLogger::getInstance()->localLogger("grids add");
		}
		
	}
	QString SceneReader::getColumnName(QString column, QJsonObject object) {
		QJsonObject fieldObject = object.value(column).toObject();

		QStringList keys = fieldObject.keys();
		QString colName = keys[0];

		return colName;

	}
	QString SceneReader::getColumnValue(QString column, QJsonObject object) {
		QJsonObject fieldObject = object.value(column).toObject();

		QStringList keys = fieldObject.keys();
		QString colName = keys[0];

		QString value = fieldObject.value(colName).toString();

		return value;

	}
	QList<int> SceneReader::getRowIds(QJsonObject object) {
		QJsonArray rowIdArray = object.value("rowId").toArray();
		QVariantList rowIdList = rowIdArray.toVariantList();
		QList<int> rowIdIntList;
		for (QVariant v : rowIdList) {
			rowIdIntList.append(v.toInt());
		}

		return rowIdIntList;
	}

	void SceneReader::read_glyph_element( GlyphScene& scene )
	{
		GlyphPlacementData data;

		QString outputString;

		data.id = read_int();
		data.label = read_int();

		data.glyph_index = read_int();
		outputString += QString::number(data.id) + " , " + QString::number(data.label) + " , " + QString::number(data.glyph_index);

		if (data.glyph_index >= 8u)
			throw std::runtime_error("Glyph element index is too large; only up to 8 glyph types are supported.");

		data.parent_id = read_int();
		outputString += " , " + QString::number(data.parent_id);
		data.pos = read_vec3();
		outputString += " , " + QString::number(data.pos.x) + " , " + QString::number(data.pos.y) + " , " + QString::number(data.pos.z);
		data.rot = read_vec3();
		data.scale = read_vec3();
		data.color = read_packed_color();
		data.geom_type = read_byte();
		data.topo = read_byte();
		data.ratio = read_float();
		data.rotation_rates = read_vec3();

		glm::vec4 c = render::unpack_color(data.color);
		outputString += " , " + QString::number(c.r) + " , " + QString::number(c.g) + " , " + QString::number(c.b) + " , " + QString::number(c.a);

		//hal::debug::print("id: %d", data.id);
		//hal::debug::print("id: %d, parent_id: %d, position: %f, %f, %f, topo: %d", data.id, data.parent_id, data.pos.x, data.pos.y, data.pos.z, data.topo);

		std::string tag = read_string();
		std::string url = read_string();
		std::string desc = read_string();

		QString descQString = QString::fromStdString(desc);
		QByteArray descArray = descQString.toUtf8();
		//There is a \0 in the last element that is messing things up
		descArray.chop(1);
		QJsonDocument descDoc = QJsonDocument::fromJson(descArray);
		//AwsLogger::getInstance()->localLogger(outputString);
		QJsonObject descRootObject = descDoc.object();

		QString xColName = getColumnName("x", descRootObject);
		QString xColValue = getColumnValue("x", descRootObject);
		QString yColName = getColumnName("y", descRootObject);
		QString yColValue = getColumnValue("y", descRootObject);
		QString zColName = getColumnName("z", descRootObject);
		QString zColValue = getColumnValue("z", descRootObject);
		QList<int> rowIds = getRowIds(descRootObject);

		Glyph3DNode* parent = data.parent_id ? scene.getGlyph3D( data.parent_id ) : nullptr;
		data.is_root = ( data.parent_id == 0 ) || !parent;
		hal::debug::_assert( data.parent_id == 0 || parent, "glyph %i is parented to nonexistent glyph %i", data.id, data.parent_id );

		auto* glyphnode = scene.allocGlyph( data.id, data.is_root, Glyph3DNodeType::GlyphElement, data.is_root ? next_filtering_index++ : -1, data.label, data.glyph_index );
		SetupGeometry( data.geom_type, *glyphnode );
		glyphnode->setString( GlyphStringType::Tag, scene.createString( tag.c_str() ) );
		glyphnode->setString( GlyphStringType::Url, scene.createString( url.c_str() ) );
		glyphnode->setString( GlyphStringType::Desc, scene.createString( desc.c_str() ) );
		glyphnode->setColor( data.color );
		glyphnode->setPlacementPolicy( ChoosePlacementPolicy( data ) );

		allScaleZ.append(data.scale.z);
		colorMapping.insert(data.scale.z, data.color);
		filteringIndexMap.insert(data.id, data.id);

		//create map with key glm::vec3 and value of list of glyph ids
		QString uid = QString::number(data.pos.x) + QString::number(data.pos.y);
		if (!summation) {
			uid = QString::number(data.id);
		}
		indexToUID.insert(data.id, uid);
		if (stackedGlyphs.contains(uid)){
			stackedGlyphs[uid]->glyphIds.append(data.id);
			stackedGlyphs[uid]->scaleZ += data.scale.z;
			stackedGlyphs[uid]->posZ += data.pos.z;
			try {
				QString str = QString::fromStdString(tag).split(" ")[1];
				str.chop(1);
				double val = str.toDouble();
				stackedGlyphs[uid]->tagValue += val;
				tagValueMap.insert(data.id, val);
			}
			catch (...) {

			}
		}
		else {
			//TODO: Parse my JSON and Add my rowIds here.  Later We will add the string representations of the x,y and z values for display when 
			//selecting but this will get us started.
			stackedGlyphs.insert(uid, new StackedGlyph());
			stackedGlyphs[uid]->glyphIds.append(data.id);
			stackedGlyphs[uid]->scaleZ = data.scale.z;
			stackedGlyphs[uid]->posZ += data.pos.z;
			stackedGlyphs[uid]->gpd = data;
			stackedGlyphs[uid]->columnX = xColName;
			stackedGlyphs[uid]->columnY = yColName;
			stackedGlyphs[uid]->columnZ = zColName;
			stackedGlyphs[uid]->dataX = xColValue;
			stackedGlyphs[uid]->dataY = yColValue;
			stackedGlyphs[uid]->dataZ = zColValue;
			stackedGlyphs[uid]->glyphIds = rowIds;
			try {
				QString str = QString::fromStdString(tag).split(" ")[1];
				str.chop(1);
				double val = str.toDouble();
				stackedGlyphs[uid]->tagValue = val;
				tagValueMap.insert(data.id, val);
			}
			catch (...) {

			}
		}

		glyphnode->setLocalPosition( data.pos );
		glyphnode->setLocalRotation( data.rot );
		glyphnode->setLocalScale( data.scale );
		glyphnode->setTorusRatio( data.ratio );

		if ( data.rotation_rates.x != 0.f )
		{
			glyphnode->setAnimationAxis( glm::vec3( 1.f, 0.f, 0.f ) );
			glyphnode->setAnimationRate( data.rotation_rates.x );
		}
		else if ( data.rotation_rates.y != 0.f )
		{
			glyphnode->setAnimationAxis( glm::vec3( 0.f, 1.f, 0.f ) );
			glyphnode->setAnimationRate( data.rotation_rates.y );
		}
		else if ( data.rotation_rates.z != 0.f )
		{
			glyphnode->setAnimationAxis( glm::vec3( 0.f, 0.f, 1.f ) );
			glyphnode->setAnimationRate( data.rotation_rates.z );
		}

		if ( data.is_root )
		{
			glm::mat4 transform = glm::translate( glm::mat4(), data.pos );
			glm::vec3 visual_scale( 1.f, 1.f, 1.f );
			transform = glm::rotate( transform, glm::radians( data.rot.y ), glm::vec3( 0.0f, 0.0f, -1.0f ) );
			transform = glm::rotate( transform, glm::radians( data.rot.x ), glm::vec3( -1.0f, 0.0f, 0.0f ) );
			transform = glm::rotate( transform, glm::radians( data.rot.z ), glm::vec3( 0.0f, 0.0f, -1.0f ) );
			if ( data.topo == kNPtopoRod )
			{
				visual_scale = glm::vec3( data.ratio * 2.0f, data.ratio * 2.0f, data.scale.z * 5.f );
				transform = glm::translate( transform, glm::vec3( 0.f, 0.f, 1.f ) );
			}
			else
			{
				transform = glm::scale( transform, data.scale );
			}

			if ( ( data.geom_type == int( GeomType::CYLINDER ) || data.geom_type == int( GeomType::WIRE_CYLINDER )
				|| data.geom_type == int( GeomType::CONE ) || data.geom_type == int( GeomType::WIRE_CONE ) )
				|| ( data.topo != kNPtopoPin
					&& ( data.geom_type == int( GeomType::PIN )
						|| data.geom_type == int( GeomType::WIRE_PIN ) ) ) ) {

				transform = glm::translate( transform, glm::vec3( 0.0f, 0.0f, -1.f ) );
			}

			glyphnode->setCachedTransform( transform );
			glyphnode->setVisualScale( visual_scale );
			++root_count;
		}
		else
		{
			auto glyph_parent = scene.getGlyph3D( data.parent_id );
			assert( glyph_parent );	// this expects the file to always have parents before children
			glyph_parent->setChild( glyphnode );
		}
		scene.add( glyphnode );

		if ( data.id >= next_id )
			next_id = data.id + 1;
	}

	void SceneReader::create_stacked_glyph(QString uid, GlyphPlacementData data, std::string t, GlyphScene& scene)
	{
		try {
			indexToUID.insert(next_id, uid);
			data.id = next_id;
			data.label = next_id;
			if (data.glyph_index >= 8u)
				throw std::runtime_error("Glyph element index is too large; only up to 8 glyph types are supported.");

			/*if (indexOfFirstStacked != -1)
				indexOfFirstStacked = data.id;
			indexOfLastStacked = data.id;*/

			data.parent_id = 0;

			data.pos.z = stackedGlyphs[uid]->posZ;
			data.scale.z = stackedGlyphs[uid]->scaleZ;
			data.color = colorMapping[findClosest(allScaleZ.size(), stackedGlyphs[uid]->scaleZ)];

			//hal::debug::print("id: %d", data.id);
			//hal::debug::print("id: %d, parent_id: %d, position: %f, %f, %f, topo: %d", data.id, data.parent_id, data.pos.x, data.pos.y, data.pos.z, data.topo);

			std::string tag = t;
			std::string url = "nourl.html";
			std::string desc = "";

			Glyph3DNode* parent = data.parent_id ? scene.getGlyph3D(data.parent_id) : nullptr;
			data.is_root = (data.parent_id == 0) || !parent;
			//hal::debug::_assert(data.parent_id == 0 || parent, "glyph %i is parented to nonexistent glyph %i", data.id, data.parent_id);
			auto* glyphnode = scene.allocGlyph(data.id, data.is_root, Glyph3DNodeType::GlyphElement, data.is_root ? next_filtering_index++ : -1, data.label, data.glyph_index);
			SetupGeometry(data.geom_type, *glyphnode);

			glyphnode->setString(GlyphStringType::Tag, scene.createString(tag.c_str()));
			glyphnode->setString(GlyphStringType::Url, scene.createString(url.c_str()));
			glyphnode->setString(GlyphStringType::Desc, scene.createString(desc.c_str()));
			glyphnode->setColor(data.color);
			glyphnode->setPlacementPolicy(ChoosePlacementPolicy(data));
		
			glyphnode->setLocalPosition(data.pos);
			glyphnode->setLocalRotation(data.rot);
			glyphnode->setLocalScale(data.scale);
			glyphnode->setTorusRatio(data.ratio);
	
			if (data.rotation_rates.x != 0.f)
			{
				glyphnode->setAnimationAxis(glm::vec3(1.f, 0.f, 0.f));
				glyphnode->setAnimationRate(data.rotation_rates.x);
			}
			else if (data.rotation_rates.y != 0.f)
			{
				glyphnode->setAnimationAxis(glm::vec3(0.f, 1.f, 0.f));
				glyphnode->setAnimationRate(data.rotation_rates.y);
			}
			else if (data.rotation_rates.z != 0.f)
			{
				glyphnode->setAnimationAxis(glm::vec3(0.f, 0.f, 1.f));
				glyphnode->setAnimationRate(data.rotation_rates.z);
			}

			if (data.is_root)
			{
				glm::mat4 transform = glm::translate(glm::mat4(), data.pos);
				glm::vec3 visual_scale(1.f, 1.f, 1.f);
				transform = glm::rotate(transform, glm::radians(data.rot.y), glm::vec3(0.0f, 0.0f, -1.0f));
				transform = glm::rotate(transform, glm::radians(data.rot.x), glm::vec3(-1.0f, 0.0f, 0.0f));
				transform = glm::rotate(transform, glm::radians(data.rot.z), glm::vec3(0.0f, 0.0f, -1.0f));
				if (data.topo == kNPtopoRod)
				{
					visual_scale = glm::vec3(data.ratio * 2.0f, data.ratio * 2.0f, data.scale.z * 5.f);
					transform = glm::translate(transform, glm::vec3(0.f, 0.f, 1.f));
				}
				else
				{
					transform = glm::scale(transform, data.scale);
				}

				if ((data.geom_type == int(GeomType::CYLINDER) || data.geom_type == int(GeomType::WIRE_CYLINDER)
					|| data.geom_type == int(GeomType::CONE) || data.geom_type == int(GeomType::WIRE_CONE))
					|| (data.topo != kNPtopoPin
						&& (data.geom_type == int(GeomType::PIN)
							|| data.geom_type == int(GeomType::WIRE_PIN)))) {

					transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, -1.f));
				}

				glyphnode->setCachedTransform(transform);
				glyphnode->setVisualScale(visual_scale);
				++root_count;
			}
			else
			{
				auto glyph_parent = scene.getGlyph3D(data.parent_id);
				//assert(glyph_parent);	// this expects the file to always have parents before children
				glyph_parent->setChild(glyphnode);
			}
			scene.add(glyphnode);

			if (data.id >= next_id)
				next_id = data.id + 1;
		}
		catch (const std::exception& e) {
			AwsLogger::getInstance()->localLogger(e.what());
		}
	}

	void SceneReader::read_link( GlyphScene& scene )
	{
		int id0 = read_int();
		int id1 = read_int();
		auto* glyph0 = scene.getGlyph3D( id0 );
		auto* glyph1 = scene.getGlyph3D( id1 );

		auto geom_type = read_byte();
		auto color = read_packed_color();
		const float thickness = 0.1f;

		LinkProfile profile = ( geom_type == int( GeomType::CYLINDER ) ) ? LinkProfile::Circle : LinkProfile::Square;
		hal::debug::_assert( glyph0 && glyph1, "can't find glyph referenced by link" );

		if ( glyph0 && glyph1 )
		{
			// need linked glyphs to awwwhave up-to-date transforms so we know where to put the link
			glyph0->getRootParent()->updateCachedTransforms( scene.getGeomDB() );
			glyph1->getRootParent()->updateCachedTransforms( scene.getGeomDB() );
			auto pt0 = glyph0->getCachedPosition();
			auto pt1 = glyph1->getCachedPosition();
			glm::vec3 origin = ( pt0 + pt1 ) * 0.5f;
			auto translate = glm::translate( glm::mat4(), origin );
			auto direction = glm::normalize( pt1 - pt0 );
			auto up = glm::vec3( 0.f, 0.f, 1.f );
			// workaround for glm::orientation glitch when direction is almost but not QUITE the same as up
			// (oddly it works fine if they're exactly the same)
			if ( fabs( glm::dot( direction, up ) ) > 0.9999f ) direction = up;
			auto rotate = glm::orientation( direction, up );
			float length = glm::length( pt0 - pt1 );
			auto scale = glm::scale( glm::mat4(), glm::vec3( thickness, thickness, length ) );

			Glyph3DNode* linknode = scene.allocGlyph( next_id++, true, Glyph3DNodeType::Link );
			linknode->setCachedTransform( translate * rotate * scale );
			linknode->setVisualScale( glm::vec3( 1.f, 1.f, 1.f ) );
			linknode->setColor( color );
			linknode->setGeometry( profile == LinkProfile::Circle ? GlyphShape::Link_Cylinder : GlyphShape::Link_Cube );
			linknode->setLinkTargets( glyph0, glyph1 );
			scene.add( linknode );
		}
	}

	// quick and dirty class to add some RAII to FILE* so the handles get closed if read() throws
	namespace
	{
		class cfile
		{
		public:
			cfile( const char* name ) { f = fopen( name, "rb" ); }
			~cfile() { if ( f ) fclose( f ); }
			FILE* get() { return f; }
		private:
			FILE* f;
		};
	}

	void SceneReader::read( const char* scenefilename, const char* countfilename, GlyphScene& scene, BaseImageRenderer& base_images, const std::vector<hal::texture*>& base_image_textures, hal::texture* default_base_texture, render::grid_renderer& grids)
	{

		//indexOfFirstStacked = -1;
		//indexOfLastStacked = -1;

		root_count = 0u;
		next_filtering_index = 0;

		hal::debug::profile_timer timer;

		cfile count_file( countfilename );
		FILE* countfile = count_file.get();
		cfile scene_file( scenefilename );
		file = scene_file.get();

		indexToUID.clear();
		stackedGlyphs.clear();
		colorMapping.clear();
		allScaleZ.clear();
		filteringIndexMap.clear();
		tagValueMap.clear();

		AwsLogger::getInstance()->localLogger(countfilename);
		AwsLogger::getInstance()->localLogger(scenefilename);

		if ( countfile && file )
		{
			// Read counts from count file.
			int count_magic = read32_endian<int>( countfile );
			AwsLogger::getInstance()->localLogger(QString::number(count_magic) + " : " + QString::number(COUNT_FILE_MAGIC_NUMBER));
			if ( count_magic != COUNT_FILE_MAGIC_NUMBER )
				throw std::runtime_error( "Invalid or corrupt cached scene; try clearing your cache." );
			int count_format_version = read32_endian<int>( countfile );
			AwsLogger::getInstance()->localLogger(QString::number(count_format_version) + " : " + QString::number(FORMAT_VERSION));
			if ( count_format_version != FORMAT_VERSION )
				throw std::runtime_error( "Invalid or corrupt cached scene; try clearing your cache." );
			int base_image_count = read32_endian<int>( countfile );
			int glyph_count = read32_endian<int>( countfile );
			int link_count = read32_endian<int>( countfile );

			auto magic = read32_endian<int>( file );
			AwsLogger::getInstance()->localLogger(QString::number(magic) + " : " + QString::number(SCENE_FILE_MAGIC_NUMBER));
			if ( magic != SCENE_FILE_MAGIC_NUMBER )
				throw std::runtime_error( "Invalid or corrupt cached scene; try clearing your cache." );
			int format_version = read32_endian<int>( file );
			AwsLogger::getInstance()->localLogger(QString::number(format_version) + " : " + QString::number(FORMAT_VERSION));
			if ( format_version != FORMAT_VERSION )
				throw std::runtime_error( "Invalid or corrupt cached scene; try clearing your cache." );

			scene.beginAdding( glyph_count + link_count);
			AwsLogger::getInstance()->localLogger("Passed beginAdding");
			AwsLogger::getInstance()->localLogger(QString::number(base_image_count));
			for ( int i = 0; i < base_image_count; ++i )
				read_base_image( base_images, base_image_textures, default_base_texture, grids );
			AwsLogger::getInstance()->localLogger("Read base images");
			for ( int i = 0; i < glyph_count; ++i )
				read_glyph_element( scene );
			AwsLogger::getInstance()->localLogger("Read glyphs");
			for ( int i = 0; i < link_count; ++i )
				read_link( scene );
			AwsLogger::getInstance()->localLogger("Read links");

			std::sort(allScaleZ.begin(), allScaleZ.end());
			AwsLogger::getInstance()->localLogger("sort allScaleZ");

			AwsLogger::getInstance()->localLogger("# of stackedGlyphs: "+ QString::number(stackedGlyphs.size()));
			scene.addToGlyphAllocation(stackedGlyphs.size());
			for (auto uid : stackedGlyphs.keys()) {
					//out << "Group: " << uid << ", " << stackedGlyphs[uid].posZ << ", " << stackedGlyphs[uid].scaleZ << ", " << stackedGlyphs[uid].tagValue << endl;
					//AwsLogger::getInstance()->localLogger(uid);
					if (stackedGlyphs[uid]->currentGlyphIds.size() > 1) {
						for (int val : stackedGlyphs[uid]->currentGlyphIds) {
							filteringIndexMap[val] = next_id;
							const glm::vec4 color = scene.getGlyph3D(val)->getColor();
							glm::vec4 c = color;
							c.a = 0;
							scene.getGlyph3D(val)->setColor(c);
						}
						QString tag = "Z: " + QString::number(stackedGlyphs[uid]->tagValue);
						create_stacked_glyph(uid, stackedGlyphs[uid]->gpd, tag.toStdString(), scene);
					}
					stackedGlyphs[uid]->currentGlyphIds = stackedGlyphs[uid]->currentGlyphIds;
					stackedGlyphs[uid]->currentTagValue = stackedGlyphs[uid]->tagValue;
			}
			AwsLogger::getInstance()->localLogger("Calculated stacks");
			/*out << "Filtering map:" << endl;
			for (auto id : filteringIndexMap.keys()) {
				out << id << ", " << filteringIndexMap[id] << endl;
			}*/
			

			// Done adding; finish scene setup and clean up.
			scene.finishAdding();
			AwsLogger::getInstance()->localLogger("Passed finishAdding");
			timer.print_ms_to_debug( "read binary scene with %i objects (%i roots), %i links", glyph_count, root_count, link_count );
		}
		else
		{
			throw std::runtime_error( "Invalid or corrupt cached scene; try clearing your cache." );
		}
	}

	float SceneReader::findClosest(int n, float target)
	{
		// Corner cases
		if (target <= allScaleZ[0])
			return allScaleZ[0];
		if (target >= allScaleZ[n - 1])
			return allScaleZ[n - 1];

		// Doing binary search
		int i = 0, j = n, mid = 0;
		while (i < j) {
			mid = (i + j) / 2;

			if (allScaleZ[mid] == target)
				return allScaleZ[mid];

			/* If target is less than array element,
				then search in left */
			if (target < allScaleZ[mid]) {

				// If target is greater than previous
				// to mid, return closest of two
				if (mid > 0 && target > allScaleZ[mid - 1])
					return getClosest(allScaleZ[mid - 1],
						allScaleZ[mid], target);

				/* Repeat for left half */
				j = mid;
			}

			// If target is greater than mid
			else {
				if (mid < n - 1 && target < allScaleZ[mid + 1])
					return getClosest(allScaleZ[mid],
						allScaleZ[mid + 1], target);
				// update i
				i = mid + 1;
			}
		}

		// Only single element left after search
		return allScaleZ[mid];
	}

	float SceneReader::getClosest(float val1, float val2, float target)
	{
		if (target - val1 >= val2 - target)
			return val2;
		else
			return val1;
	}
}
