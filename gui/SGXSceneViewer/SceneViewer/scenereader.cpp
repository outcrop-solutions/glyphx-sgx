
#include "scenereader.h"
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>
#include <endian.h>
#include <hal/hal.h>
#include <render/grid_renderer.h>
#include "baseimagerenderer.h"
#include "legacyglyphplacement.h"

namespace SynGlyphX
{
	namespace
	{
		const uint32_t MAGIC_NUMBER = 0xa042bc3f;

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
		return glm::vec4(
			static_cast<float>( read_int() / 255.f ),
			static_cast<float>( read_int() / 255.f ),
			static_cast<float>( read_int() / 255.f ),
			static_cast<float>( read_int() / 255.f ) );
	}

	glm::vec4 SceneReader::read_solid_color()
	{
		// colors are stored as integers, read and normalize to float
		return glm::vec4(
			static_cast<float>( read_int() / 255.f ),
			static_cast<float>( read_int() / 255.f ),
			static_cast<float>( read_int() / 255.f ),
			1.f );
	}

	glm::vec3 SceneReader::read_vec3()
	{
		// colors are stored as integers, read and normalize to float
		return glm::vec3(
			static_cast<float>( read_float() ),
			static_cast<float>( read_float() ),
			static_cast<float>( read_float() ) );
	}

	void SceneReader::read( const char* filename, BaseImageRenderer& base_images, const std::vector<hal::texture*>& base_image_textures, hal::texture* default_base_texture, render::grid_renderer& grids )
	{
		file = fopen( filename, "rb" );
		if ( file )
		{
			// First 4 bytes : magic number
			auto magic = read32_endian<int>( file );
			assert( magic == MAGIC_NUMBER );

			// First, get counts.
			auto base_image_count = read_int();
			auto node_count = read_int();
			hal::debug::print( "Reading %i glyph elements and %i base images.", node_count, base_image_count );

			// Next, read base images.
			for ( auto i = 0; i < base_image_count; ++i )
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

			// Then, read glyph elements.
			for ( auto i = 0; i < node_count; ++i )
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


			}

			fclose( file );
		}
	}
}
