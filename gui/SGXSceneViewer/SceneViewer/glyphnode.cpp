
#include "glyphnode.h"
#include <render/model_loader.h>
#include <render/torus_generator.h>
#include "placementpolicy.h"
#include "glyphgeometrydb.h"

namespace SynGlyphX
{
	namespace
	{
		std::unordered_set<std::string> tag_pool;
	}

	Glyph3DNode::~Glyph3DNode()
	{
		delete placement;
	}

	render::model* Glyph3DNode::getModel( float dist ) const
	{
		return GlyphGeometryDB::get( geometry, getTorusRatio(), dist );
	}

	void Glyph3DNode::updateCachedTransforms() const
	{
		assert( placement || children.size() == 0 );	// if the node has children it must also have a placement policy

		auto model = getModel();
		bound = render::transform_bound( model->get_bound(), cached_transform );
		if ( placement ) placement->repositionChildren( *this, cached_transform );
		combined_bound = bound;
		for ( auto& c : children )
		{
			c->updateCachedTransforms();
			combined_bound = render::combine_bounds( combined_bound, c->combined_bound );
		}
	}

	void Glyph3DNode::enumNodes( std::function<bool( const Glyph3DNode& )> fn ) const
	{
		// Depth-first traversal.
		std::stack<const Glyph3DNode*> q;
		q.push( this );

		while ( q.size() > 0 )
		{
			auto node = q.top();
			q.pop();

			bool recurse = fn( *node );

			if ( recurse )
			{
				for ( const auto& c : node->getChildren() )
					q.push( c );
			}
		}
	}

	namespace
	{
		// Quick and dirty function to strip surrounding quotes and XML stuff from tag.
		// Just find the outermost >/< pair and return whatever's between them.
		std::string strip_tag( const char* tag )
		{
			int len = strlen( tag );
			int begin = -1, end = -1;
			for ( int l = 0; l < len; ++l )
			{
				if ( tag[l] == '>' )
				{
					begin = l + 1;
					break;
				}
			}
			for ( int r = len - 1; r > begin; --r )
			{
				if ( tag[r] == '<' )
				{
					end = r;
				}
			}

			std::string tagstr( tag );
			if ( begin == -1 || end == -1 )
				return tagstr;	// if we failed, just return the original tag
			else
				return tagstr.substr( begin, end - begin );
		}
	}

	void Glyph3DNode::setTag( const char* _tag )
	{
		auto tagstr = strip_tag( _tag );
		auto t = tag_pool.find( tagstr );
		if ( t == tag_pool.end() )
		{
			tag_pool.insert( tagstr );
			t = tag_pool.find( tagstr );
		}

		tag = t->c_str();
	}

	void Glyph3DNode::clearTagPool()
	{
		// Todo: ensure there are no Glyph3DNodes using the pool?
		tag_pool.clear();
	}

	const Glyph3DNode* Glyph3DNode::getRootParent() const
	{
		const Glyph3DNode* root = this;
		while ( root->parent != nullptr )
			root = root->parent;
		return root;
	}

	glm::mat4 Glyph3DNode::getVisualTransform() const
	{
		return glm::scale( glm::mat4(), visual_scale ); 
	}
}

