
#include "legacyscenereader.h"
#include <hal/debug.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <render/grid_renderer.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <hal/debug.h>
#include "glyphscene.h"
#include "glyphnode.h"
#include "glyphgeometrydb.h"
#include "legacyglyphplacement.h"
#include "baseimagerenderer.h"

namespace SynGlyphX
{
	namespace
	{
		enum class EntryType : int
		{
			GLYPH = 5,
			BASE_IMAGE = 6,
			LINK = 7,
		};

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

		struct ColumnIndices
		{
			int id, parent_id, child_id, type;
			int x, y, z, rx, ry, rz, sx, sy, sz;
			int r, g, b, a;
			int rotate_rate_x, rotate_rate_y, rotate_rate_z;
			int geom, topo;
			int color_idx;
			int ratio;
			int grid_cell_width, grid_cell_height;
			int grid_segments_x, grid_segments_y;
			int texture_id;
		};

		int GetHeaderIndex( const std::vector<std::string>& headerList, const char* header )
		{
			auto it = std::find( headerList.begin(), headerList.end(), header );
			if ( it == headerList.end() )
			{
				assert( false );
				return -1;
			}
			else
			{
				return it - headerList.begin();
			}
		}

		unsigned int CountGlyphs( const char* csv )
		{
			std::ifstream in( csv );
			hal::debug::profile_timer timer;
			unsigned int count = 0u;
			while ( !in.eof() )
			{
				std::string line;
				std::getline( in, line );
				if ( line.length() > 0 )
				{
					++count;	// TEMP!!! must check type field before adding
				}
			}
			timer.print_ms_to_debug( "glyph count prepass" );

			return count;
		}

		void ReadLine( std::ifstream& in, std::vector<std::string>& tokens )
		{
			assert( tokens.size() == 0 );
			std::string line;
			std::getline( in, line );
			unsigned int idx = 0u;
			std::string token;
			while ( idx < line.size() )
			{
				auto c = line[idx];
				if ( c == ',' && token.size() > 0 )
				{
					tokens.push_back( token );
					token = "";
				}
				else
				{
					token += c;
				}
				++idx;
			}
		}

		float GetItemF( const std::vector<std::string>& dataline, const unsigned int index )
		{
			assert( index < dataline.size() );
			return atof( dataline[index].c_str() );
		}

		int GetItemI( const std::vector<std::string>& dataline, const unsigned int index )
		{
			assert( index < dataline.size() );
			return atoi( dataline[index].c_str() );
		}

		glm::vec2 GetItemVec2( const std::vector<std::string>& dataline, const unsigned int first_component_index )
		{
			assert( first_component_index + 1 < dataline.size() );
			return glm::vec2( atof( dataline[first_component_index].c_str() ), atof( dataline[first_component_index + 1].c_str() ) );
		}

		glm::vec3 GetItemVec3( const std::vector<std::string>& dataline, const unsigned int first_component_index )
		{
			assert( first_component_index + 2 < dataline.size() );
			return glm::vec3( atof( dataline[first_component_index].c_str() ), atof( dataline[first_component_index + 1].c_str() ), atof( dataline[first_component_index + 2].c_str() ) );
		}

		glm::vec4 GetItemVec4( const std::vector<std::string>& dataline, const unsigned int first_component_index )
		{
			assert( first_component_index + 3 < dataline.size() );
			return glm::vec4( atof( dataline[first_component_index].c_str() ), atof( dataline[first_component_index + 1].c_str() ), atof( dataline[first_component_index + 2].c_str() ), atof( dataline[first_component_index + 3].c_str() ) );
		}

		glm::vec4 GetItemColor( const std::vector<std::string>& dataline, const unsigned int first_component_index )
		{
			assert( first_component_index + 3 < dataline.size() );
			return glm::vec4( atof( dataline[first_component_index].c_str() ) / 255.f, atof( dataline[first_component_index + 1].c_str() ) / 255.f, atof( dataline[first_component_index + 2].c_str() ) / 255.f, atof( dataline[first_component_index + 3].c_str() ) / 255.f );
		}
	}

	namespace LegacySceneReader
	{
		const float BASE_SCALE = 1.f;	// temporary scale to make objects more visible until topo code available

		SGXSCENEVIEWER_API bool LoadLegacyScene( GlyphScene& scene, BaseImageRenderer& base_images, render::grid_renderer& grids, hal::texture* default_base_texture, const char* mainCSV, const char* tagCSV, const std::vector<hal::texture*>& base_image_textures )
		{
			hal::debug::profile_timer timer;
			unsigned int object_count = 0u;
			unsigned int root_count = 0u;
			int next_filtering_index = 0;
			
			auto count = CountGlyphs( mainCSV );

			std::ifstream main( mainCSV );
			if ( main )
			{
				scene.beginAdding( count );

				std::vector<std::string> headers;
				ReadLine( main, headers );

				ColumnIndices columns;
				columns.id = GetHeaderIndex( headers, "id" );
				columns.parent_id = GetHeaderIndex( headers, "parent_id" );
				columns.child_id = GetHeaderIndex( headers, "child_id" );
				columns.type = GetHeaderIndex( headers, "type" );
				columns.x = GetHeaderIndex( headers, "translate_x" );
				columns.y = GetHeaderIndex( headers, "translate_y" );
				columns.z = GetHeaderIndex( headers, "translate_z" );
				columns.rx = GetHeaderIndex( headers, "rotate_x" );
				columns.ry = GetHeaderIndex( headers, "rotate_y" );
				columns.rz = GetHeaderIndex( headers, "rotate_z" );
				columns.sx = GetHeaderIndex( headers, "scale_x" );
				columns.sy = GetHeaderIndex( headers, "scale_y" );
				columns.sz = GetHeaderIndex( headers, "scale_z" );
				columns.r = GetHeaderIndex( headers, "color_r" );
				columns.g = GetHeaderIndex( headers, "color_g" );
				columns.b = GetHeaderIndex( headers, "color_b" );
				columns.a = GetHeaderIndex( headers, "color_a" );
				columns.geom = GetHeaderIndex( headers, "geometry" );
				columns.topo = GetHeaderIndex( headers, "topo" );
				columns.color_idx = GetHeaderIndex( headers, "color_index" );
				columns.ratio = GetHeaderIndex( headers, "ratio" );
				columns.rotate_rate_x = GetHeaderIndex( headers, "rotate_rate_x" );
				columns.rotate_rate_y = GetHeaderIndex( headers, "rotate_rate_y" );
				columns.rotate_rate_z = GetHeaderIndex( headers, "rotate_rate_z" );
				columns.grid_cell_width = GetHeaderIndex( headers, "aux_a_x" );
				columns.grid_cell_height = GetHeaderIndex( headers, "aux_a_y" );
				columns.grid_segments_x = GetHeaderIndex( headers, "segments_x" );
				columns.grid_segments_y = GetHeaderIndex( headers, "segments_y" );
				columns.texture_id = GetHeaderIndex( headers, "texture_id" );

				while ( !main.eof() )
				{
					// TODO: optimize (not too bad in release, but fairly slow in debug for large CSVs)
					std::vector<std::string> dataline;
					ReadLine( main, dataline );
					if ( dataline.size() > 0 )
					{
						assert( dataline.size() == headers.size() );

						auto type = (EntryType)GetItemI( dataline, columns.type );
						// process here (don't store! waste of space)
						if ( type == EntryType::GLYPH )
						{
							GlyphCSVData data;
							data.id = GetItemI( dataline, columns.id );
							data.parent_id = GetItemI( dataline, columns.parent_id );
							Glyph3DNode* parent = data.parent_id ? scene.getGlyph3D( data.parent_id ) : nullptr;
							data.is_root = ( data.parent_id == 0 ) || !parent;
							data.color = GetItemColor( dataline, columns.r );
							data.pos = GetItemVec3( dataline, columns.x );
							data.rot = GetItemVec3( dataline, columns.rx );
							data.scale = GetItemVec3( dataline, columns.sx );
							data.color_index = GetItemI( dataline, columns.color_idx );
							data.geom_type = GetItemI( dataline, columns.geom );
							data.topo = GetItemI( dataline, columns.topo );
							data.ratio = GetItemF( dataline, columns.ratio );
							data.rotation_rates = GetItemVec3( dataline, columns.rotate_rate_x );

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

							++object_count;

							if ( data.is_root )
							{
								glm::mat4 transform = glm::translate( glm::mat4(), data.pos );
								glm::mat4 visual_transform;
								transform = glm::rotate( transform, glm::radians( data.rot.y ), glm::vec3( 0.0f, 0.0f, -1.0f ) );
								transform = glm::rotate( transform, glm::radians( data.rot.x ), glm::vec3( -1.0f, 0.0f, 0.0f ) );
								transform = glm::rotate( transform, glm::radians( data.rot.z ), glm::vec3( 0.0f, 0.0f, -1.0f ) );
								if ( data.topo == kNPtopoRod )
								{
									visual_transform = glm::scale( visual_transform, glm::vec3( data.ratio * 2.0f, data.ratio * 2.0f, data.scale.z * 5.f ) );
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
								glyphnode->setVisualTransform( visual_transform );
								++root_count;
							}
							else
							{
								auto glyph_parent = scene.getGlyph3D( data.parent_id );
								assert( glyph_parent );	// this expects CSV to always have parents before children
								glyph_parent->setChild( glyphnode );
							}
							scene.add( glyphnode );
						}
						else if ( type == EntryType::BASE_IMAGE )
						{
							float cell_w = GetItemF( dataline, columns.grid_cell_width );
							float cell_h = GetItemF( dataline, columns.grid_cell_height );
							float cells_x = GetItemF( dataline, columns.grid_segments_x );
							float cells_y = GetItemF( dataline, columns.grid_segments_y );
							glm::vec2 size = glm::vec2( cell_w * cells_x, cell_h * cells_y );
							glm::vec3 size3 = glm::vec3( size, 0.f );
							glm::mat4 scale = glm::scale( glm::mat4(), GetItemVec3( dataline, columns.sx ) );
							glm::mat4 translate = glm::translate( glm::mat4(), GetItemVec3( dataline, columns.x ) );
							glm::vec3 rotatev( glm::radians( GetItemF( dataline, columns.rx ) ), glm::radians( GetItemF( dataline, columns.ry ) ), glm::radians( GetItemF( dataline, columns.rz ) ) );
							auto rotate = glm::rotate( glm::mat4(), rotatev.y, glm::vec3( 0.f, 0.f, -1.f ) );
							rotate = glm::rotate( rotate, rotatev.x, glm::vec3( -1.f, 0.f, 0.f ) );
							rotate = glm::rotate( rotate, rotatev.z, glm::vec3( 0.f, 0.f, -1.f ) );
							glm::vec4 grid_color = GetItemColor( dataline, columns.r );

							unsigned int tex_id = unsigned int( GetItemI( dataline, columns.texture_id ) );
							if ( tex_id != 0 )
							{
								hal::texture* tex = default_base_texture;
								if ( tex_id > 1u )	// 1 = use default texture
								{
									auto offset_tex_id = tex_id - 2u;	// offset to account for 0 = none, 1 = default
									if ( offset_tex_id < base_image_textures.size() ) tex = base_image_textures[offset_tex_id];
								}
								base_images.add( tex, translate * rotate * scale, size );
							}

							if ( cells_x >= 2 || cells_y >= 2 )
								grids.add( translate * rotate * scale, size, cells_x, cells_y, grid_color );
						}
						else if ( type == EntryType::LINK )
						{
							int id0 = GetItemI( dataline, columns.parent_id );
							int id1 = GetItemI( dataline, columns.child_id );
							glm::vec4 color = GetItemColor( dataline, columns.r );
							auto* glyph0 = scene.getGlyph3D( id0 );
							auto* glyph1 = scene.getGlyph3D( id1 );
							auto geom_type = GetItemI( dataline, columns.geom );
							float thickness = GetItemF( dataline, columns.ratio );
							LinkProfile profile = ( geom_type == int( GeomType::CYLINDER ) ) ? LinkProfile::Circle : LinkProfile::Square;
							hal::debug::_assert( glyph0 && glyph1, "can't find glyph referenced by link" );

							if ( glyph0 && glyph1 )
							{
								// need linked glyphs to have up-to-date transforms so we know where to put the link
								glyph0->getRootParent()->updateCachedTransforms();
								glyph1->getRootParent()->updateCachedTransforms();
								auto pt0 = glyph0->getCachedPosition();
								auto pt1 = glyph1->getCachedPosition();
								glm::vec3 origin = ( pt0 + pt1 ) * 0.5f;
								auto translate = glm::translate( glm::mat4(), origin );
								auto rotate = glm::orientation( glm::normalize( pt1 - pt0 ), glm::vec3( 0.f, 0.f, 1.f ) );
								float length = glm::length( pt0 - pt1 );
								auto scale = glm::scale( glm::mat4(), glm::vec3( thickness, thickness, length ) );

								Glyph3DNode* linknode = scene.allocGlyph( GetItemI( dataline, columns.id ), true, Glyph3DNodeType::Link );
								linknode->setCachedTransform( translate * rotate * scale );
								linknode->setColor( color );
								linknode->setGeometry( profile == LinkProfile::Circle ? GlyphShape::Link_Cylinder : GlyphShape::Link_Cube );
								linknode->setLinkTargets( glyph0, glyph1 );
								scene.add( linknode );
							}
						}
					}
				}
			}
			else
			{
				return false;
			}

			// read tags
			std::ifstream tags( tagCSV );
			if ( tags )
			{
				std::vector<std::string> headers;
				ReadLine( tags, headers );

				int record_id_col = GetHeaderIndex( headers, "record_id" );
				int title_col = GetHeaderIndex( headers, "title" );

				while ( !tags.eof() )
				{
					std::vector<std::string> dataline;
					ReadLine( tags, dataline );
					if ( dataline.size() > 0 )
					{
						// Todo: handle commas in quotes (since apparently we allow this).
						if ( dataline.size() != headers.size() )
							hal::debug::print( "Quoted commas in CSV columns not yet supported." );
						// assert( dataline.size() == headers.size() );
						int record_id = atoi( dataline[record_id_col].c_str() );
						auto* glyph = scene.getGlyph3D( record_id );
						hal::debug::_assert( glyph, "could not find glyph referenced by tag" );
						if ( glyph ) glyph->setTag( dataline[title_col].c_str() );
					}
				}
			}
			else
			{
				return false;
			}

			hal::debug::print( "Read %i objects (%i of which are roots) in %i ms.", object_count, root_count, timer.elapsed_ms() );

			scene.finishAdding();

			return true;
		}
	}
}