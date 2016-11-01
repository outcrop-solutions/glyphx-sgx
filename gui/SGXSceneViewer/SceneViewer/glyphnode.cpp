
#include "glyphnode.h"
#include <glm/gtc/matrix_transform.hpp>
#include <render/model.h>
#include <render/model_loader.h>
#include <render/torus_generator.h>
#include <unordered_set>
#include <stack>
#include "placementpolicy.h"
#include "glyphgeometrydb.h"

namespace SynGlyphX
{
	Glyph3DNode::~Glyph3DNode()
	{
		delete placement;
	}

	void Glyph3DNode::updateCachedTransforms( const GlyphGeometryDB& db ) const
	{
		assert( placement || children.size() == 0 );	// if the node has children it must also have a placement policy

		if ( parent && ( parent->animation_root || parent->animation_child ) )
		{
			animation_child = true;
			animation_rate += parent->animation_rate;
			animation_axis = parent->animation_axis;
		}
		else if ( animation_rate != 0.f )
		{
			animation_root = true;
		}

		auto model = db.get( getGeometry() );
		bound = render::transform_bound( model->get_bound(), cached_transform * getVisualTransform() );
		if ( placement ) placement->repositionChildren( *this, cached_transform );
		combined_bound = bound;
		for ( auto& c : children )
		{
			c->updateCachedTransforms( db );
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

	void Glyph3DNode::setTag( const char* _tag )
	{
		tag = _tag;
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

	glm::vec3 Glyph3DNode::getAnimationCenter() const
	{
		if ( animation_root )
		{
			return getCachedPosition();
		}
		else if ( animation_child )
		{
			const Glyph3DNode* n = parent;
			while ( n && ( n->animation_root || n->animation_child ) )
			{
				if ( n->animation_root )
					return n->getCachedPosition();
				n = n->parent;
			}
		}
		return glm::vec3();
	}
}

