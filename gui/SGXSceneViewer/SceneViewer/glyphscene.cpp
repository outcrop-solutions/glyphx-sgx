
#include "glyphscene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <hal/debug.h>
#include <render/model.h>
#include "glyphgeometrydb.h"
#include "glm/ext.hpp"
#include "AwsLogger.h"

namespace SynGlyphX
{
	GlyphScene::~GlyphScene()
	{
		clear();
	}

	Glyph3DNode* GlyphScene::allocGlyph( unsigned int _id, bool _isRoot, Glyph3DNodeType _type, int _filtering_index, uint32_t _label, uint32_t _glyph_index)
	{
		assert( glyph_storage_size != 0u );
		assert( glyph_storage_next < glyph_storage_size );
		char* mem = glyph_storage + sizeof(Glyph3DNode) * glyph_storage_next;
		++glyph_storage_next;
		return new (mem) Glyph3DNode(_id, _isRoot, _type, _filtering_index, _label, _glyph_index);
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

	void GlyphScene::addToGlyphAllocation(unsigned int count) 
	{
		size_t alloc_size = (glyph_storage_size + count) * sizeof(Glyph3DNode);
		char * newArray = new char[alloc_size];
		std::strcpy(newArray, glyph_storage);
		glyph_storage = newArray;
		glyph_storage_size += count;
	}

	void GlyphScene::finishAdding()
	{
		hal::debug::profile_timer timer0;
		updateCachedTransforms();
		timer0.print_ms_to_debug( "updated cached transforms" );
		AwsLogger::getInstance()->localLogger("updated cached transforms");

		hal::debug::profile_timer timer2;
		render::box_bound scene_bound;
		for ( auto& g : glyphs )
		{
			if ( g.second->isRoot() )
				scene_bound = render::combine_bounds( scene_bound, g.second->getCachedCombinedBound() );
		}
		timer2.print_ms_to_debug( "computed scene bound" );
		AwsLogger::getInstance()->localLogger("computed scene bound");

		if ( octree ) delete octree;
		octree = new render::octree<Glyph3DNode>( scene_bound, render::octree_settings{ 0.1f, 4u, 16u } );

		hal::debug::profile_timer timer1;
		for ( auto& g : glyphs )
			if ( g.second->isRoot() ) octree->insert( g.second, g.second->getCachedCombinedBound() );
		octree->gather_stats_DEBUG();
		timer1.print_ms_to_debug( "built octree" );
		AwsLogger::getInstance()->localLogger("built octree");

		hal::debug::profile_timer timer3;
		if (m_useSuperimposed) {
			compute_groups();
		}
		timer3.print_ms_to_debug( "computed groups" );
		AwsLogger::getInstance()->localLogger("computed groups");

		hal::debug::profile_timer timer4;
		update_groups();
		timer4.print_ms_to_debug( "updated groups" );
		AwsLogger::getInstance()->localLogger("updated groups");
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
		groups.clear();
		selection.clear();
		delete octree;
		explode_state = group_state::retracted;
		active_group = 0u;
		group_status = 0.f;
		octree = nullptr;
		string_pool.clear();
		tag_enabled.clear();
	}

	void GlyphScene::enumGlyphs( std::function<bool( const Glyph3DNode& )> fn ) const
	{
		for ( auto& entry : glyphs )
		{
			auto& glyph = entry.second;
			bool result = fn( *glyph );
			if ( !result ) break;
		}
	}

	const Glyph3DNode* GlyphScene::pick( const glm::vec3& ray_origin, const glm::vec3& ray_dir, bool include_filtered_out, bool active_group_only ) const
	{
		return pick_with_distance( ray_origin, ray_dir, include_filtered_out, active_group_only ).first;
	}

	std::pair<const Glyph3DNode*, float> GlyphScene::pick_with_distance( const glm::vec3& ray_origin, const glm::vec3& ray_dir, bool include_filtered_out, bool active_group_only ) const
	{
		// No scene yet, early out.
		if ( empty() )
			return{ nullptr, FLT_MAX };

		hal::debug::profile_timer timer;

		// Gather candidates by picking against the octree. (Don't do this if we're only doing the active exploded group.)
		std::unordered_set<const Glyph3DNode*> candidates;
		if ( !active_group_only )
		{
			octree->pick( ray_origin, ray_dir, [&]( const Glyph3DNode* node ) {
				if ( ( include_filtered_out || passedFilter( node ) ) )
					candidates.insert( node );
			} );
		}

		// Glyphs in an exploded group won't have their temporary positions reflected in the octree, so make sure they're all
		// in the candidate set.
		if ( active_group > 0.f && group_status > 0.f )
		{
			const auto& group = get_group( static_cast<unsigned int>( active_group ) );
			for ( auto n : group.nodes )
			{
				assert( n->isRoot() );
				candidates.insert( n );
			}
		}

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

				bool exploded = isExploded( node );

				// First, check sphere bounds.
				auto bound = node->getCachedBound();
				if ( exploded )
					bound.set_center( apply_explosion_offset( node, bound.get_center() ) );
				auto combined_bound = node->getCachedCombinedBound();
				float t;
				glm::vec3 q_;
				int intersect_local = bound.intersects_ray( ray_origin, ray_dir, t, q_ );
				int intersect_combined = combined_bound.intersects_ray( ray_origin, ray_dir, t, q_ );
				if ( node->getColor().a > 0.f && intersect_local > 0 )
				{
					// We intersected the sphere bound, so pick against the actual model.
					auto model = db.get( node->getGeometry() );
					glm::vec3 pt;
					glm::mat4 transform = node->getCachedTransform() * node->getVisualTransform();
					if ( exploded )
						transform = glm::translate( glm::mat4(), getExplodedPositionOffset( node ) ) * transform;
					if ( model->pick( ray_origin, ray_dir, transform, pt ) )
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
				// Exception: if the node is part of an exploded group, the combined bound is unreliable,
				// so we have to check the children regardless.
				if ( intersect_combined > 0 || exploded )
					for ( const auto& c : node->getChildren() )
						q.push( c );
			}

		}

		timer.print_ms_to_debug( "picked against octree" );

		return{ best_glyph, best_dist };
	}

	void GlyphScene::toggleExplode( unsigned int group )
	{
		active_group = group;
		if ( explode_state == group_state::retracted || explode_state == group_state::retracting )
			explode( group );
		else if ( explode_state == group_state::exploded || explode_state == group_state::exploding )
			collapse( group );
	}

	void GlyphScene::collapse( unsigned int group )
	{
		if ( group <= groups.size() && group > 0u )
		{
			if ( explode_state == group_state::exploded || explode_state == group_state::exploding )
				explode_state = group_state::retracting;

			auto& g = groups[group - 1];
			for ( auto& n : g.nodes )
			{
				n->enumNodes( [this]( const Glyph3DNode& node )
				{
					disableTag( &node );
					return true;
				} );
			}
		}
	}

	void GlyphScene::explode( unsigned int group )
	{
		active_group = group;
		if ( explode_state == group_state::retracted || explode_state == group_state::retracting )
			explode_state = group_state::exploding;
	}

	void GlyphScene::setSelected( const Glyph3DNode* glyph )
	{
		if ( selection.find( glyph ) == selection.end() )
		{
			selection.insert( glyph );
			selection_changed = true;
			/*
			hal::debug::print("lp: %s", glm::to_string(glyph->getLocalPosition()));
			hal::debug::print("lr: %s", glm::to_string(glyph->getLocalRotation()));
			hal::debug::print("ls: %s", glm::to_string(glyph->getLocalScale()));
			hal::debug::print("cp: %s", glm::to_string(glyph->getCachedPosition()));
			hal::debug::print("ct: %s", glm::to_string(glyph->getCachedTransform()));
			*/
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
		enumGlyphs( [this]( const Glyph3DNode& node ) {
			if ( node.isRoot() )
				//AwsLogger::getInstance()->localLogger(QString::number(node.getID()));
				node.updateCachedTransforms( db );
			return true;
		} );
	}

	render::box_bound& GlyphScene::getBound() const
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
		if ( glyph->getString( GlyphStringType::Tag ) )
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

	const Glyph3DNode* GlyphScene::getSingleRoot() const
	{
		if ( !getSingleSelection() ) return nullptr;

		const Glyph3DNode* single_parent = getSingleSelection()->getRootParent();
		for ( auto g : selection )
			if ( g->getRootParent() != single_parent )
				return nullptr;

		return single_parent;
	}

	namespace
	{
		bool close_enough_to_create_group( const Glyph3DNode* g, const Glyph3DNode* n )
		{
			// if the ratio of the distance between the two glyphs' centers to the larger of their bound radii is smaller than radius_ratio,
			// they pass the threshold to create a group. (this is pretty much just a test to check if they're really close together without
			// using a fixed threshold that wouldn't vary with the glyph's scales.)
			const float radius_ratio = 0.05f;
			float larger_radius = glm::max( g->getCachedCombinedBound().get_radius(), n->getCachedCombinedBound().get_radius() );
			float center_dist = glm::distance( g->getCachedPosition(), n->getCachedPosition() );
			return ( center_dist / larger_radius < radius_ratio );
		}
	}

	void GlyphScene::compute_groups()
	{
		assert( groups.size() == 0 );

		std::vector<const Glyph3DNode*> ungrouped;

		for ( auto& g : glyphs )
			if ( g.second->isRoot() && g.second->getType() == Glyph3DNodeType::GlyphElement )
				ungrouped.push_back( g.second );

		// First pass : quickly, roughly group nodes into groups.
		hal::debug::profile_timer timer0;
		for ( auto& glyph0 : ungrouped )
		{
			if ( !glyph0->grouped )
			{
				superimposed_group group;
				group.nodes.push_back( glyph0 );
				bool create_group = false;

				// check the glyph's bound against the rest of the scene...
				// we want to add glyphs to a group if their centers call within its bound, BUT the test for actually creating
				// a group to begin with (close_enough_to_create_group) is stricter to prevent us from creating groups around
				// glyphs that just have barely touching bounds.
				octree->overlap( glyph0->getCachedCombinedBound(), [&group, &create_group, glyph0]( Glyph3DNode* glyph1 ) {
					if ( !glyph1->grouped && glyph1->getType() == Glyph3DNodeType::GlyphElement )
					{
						auto dist = glm::distance( glyph0->getCachedPosition(), glyph1->getCachedPosition() );
						if ( glyph0 != glyph1 && dist < glyph0->getCachedCombinedBound().get_radius() )
						{
							if ( close_enough_to_create_group( glyph0, glyph1 ) )
								create_group = true;
							group.nodes.push_back( glyph1 );
						}
					}
				} );

				if ( create_group )
				{
					for ( auto g : group.nodes ) g->grouped = true;
					groups.push_back( group );
				}
			}
		}
		timer0.print_ms_to_debug( "grouping first pass" );

		// Second pass: for each grouped node, check if any of the other groups are a better fit for it (i.e., its center
		// is closer to the center of the group than the group it was initially assigned to).
		// (This is faster to do here than as part of the first pass, since now that we already know the groupings
		// we only have to check against every existing GROUP, not every existing NODE.)
		hal::debug::profile_timer timer1;
		for ( auto& group0 : groups )
		{
			auto it = group0.nodes.begin();
			++it;	// start at second element (first defines the bounds of the group)
			while ( it != group0.nodes.end() )
			{
				auto node = *it;
				superimposed_group* best_group = nullptr;
				float best_dist = glm::distance( node->getCachedPosition(), group0.nodes[0]->getCachedPosition() );

				// check against each other group to see if it's a better fit for the node
				for ( auto& group1 : groups )
				{
					if ( &group0 != &group1 )
					{
						float new_dist = glm::distance( node->getCachedPosition(), group1.nodes[0]->getCachedPosition() );
						if ( new_dist < best_dist )
						{
							best_group = &group1;
							best_dist = new_dist;
						}
					}
				}

				// if we found a better group move, the glyph to it
				if ( best_group != nullptr )
				{
					best_group->nodes.push_back( node );
					it = group0.nodes.erase( it );
				}
				else
				{
					++it;
				}
			}
		}
		timer1.print_ms_to_debug( "grouping second pass" );

		hal::debug::print( "computed %i superimposed groups", groups.size() );
	}

	void GlyphScene::update_groups()
	{
		float groupidx = 1.f;
		auto it = groups.begin();
		while ( it != groups.end() )
		{
			auto& group = *it;
			unsigned int rows = static_cast<unsigned int>( round( sqrtf( float( group.nodes.size() ) ) ) );
			const glm::vec3 explode_axis_0( 1.f, 0.f, 0.f );
			const glm::vec3 explode_axis_1( 0.f, 1.f, 0.f );
			float row = 0.f, col = 0.f;

			// Use the largest bound's diameter to decide how to space the group.
			float spacing = 1.f;
			const float spacing_buffer = 1.1f;
			for ( auto g : group.nodes )
				spacing = glm::max( spacing, g->getCachedCombinedBound().get_radius() * 2.f );
			spacing *= spacing_buffer;

			for ( auto g : group.nodes )
			{
				assert( g->isRoot() );
				glm::vec3 explode = explode_axis_0 * row * spacing + explode_axis_1 * col * spacing;
				explode.x -= rows * spacing * 0.5f;
				explode.y -= rows * spacing * 0.5f;
				g->setExplodedPosition( groupidx, explode );
				row += 1.f;
				if ( row >= rows )
				{
					col += 1.f;
					row = 0.f;
				}
			}

			groupidx += 1.f;
			++it;
		}
	}

	void GlyphScene::update( float timeDelta )
	{
		float direction = 0.f;
		if ( explode_state == group_state::exploding )
		{
			group_status = glm::clamp( group_status, 0.f, 1.f );
			if ( group_status >= 1.f )
				explode_state = group_state::exploded;
			direction = 1.f;
		}
		else if ( explode_state == group_state::retracting )
		{
			direction = -1.f;
			group_status = glm::clamp( group_status, 0.f, 1.f );
			if ( group_status <= 0.f )
			{
				explode_state = group_state::retracted;
				active_group = 0;
			}
		}
		group_status += direction * timeDelta * 0.005f;
		group_status = glm::clamp( group_status, 0.f, 1.f );
	}

	glm::vec3 GlyphScene::getExplodedPosition( const Glyph3DNode* node ) const
	{
		glm::vec3 pos = node->getCachedPosition();
		pos = apply_explosion_offset( node, pos );
		return pos;
	}

	glm::vec3 GlyphScene::getExplodedPositionOffset( const Glyph3DNode* node ) const
	{
		return glm::mix( glm::vec3(), node->getExplodedPosition(), group_status );
	}

	glm::vec3 GlyphScene::apply_explosion_offset( const Glyph3DNode* node, const glm::vec3& pos ) const
	{
		if ( node->getExplodedPositionGroup() == active_group )
			return pos + getExplodedPositionOffset( node );
		else
			return pos;
	}

	void GlyphScene::enumGroups( std::function<void( const std::vector< const Glyph3DNode* >&, unsigned int )> fn )
	{
		for ( auto i = 0u; i < groups.size(); ++i )
		{
			auto& g = groups[i];
			fn( g.nodes, i + 1 );
		}
	}

	bool GlyphScene::anyGlyphInGroupVisible( unsigned int group ) const
	{
		if ( filter_mode == FilteredResultsDisplayMode::HideUnfiltered )
		{
			// If any one glyph in the group is visible, we'll consider the group visible.
			auto& g = get_group( group );
			for ( auto n : g.nodes )
			{
				if ( passedFilter( n ) )
					return true;
			}
			return false;
		}
		else
		{
			return true;
		}
	}

	const char* GlyphScene::createString( const char* text )
	{
		auto t = string_pool.find( text );
		if ( t == string_pool.end() )
			t = string_pool.insert( text ).first;

		return t->c_str();
	}

	void GlyphScene::enumGlyphStrings( std::function<void( int id, int parent_id, int filtering_idx, const char* tag, const char* url, const char* desc )> fn ) const
	{
		for ( auto& g : glyphs )
		{
			auto glyph = g.second;
			if ( glyph->getType() == Glyph3DNodeType::GlyphElement )
			{
				int parent_id = glyph->getParent() ? glyph->getParent()->getID() : 0;
				fn( glyph->getID(), parent_id, glyph->getFilteringIndex(), glyph->getString( GlyphStringType::Tag ), glyph->getString( GlyphStringType::Url ), glyph->getString( GlyphStringType::Desc ) );
			}
		}
	}

	std::vector<int> GlyphScene::getSelectionIds()
	{
		std::vector<int> ids;

		for (auto entry : selection){
			ids.push_back(entry->getFilteringIndex());
		}
		
		return ids;
	}
}
