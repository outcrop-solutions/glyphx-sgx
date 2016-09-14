
#pragma once

#include "../platform.h"
#include <functional>
#include <stack>
#include <glm/glm.hpp>

namespace SynGlyphX
{
	namespace render
	{
		class sphere_bound;
		class box_bound;
		template<typename content_t> class octree_node;

		struct octree_settings
		{
			float looseness;
			unsigned int max_depth;
			unsigned int max_content_per_node;
		};

		template<typename content_t>
		class octree
		{
		public:
			octree( const box_bound& bound, const octree_settings& _settings )
				: settings( _settings )
			{
				assert( settings.looseness <= 0.5f );
				// Make the octree a cube of the size of the bound's largest dimension.
				glm::vec3 size = bound.get_size();
				glm::vec3 center = bound.get_center();
				float dim = std::max( size.x, std::max( size.y, size.z ) );
				glm::vec3 halfsize( dim * 0.5f, dim * 0.5f, dim * 0.5f );
				render::box_bound new_bound( center - halfsize, center + halfsize );
				root = new octree_node<content_t>( new_bound, 0u, settings );
			}
			octree( const octree& ) = delete;
			~octree() { delete root; }

			void insert( content_t* content, const sphere_bound& bound ) { root->insert( content, bound ); }

			void pick( const glm::vec3& ray_origin, const glm::vec3& ray_dir, std::function<void( content_t* )> fn );

			void gather_stats_DEBUG();

		private:
			octree_settings settings;
			octree_node<content_t>* root;
		};

		template<typename content_t>
		class octree_node
		{
		public:
			octree_node( const box_bound& bound, unsigned int depth, const octree_settings& settings );
			octree_node( const octree_node& ) = delete;
			~octree_node();

			void insert( content_t* content, const sphere_bound& bound );
			unsigned int get_depth() const { return depth; }
			unsigned int get_object_count() const { return contents.size(); }

			const box_bound& get_bound() { return bound; }
			const box_bound& get_loose_bound() { return loose_bound; }

			octree_node** get_children() { return children; }

			struct content_container
			{
				sphere_bound bound;
				content_t* object;
			};

			const std::vector<content_container>& get_contents() { return contents; }

		private:
			box_bound bound, loose_bound;
			unsigned int depth;
			octree_node** children;

			std::vector<content_container> contents;

			bool try_adding_to_children( content_t* content, const sphere_bound& bound );

			const octree_settings& settings;

			void split();
		};

		template<typename content_t>
		octree_node<content_t>::octree_node( const box_bound& _bound, unsigned int _depth, const octree_settings& _settings ) :
			bound( _bound ), loose_bound( _bound ), depth( _depth ), settings( _settings ), children( nullptr )
		{
			loose_bound.dilate( loose_bound.get_size() * settings.looseness );
		}

		template<typename content_t>
		octree_node<content_t>::~octree_node()
		{
			if ( children )
			{
				for ( int i = 0; i < 8; ++i )
				{
					auto& child = children[i];
					delete child;
				}
				delete children;
			}
		}

		template<typename content_t>
		void octree_node<content_t>::insert( content_t* content, const sphere_bound& content_bound )
		{
			if ( !children )
			{
				if ( contents.size() >= settings.max_content_per_node && depth < settings.max_depth )
				{
					split();
					unsigned int idx = 0;
					while ( idx < contents.size() )
					{
						bool moved = try_adding_to_children( contents[idx].object, contents[idx].bound );
						if ( moved )
							contents.erase( contents.begin() + idx );
						else
							++idx;
					}
				}
			}

			bool added_to_child = try_adding_to_children( content, content_bound );

			if ( !added_to_child )
			{
				content_container cc{ content_bound, content };
				contents.push_back( cc );
			}
		}

		template<typename content_t>
		bool octree_node<content_t>::try_adding_to_children( content_t* content, const sphere_bound& content_bound )
		{
			if ( children )
			{
				for ( int i = 0; i < 8; ++i )
				{
					auto& child = *children[i];
					if ( child.loose_bound.contains( content_bound ) )
					{
						child.insert( content, content_bound );
						return true;
					}
				}
			}
			return false;
		}

		template<typename content_t>
		void octree_node<content_t>::split()
		{
			assert( !children );

			glm::vec3 min = bound.get_min();
			glm::vec3 max = bound.get_max();
			glm::vec3 ctr = min + ( max - min ) * 0.5f;

			children = new octree_node*[8]
			{
				new octree_node{ box_bound( glm::vec3( min.x, min.y, min.z ), glm::vec3( ctr.x, ctr.y, ctr.z ) ), depth + 1u, settings },
				new octree_node{ box_bound( glm::vec3( ctr.x, min.y, min.z ), glm::vec3( max.x, ctr.y, ctr.z ) ), depth + 1u, settings },
				new octree_node{ box_bound( glm::vec3( ctr.x, ctr.y, min.z ), glm::vec3( max.x, max.y, ctr.z ) ), depth + 1u, settings },
				new octree_node{ box_bound( glm::vec3( min.x, ctr.y, min.z ), glm::vec3( ctr.x, max.y, ctr.z ) ), depth + 1u, settings },

				new octree_node{ box_bound( glm::vec3( min.x, min.y, ctr.z ), glm::vec3( ctr.x, ctr.y, max.z ) ), depth + 1u, settings },
				new octree_node{ box_bound( glm::vec3( ctr.x, min.y, ctr.z ), glm::vec3( max.x, ctr.y, max.z ) ), depth + 1u, settings },
				new octree_node{ box_bound( glm::vec3( ctr.x, ctr.y, ctr.z ), glm::vec3( max.x, max.y, max.z ) ), depth + 1u, settings },
				new octree_node{ box_bound( glm::vec3( min.x, ctr.y, ctr.z ), glm::vec3( ctr.x, max.y, max.z ) ), depth + 1u, settings },
			};
		}

		template<typename content_t>
		void octree<content_t>::gather_stats_DEBUG()
		{
			std::vector<unsigned int> nodes_per_level;
			std::vector<unsigned int> objects_per_level;

			std::stack<octree_node<content_t>*> nodes;
			nodes.push( root );

			while ( !nodes.empty() )
			{
				octree_node<content_t>* node = nodes.top();
				nodes.pop();

				auto level = node->get_depth();
				if ( objects_per_level.size() <= level )
				{
					objects_per_level.resize( level + 1 );
					nodes_per_level.resize( level + 1 );
				}
				++nodes_per_level[level];
				objects_per_level[level] += node->get_object_count();

				auto children = node->get_children();
				if ( children )
				{
					for ( unsigned int i = 0; i < 8; ++i )
						nodes.push( children[i] );
				}
			}

			for ( unsigned int i = 0; i < objects_per_level.size(); ++i )
			{
				hal::debug::print( "Level %i: %i nodes, %i objects", i, nodes_per_level[i], objects_per_level[i] );
			}
		}

		template<typename content_t>
		void octree<content_t>::pick( const glm::vec3& ray_origin, const glm::vec3& ray_dir, std::function<void( content_t* )> fn )
		{
			std::stack<octree_node<content_t>*> nodes;
			nodes.push( root );

			while ( nodes.size() > 0 )
			{
				octree_node<content_t>* node = nodes.top();
				nodes.pop();

				if ( node->get_loose_bound().intersects_ray( ray_origin, ray_dir ) )
				{
					for ( auto& c : node->get_contents() )
					{
						float t;
						glm::vec3 q;
						if ( c.bound.intersects_ray( ray_origin, ray_dir, t, q ) != 0 )
						{
							fn( c.object );
						}
					}

					auto children = node->get_children();
					if ( children )
					{
						for ( unsigned int i = 0; i < 8; ++i )
							nodes.push( children[i] );
					}
				}
			}
		}
	}
}
