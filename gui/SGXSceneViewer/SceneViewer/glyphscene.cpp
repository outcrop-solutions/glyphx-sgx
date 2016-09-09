
#include "glyphscene.h"
#include <hal/debug.h>
#include <render/model.h>

namespace SynGlyphX
{
	GlyphScene::~GlyphScene()
	{
		clear();
	}

	Glyph3DNode* GlyphScene::allocGlyph( unsigned int _id, bool _isRoot, Glyph3DNodeType _type, int _filtering_index )
	{
		assert( glyph_storage_size != 0u );
		assert( glyph_storage_next < glyph_storage_size );
		char* mem = glyph_storage + sizeof( Glyph3DNode ) * glyph_storage_next;
		++glyph_storage_next;
		return new ( mem ) Glyph3DNode( _id, _isRoot, _type, _filtering_index );
	}

	void GlyphScene::add( Glyph3DNode* glyph )
	{
		if ( glyph->getAnimationRate() > 0.f ) has_animation = true;
		glyphs.insert( std::make_pair( glyph->getID(), glyph ) );
		glyphs_by_filtering_index.insert( std::make_pair( glyph->getFilteringIndex(), glyph ) );
		bound_update_needed = true;
		scene_changed = true;
	}

	void GlyphScene::beginAdding( unsigned int count )
	{
		size_t alloc_size = count * sizeof( Glyph3DNode );
		hal::debug::print( "Allocating %u bytes for glyph cache.", alloc_size );
		glyph_storage = new char[alloc_size];
		glyph_storage_size = count;
		glyph_storage_next = 0u;
	}

	void GlyphScene::finishAdding()
	{
		hal::debug::profile_timer timer0;
		updateCachedTransforms();
		timer0.print_ms_to_debug( "updated cached transforms" );

		hal::debug::profile_timer timer2;
		render::box_bound scene_bound;
		for ( auto& g : glyphs )
		{
			if ( g.second->isRoot() )
				scene_bound = render::combine_bounds( scene_bound, g.second->getCachedCombinedBound() );
		}
		timer2.print_ms_to_debug( "computed scene bound" );

		if ( octree ) delete octree;
		octree = new render::octree<Glyph3DNode>( scene_bound, render::octree_settings{ 0.1f, 4u, 16u } );

		hal::debug::profile_timer timer1;
		for ( auto& g : glyphs )
			if ( g.second->isRoot() ) octree->insert( g.second, g.second->getCachedCombinedBound() );
		octree->gather_stats_DEBUG();
		timer1.print_ms_to_debug( "built octree" );
	}

	void GlyphScene::clear()
	{
		has_animation = false;
		bound_update_needed = true;
		scene_changed = true;
		if ( glyph_storage )
		{
			for ( auto& g : glyphs )
				g.second->~Glyph3DNode();		// explicit destruct since we used placement new
			delete[] glyph_storage;
		}
		glyph_storage = nullptr;
		glyph_storage_next = 0u;
		glyphs.clear();
		glyphs_by_filtering_index.clear();
		selection.clear();
		Glyph3DNode::clearTagPool();
		delete octree;
		octree = nullptr;
	}

	void GlyphScene::enumGlyphs( std::function<bool( const Glyph3DNode& )> fn, bool includeChildren ) const
	{
		for ( auto& entry : glyphs )
		{
			auto& glyph = entry.second;
			bool result = fn( *glyph );
			if ( !result ) break;
		}
	}

	const Glyph3DNode* GlyphScene::pick( const glm::vec3& ray_origin, const glm::vec3& ray_dir, bool include_filtered_out ) const
	{
		// No scene yet, early out.
		if ( empty() )
			return nullptr;

		hal::debug::profile_timer timer;

		// Gather candidates by picking against the octree.
		std::unordered_set<const Glyph3DNode*> candidates;
		octree->pick( ray_origin, ray_dir, [&]( const Glyph3DNode* node ) {
			if ( include_filtered_out || passedFilter( node ) )
				candidates.insert( node );
		} );

		float best_dist = FLT_MAX;
		const Glyph3DNode* best_glyph = nullptr;

		// Pick through the candidates we've gathered to find the closest one.
		// TODO: A minor optimization here might be to cull candidates that are, at their closest point, still
		// further away than best_dist.
		for ( auto& glyph : candidates )
		{
			// Depth-first search.
			std::stack<const Glyph3DNode*> q;
			q.push( glyph );

			while ( q.size() > 0 )
			{
				auto node = q.top();
				q.pop();

				// First, check sphere bounds.
				auto bound = node->getCachedBound();
				auto combined_bound = node->getCachedCombinedBound();
				float t;
				glm::vec3 q_;
				int intersect_local = bound.intersects_ray( ray_origin, ray_dir, t, q_ );
				int intersect_combined = combined_bound.intersects_ray( ray_origin, ray_dir, t, q_ );
				if ( intersect_local > 0 )
				{
					// We intersected the sphere bound, so pick against the actual model.
					auto model = node->getModel(/* todo: correct LOD? */ );
					glm::vec3 pt;
					if ( model->pick( ray_origin, ray_dir, node->getCachedTransform() * node->getVisualTransform(), pt ) )
					{
						float dist = glm::length( ray_origin - pt );
						if ( dist < best_dist )
						{
							best_glyph = node;
							best_dist = dist;
						}
					}
				}

				// Only check this object's children if its combined bound was hit (since it contains them).
				if ( intersect_combined > 0 )
					for ( const auto& c : node->getChildren() )
						q.push( c );
			}

		}

		timer.print_ms_to_debug( "picked against octree" );

		return best_glyph;
	}

	void GlyphScene::setSelected( const Glyph3DNode* glyph )
	{
		if ( selection.find( glyph ) == selection.end() )
		{
			selection.insert( glyph );
			selection_changed = true;
		}
	}

	void GlyphScene::setUnSelected( const Glyph3DNode* glyph )
	{
		auto it = selection.find( glyph );
		if ( it != selection.end() )
		{
			selection.erase( it );
			selection_changed = true;
		}
	}

	bool GlyphScene::isSelected( const Glyph3DNode* glyph ) const
	{
		return selection.find( glyph ) != selection.end();
	}

	Glyph3DNode* GlyphScene::getGlyph3D( Glyph3DHandle handle )
	{
		auto result = glyphs.find( handle );
		if ( result != glyphs.end() )
			return result->second;
		else
			return nullptr;
	}

	void GlyphScene::enumSelected( std::function<void( const Glyph3DNode& glyph )> fn ) const
	{
		for ( auto& entry : selection )
			fn( *entry );
	}

	void GlyphScene::enumTagEnabled( std::function<void( const Glyph3DNode& glyph )> fn ) const
	{
		for ( auto& entry : tag_enabled )
			fn( *entry );
	}

	void GlyphScene::updateCachedTransforms() const
	{
		enumGlyphs( []( const Glyph3DNode& node ) {
			if ( node.isRoot() )
				node.updateCachedTransforms();
			return true;
		}, true );
	}

	render::box_bound& GlyphScene::get_bound() const
	{
		if ( bound_update_needed )
		{
			glm::vec3 min( FLT_MAX, FLT_MAX, FLT_MAX );
			glm::vec3 max( FLT_MIN, FLT_MIN, FLT_MIN );

			for ( const auto& g : glyphs )
			{
				if ( g.second->isRoot() )
				{
					auto pos = g.second->getCachedCombinedBound().get_center();
					if ( pos.x < min.x ) min.x = pos.x;
					if ( pos.y < min.y ) min.y = pos.y;
					if ( pos.z < min.z ) min.z = pos.z;
					if ( pos.x > max.x ) max.x = pos.x;
					if ( pos.y > max.y ) max.y = pos.y;
					if ( pos.z > max.z ) max.z = pos.z;
				}
			}

			cached_bound = render::box_bound( min, max );

			bound_update_needed = false;
		}
		return cached_bound;
	}

	void GlyphScene::enableTag( const Glyph3DNode* glyph ) const
	{
		auto it = tag_enabled.find( glyph );
		if ( it == tag_enabled.end() )
			tag_enabled.insert( glyph );
	}

	void GlyphScene::disableTag( const Glyph3DNode* glyph ) const
	{
		if ( glyph->getTag() )
		{
			auto it = tag_enabled.find( glyph );
			if ( it != tag_enabled.end() )
				tag_enabled.erase( it );
		}
	}

	void GlyphScene::setPassedFilter( unsigned int index )
	{
		auto it = glyphs_by_filtering_index.find( index );
		hal::debug::print( "filtering index %i", index );
		hal::debug::_assert( it != glyphs_by_filtering_index.end(), "GlyphScene::setFiltered: glyph index %i missing", index );
		if ( it != glyphs_by_filtering_index.end() )
		{
			if ( filtered.find( it->second ) == filtered.end() )
				filtered.insert( it->second );
		}
		scene_changed = true;
		filter_applied = true;
	}

	void GlyphScene::clearFilteredOutFromSelection()
	{
		// clear elements that fail the filter from the current selection
		for ( auto it = selection.begin(); it != selection.end(); /* intentionally blank */ )
		{
			if ( !passedFilter( *it ) )
				selection.erase( it++ );
			else
				++it;
		}
		selection_changed = true;
	}

	namespace
	{
		void debug_print( const GlyphScene& scene, const Glyph3DNode* node, unsigned int depth )
		{
			auto cbound_center = node->getCachedCombinedBound().get_center();
			auto cbound_radius = node->getCachedCombinedBound().get_radius();
			auto bound_center = node->getCachedBound().get_center();
			auto bound_radius = node->getCachedBound().get_radius();
			for ( unsigned int i = 0; i < depth; ++i ) hal::debug::print_noln( "  " );
			hal::debug::print( "%s%s%i : geom = %i bound = [%f %f %f] %f cbound = [%f %f %f] %f scalez = %f",
				node->isRoot() ? "ROOT " : "", scene.isSelected( node ) ? " [SELECTED] " : "", node->getID(), int( node->getGeometry() ),
				bound_center.x, bound_center.y, bound_center.z, bound_radius,
				cbound_center.x, cbound_center.y, cbound_center.z, cbound_radius,
				node->getLocalScale().z
			);

			for ( auto n : node->getChildren() )
				debug_print( scene, n, depth + 1 );
		}
	}

	void GlyphScene::debugPrint( const Glyph3DNode* node )
	{
		debug_print( *this, node->getRootParent(), 0u );
	}

	bool GlyphScene::passedFilter( const Glyph3DNode* node ) const
	{
		bool passes = true;
		if ( filter_applied )
		{
			if ( node->getType() == Glyph3DNodeType::GlyphElement )
				passes = filtered.find( node->getRootParent() ) != filtered.end();	// filtering is based on root node
			else if ( node->getType() == Glyph3DNodeType::Link )
			{
				// if mode is translucent, show link if either of its targets pass the filter, otherwise show only if
				// both of its targets pass the filter
				if ( filter_mode == FilteredResultsDisplayMode::TranslucentUnfiltered )
					passes = ( passedFilter( node->getLinkTarget1() ) || passedFilter( node->getLinkTarget2() ) );
				else
					passes = ( passedFilter( node->getLinkTarget1() ) && passedFilter( node->getLinkTarget2() ) );
			}
		}
		return passes;
	}
}
