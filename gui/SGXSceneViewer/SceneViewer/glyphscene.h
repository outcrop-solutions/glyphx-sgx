
#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <render/octree.h>
#include <unordered_set>
#include "glyphnode.h"
#include "glyphgeometrydb.h"

namespace SynGlyphX
{
	typedef unsigned int Glyph3DHandle;
	const Glyph3DHandle INVALID_GLYPH_HANDLE = 0u;
	class GlyphGeometryDB;

	class SGXSCENEVIEWER_API GlyphScene
	{
	public:
		GlyphScene( GlyphGeometryDB _db ) : octree( nullptr ), filter_applied( false ), selection_changed( false ), glyph_storage( nullptr ), glyph_storage_next( 0u ),
			filter_mode( FilteredResultsDisplayMode::TranslucentUnfiltered ), has_animation( false ), db( _db ), explode_state( group_state::retracted ),
			active_group( 0 ), group_status( 0.f ) { }
		~GlyphScene();
		GlyphScene( const GlyphScene& ) = delete;

		void update( float timeDelta );

		Glyph3DNode* allocGlyph( unsigned int _id, bool _isRoot, Glyph3DNodeType _type, int _filtering_index = -1 );

		void beginAdding( unsigned int count );
		void add( Glyph3DNode* glyph );
		void finishAdding();
		void clear();

		bool empty() const { return glyphs.empty(); }

		const Glyph3DNode* pick( const glm::vec3& ray_origin, const glm::vec3& ray_dir, bool include_filtered_out = true, bool active_group_only = false ) const;
		std::pair<const Glyph3DNode*, float> pick_with_distance( const glm::vec3& ray_origin, const glm::vec3& ray_dir, bool include_filtered_out = true, bool active_group_only = false ) const;

		void enumGlyphs( std::function<bool( const Glyph3DNode& )> fn, bool includeChildren ) const;

		void updateCachedTransforms() const;

		void setSelected( const Glyph3DNode* glyph );
		void setUnSelected( const Glyph3DNode* glyph );
		void clearSelection() { selection.clear(); selection_changed = true; }
		void clearFilteredOutFromSelection();
		bool isSelected( const Glyph3DNode* glyph ) const;
		bool selectionEmpty() const { return selection.size() == 0; }
		unsigned int selectionSize() const { return selection.size(); }
		const Glyph3DNode* getSingleSelection() const
		{
			if ( selection.size() == 0 ) return nullptr;
			else return *selection.begin();
		}
		const Glyph3DNode* getSingleRoot() const;
		void enumSelected( std::function<void( const Glyph3DNode& glyph )> fn ) const;

		void enableTag( const Glyph3DNode* glyph ) const;
		void disableTag( const Glyph3DNode* glyph ) const;
		void disableAllTags() const { tag_enabled.clear(); }
		void enumTagEnabled( std::function<void( const Glyph3DNode& glyph )> fn ) const;

		void setFilterMode( FilteredResultsDisplayMode mode ) { filter_mode = mode; scene_changed = true; }
		FilteredResultsDisplayMode getFilterMode() { return filter_mode; }
		void setPassedFilter( unsigned int index );
		void clearFilter() { filtered.clear(); scene_changed = true; filter_applied = false; }
		bool passedFilter( const Glyph3DNode* node ) const;
		bool filterApplied() const { return filter_applied; }	// true if any filters have been applied since clearFilter() was called

		Glyph3DNode* getGlyph3D( Glyph3DHandle handle );

		bool getChanged() { return scene_changed; }
		void clearChangedFlag() { scene_changed = false; }

		bool getSelectionChanged() { return selection_changed; }
		void clearSelectionChangedFlag() { selection_changed = false; }

		render::box_bound& getBound() const;

		bool hasAnimation() const { return has_animation; }

		unsigned int getGroupSize( unsigned int group ) { return groups[group - 1].nodes.size(); }
		float getGroupStatus() const { return group_status; }
		unsigned int getActiveGroup() const { return active_group; }
		glm::vec3 getExplodedPosition( const Glyph3DNode* node ) const;
		glm::vec3 getExplodedPositionOffset( const Glyph3DNode* node ) const;
		bool isExploded( const Glyph3DNode* node ) const { return node->getRootParent()->getAlternatePositionGroup() == getActiveGroup() && group_status > 0.f; }
		bool isExploded( unsigned int group ) const { return group != 0 && group == active_group && group_status > 0.f; }
		void enumGroups( std::function<void( const std::vector< const Glyph3DNode*>&, unsigned int )> );

		void debugPrint( const Glyph3DNode* node );

		void toggleExplode( unsigned int group );
		void explode( unsigned int group );
		void collapse( unsigned int group );

		static const unsigned int NO_GROUP = 0u;

	private:
		void compute_groups();
		void update_groups();
		glm::vec3 apply_explosion_offset( const Glyph3DNode* node, const glm::vec3& pos ) const;

		render::octree<Glyph3DNode>* octree;
		std::unordered_map<Glyph3DHandle, Glyph3DNode*> glyphs;
		std::unordered_map<Glyph3DHandle, Glyph3DNode*> glyphs_by_filtering_index;
		std::unordered_set<const Glyph3DNode*> selection;

		enum class group_state
		{
			exploding,
			exploded,
			retracting,
			retracted,
		};

		struct superimposed_group
		{
			std::vector< const Glyph3DNode* > nodes;
		};
		std::vector< superimposed_group > groups;
		group_state explode_state;
		float group_status;
		unsigned int active_group;
		const superimposed_group& get_group( unsigned int idx ) const { return groups[idx - 1]; }	// group ids are 1-based

		// Temporary and cached properties that don't affect this object's logical const-ness.
		mutable bool bound_update_needed;
		mutable render::box_bound cached_bound;
		mutable std::unordered_set<const Glyph3DNode*> tag_enabled;
		mutable std::unordered_set<const Glyph3DNode*> filtered;
		mutable bool filter_applied;

		bool scene_changed;
		bool selection_changed;
		FilteredResultsDisplayMode filter_mode;

		bool has_animation;

		GlyphGeometryDB& db;
		char* glyph_storage;
		unsigned int glyph_storage_next, glyph_storage_size;
	};
}
