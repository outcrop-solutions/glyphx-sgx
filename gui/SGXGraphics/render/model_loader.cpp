
#include "model_loader.h"
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <hal/device.h>
#include <hal/vertex_format.h>
#include "model.h"

using namespace Assimp;

namespace SynGlyphX
{
	namespace render
	{
		typedef std::pair<hal::mesh*, sphere_bound> mesh_with_bound;

		namespace
		{
			void import_node( const aiScene* scene, aiNode* node, glm::mat4 accumulated_transform, model* _model, std::vector<mesh_with_bound>& mesh_list )
			{
				glm::mat4 node_transform = glm::make_mat4( reinterpret_cast<float*>( &node->mTransformation ) );
				accumulated_transform = accumulated_transform * glm::transpose( node_transform );

				for ( unsigned int i = 0; i < node->mNumMeshes; ++i )
				{
					if ( mesh_list[node->mMeshes[i]].first )
					{
						model_part* part = new model_part( mesh_list[node->mMeshes[i]].first, accumulated_transform, mesh_list[node->mMeshes[i]].second, node->mName.C_Str() );
						_model->add_part( part );
					}
				}

				for ( unsigned int i = 0; i < node->mNumChildren; ++i )
				{
					import_node( scene, node->mChildren[i], accumulated_transform, _model, mesh_list );
				}
			}

			hal::mesh* import_mesh( aiMesh* mesh, sphere_bound& bound, const model_loader_settings& settings )
			{
				assert( mesh->HasPositions() );

				unsigned int pos_offset = 0u, uv_offset = 0u, color_offset = 0u, normal_offset = 0u, bitangent_offset = 0u, tangent_offset = 0u;

				hal::vertex_format fmt;
				fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3u, hal::stream_semantic::position, 0 ) );
				pos_offset = 0u;

				if ( mesh->HasTextureCoords( 0 ) )
				{
					fmt.add_stream( hal::stream_info( hal::stream_type::float32, 2u, hal::stream_semantic::texcoord, 0 ) );
					uv_offset = fmt.element_offset_in_bytes( hal::stream_semantic::texcoord, 0 );
				}

				if ( mesh->HasVertexColors( 0 ) )
				{
					fmt.add_stream( hal::stream_info( hal::stream_type::float32, 4u, hal::stream_semantic::color, 0 ) );
					color_offset = fmt.element_offset_in_bytes( hal::stream_semantic::color, 0 );
				}

				if ( mesh->HasNormals() )
				{
					fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3u, hal::stream_semantic::normal, 0 ) );
					normal_offset = fmt.element_offset_in_bytes( hal::stream_semantic::normal, 0 );
				}

				if ( mesh->HasTangentsAndBitangents() )
				{
					fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3u, hal::stream_semantic::bitangent, 0 ) );
					bitangent_offset = fmt.element_offset_in_bytes( hal::stream_semantic::bitangent, 0 );
					fmt.add_stream( hal::stream_info( hal::stream_type::float32, 3u, hal::stream_semantic::tangent, 0 ) );
					tangent_offset = fmt.element_offset_in_bytes( hal::stream_semantic::tangent, 0 );
				}

				unsigned int vertex_count = mesh->mNumVertices;
				unsigned int vertex_size = fmt.vertex_size_in_bytes();

				std::vector<unsigned char> meshdata;
				meshdata.resize( vertex_count * vertex_size );

				aiVector3D* verts = mesh->mVertices;
				aiVector3D* uv = mesh->mTextureCoords[0];
				aiColor4D* colors = mesh->mColors[0];
				aiVector3D* normals = mesh->mNormals;
				aiVector3D* bitangents = mesh->mBitangents;
				aiVector3D* tangents = mesh->mTangents;

				unsigned int float_size = sizeof( float );

				for ( unsigned int i = 0; i < mesh->mNumVertices; ++i )
				{
					unsigned char* vert_base = &meshdata[i * vertex_size];
					memcpy_s( vert_base + pos_offset, vertex_size, &verts[i].x, float_size * 3 );
					if ( uv )
						memcpy_s( vert_base + uv_offset, vertex_size, &uv[i].x, float_size * 2 );
					if ( colors )
						memcpy_s( vert_base + color_offset, vertex_size, &colors[i].r, float_size * 4 );
					if ( normals )
						memcpy_s( vert_base + normal_offset, vertex_size, &normals[i].x, float_size * 3 );
					if ( bitangents )
						memcpy_s( vert_base + bitangent_offset, vertex_size, &bitangents[i].x, float_size * 3 );
					if ( tangents )
						memcpy_s( vert_base + tangent_offset, vertex_size, &tangents[i].x, float_size * 3 );
				}

				bound = sphere_bound::from_points( reinterpret_cast<glm::vec3*>( &meshdata[0] ), vertex_size, vertex_count );

				std::vector<unsigned int> indexdata;
				indexdata.resize( mesh->mNumFaces * 3u );

				for ( unsigned int i = 0; i < mesh->mNumFaces; ++i )
				{
					aiFace* face = &mesh->mFaces[i];
					assert( face->mNumIndices == 3 );	// should have triangulated mesh already via aiProcess_Triangulate
					indexdata[i * 3 + 0] = face->mIndices[0];
					indexdata[i * 3 + 1] = face->mIndices[1];
					indexdata[i * 3 + 2] = face->mIndices[2];
				}

				hal::mesh* result = hal::device::create_mesh( fmt, hal::primitive_type::triangle_list, vertex_count, &meshdata[0], mesh->mNumFaces, &indexdata[0], settings.support_readback );

				return result;
			}
		}

		model* load_model( const char* file, const model_loader_settings& settings )
		{
			Importer imp;
			unsigned int flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_TransformUVCoords | aiProcess_Triangulate;
			const aiScene* scene = imp.ReadFile( file, flags );

			model* result = nullptr;

			if ( scene )
			{
				std::vector<mesh_with_bound> meshes;

				for ( unsigned int i = 0; i < scene->mNumMeshes; ++i )
				{
					sphere_bound bound;
					hal::mesh* mesh = import_mesh( scene->mMeshes[i], bound, settings );
					meshes.push_back( std::make_pair( mesh, bound ) );
				}

				result = new model;
				import_node( scene, scene->mRootNode, glm::mat4(), result, meshes );

				// Mesh import code will have added its own refs to the mesh list, so release them.
				for ( auto m : meshes )
					hal::device::release( m.first );

				result->set_name( file );
			}

			return result;
		}

		model_loader_settings model_loader_defaults()
		{
			model_loader_settings s;
			s.support_readback = false;
			return s;
		}
	}
}
