
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
#include "legacyglyphplacement.h"
#include "glyphscene.h"

namespace SynGlyphX
{
	namespace
	{
		const uint32_t SCENE_FILE_MAGIC_NUMBER = 0xa042bc3f;
		const uint32_t COUNT_FILE_MAGIC_NUMBER = 0x294ee1ac;

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

	glm::vec4 SceneReader::read_color()
	{
		// colors are stored as integers, read and normalize to float
		glm::vec4 result;
		result.x = static_cast<float>( read_int() / 255.f );
		result.y = static_cast<float>( read_int() / 255.f );
		result.z = static_cast<float>( read_int() / 255.f );
		result.a = static_cast<float>( read_int() / 255.f );
		return result;
	}

	glm::vec4 SceneReader::read_solid_color()
	{
		// colors are stored as integers, read and normalize to float
		glm::vec4 result;
		result.x = static_cast<float>( read_int() / 255.f );
		result.y = static_cast<float>( read_int() / 255.f );
		result.z = static_cast<float>( read_int() / 255.f );
		result.a = 1.f;
		return result;
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
		auto tex_id = read_int();
		glm::vec3 pos, rot;
		pos.x = read_float(); pos.y = read_float(); pos.z = read_float();
		rot.x = glm::radians( read_float() ); rot.y = glm::radians( read_float() ); rot.z = glm::radians( read_float() );
		auto color = read_solid_color();
		float grid_cell_w = read_float(), grid_cell_h = read_float();
		int grid_cells_x = read_int(), grid_cells_y = read_int();

		if ( tex_id != 0 )
		{
			auto transform = glm::translate( glm::mat4(), pos ) * get_rotation_matrix( rot );
			glm::vec2 size = glm::vec2( grid_cell_w * grid_cells_x, grid_cell_h * grid_cells_y );

			hal::texture* tex = default_base_texture;
			if ( tex_id > 1u )	// 1 = use default texture
			{
				auto offset_tex_id = tex_id - 2u;	// offset to account for 0 = none, 1 = default
				if ( offset_tex_id < base_image_textures.size() ) tex = base_image_textures[offset_tex_id];
			}
			base_images.add( tex, transform, size );

			if ( grid_cells_x >= 2 || grid_cells_y >= 2 )
				grids.add( transform, size, grid_cells_x, grid_cells_y, color );
		}
	}

	void SceneReader::read_glyph_element( GlyphScene& scene )
	{
		GlyphCSVData data;

		data.id = read_int();
		data.parent_id = read_int();
		data.pos = read_vec3();
		data.rot = read_vec3();
		data.scale = read_vec3();
		data.color = read_color();
		data.geom_type = read_int();
		data.topo = read_int();
		data.ratio = read_float();
		data.rotation_rates = read_vec3();

		Glyph3DNode* parent = data.parent_id ? scene.getGlyph3D( data.parent_id ) : nullptr;
		data.is_root = ( data.parent_id == 0 ) || !parent;
		hal::debug::_assert( data.parent_id == 0 || parent, "glyph %i is parented to nonexistent glyph %i", data.id, data.parent_id );

		auto* glyphnode = scene.allocGlyph( data.id, data.is_root, Glyph3DNodeType::GlyphElement, data.is_root ? next_filtering_index++ : -1 );
		SetupGeometry( data.geom_type, *glyphnode );
		glyphnode->setColor( data.color );
		glyphnode->setPlacementPolicy( ChoosePlacementPolicy( data ) );

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
			assert( glyph_parent );	// this expects CSV to always have parents before children
			glyph_parent->setChild( glyphnode );
		}
		scene.add( glyphnode );

		if ( data.id >= next_id )
			next_id = data.id + 1;
	}

	void SceneReader::read_link( GlyphScene& scene )
	{
		int id0 = read_int();
		int id1 = read_int();
		auto* glyph0 = scene.getGlyph3D( id0 );
		auto* glyph1 = scene.getGlyph3D( id1 );

		auto geom_type = read_int();
		glm::vec4 color = read_color();
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

	void SceneReader::read( const char* scenefilename, const char* countfilename, GlyphScene& scene, BaseImageRenderer& base_images, const std::vector<hal::texture*>& base_image_textures, hal::texture* default_base_texture, render::grid_renderer& grids )
	{
		root_count = 0u;
		int next_filtering_index = 0;
		int next_id = 0;

		hal::debug::profile_timer timer;

		FILE* countfile = fopen( countfilename, "rb" );

		file = fopen( scenefilename, "rb" );
		if ( countfile && file )
		{
			// Read counts from count file.
			int count_magic = read32_endian<int>( countfile );
			assert( count_magic == COUNT_FILE_MAGIC_NUMBER );
			int base_image_count = read32_endian<int>( countfile );
			int glyph_count = read32_endian<int>( countfile );
			int link_count = read32_endian<int>( countfile );
			int tag_count = read32_endian<int>( countfile );

			auto magic = read32_endian<int>( file );
			assert( magic == SCENE_FILE_MAGIC_NUMBER );

			scene.beginAdding( glyph_count + link_count );

			for ( int i = 0; i < base_image_count; ++i )
				read_base_image( base_images, base_image_textures, default_base_texture, grids );
			for ( int i = 0; i < glyph_count; ++i )
				read_glyph_element( scene );
			for ( int i = 0; i < link_count; ++i )
				read_link( scene );

			// Done adding; finish scene setup and clean up.
			scene.finishAdding();
			timer.print_ms_to_debug( "read binary scene with %i objects (%i roots), %i links", glyph_count, root_count, link_count );
		}

		if ( file ) fclose( file );
		if ( countfile ) fclose( countfile );
	}
}
