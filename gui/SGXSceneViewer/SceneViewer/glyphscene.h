
#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <render/octree.h>
#include <string>
#include <unordered_set>
#include "glyphnode.h"
#include "glyphgeometrydb.h"
#include "loadedglyphdatainterface.h"

namespace SynGlyphX
{
	typedef unsigned int Glyph3DHandle;
	const Glyph3DHandle INVALID_GLYPH_HANDLE = 0u;
	class GlyphGeometryDB;

	class SGXSCENEVIEWER_API GlyphScene : public LoadedGlyphDataInterface
	{
	public:
		GlyphScene( GlyphGeometryDB& _db ) : octree( nullptr ), explode_state( group_state::retracted ), group_status( 0.f ), active_group( 0 ), filter_applied( false ),
            scene_changed( true ), selection_changed( false ), filter_mode( FilteredResultsDisplayMode::TranslucentUnfiltered ), has_animation( false ), db( _db ),
            glyph_storage( nullptr ), glyph_storage_next( 0u ) { }
		virtual ~GlyphScene();
		GlyphScene( const GlyphScene& ) = delete;
		void operator=( const GlyphScene& ) = delete;

		void update( float timeDelta );

		Glyph3DNode* allocGlyph( unsigned int _id, bool _isRoot, Glyph3DNodeType _type, int _filtering_index = -1, uint32_t label = uint32_t(-1), uint32_t glyph_index = uint32_t(-1) );
		const char* createString( const char* text );

		void beginAdding( unsigned int count );
		void addToGlyphAllocation( unsigned int count );
		void add( Glyph3DNode* glyph );
		void remove(int id) { glyphs.erase(id); }
		void finishAdding();
		void clear();

		bool empty() const { return glyphs.empty(); }

		const Glyph3DNode* pick( const glm::vec3& ray_origin, const glm::vec3& ray_dir, bool include_filtered_out = true, bool active_group_only = false ) const;
		std::pair<const Glyph3DNode*, float> pick_with_distance( const glm::vec3& ray_origin, const glm::vec3& ray_dir, bool include_filtered_out = true, bool active_group_only = false ) const;

		void enumGlyphs( std::function<bool( const Glyph3DNode& )> fn ) const;

		void updateCachedTransforms() const;

		void setSelected( const Glyph3DNode* glyph );
		void setUnSelected( const Glyph3DNode* glyph );
		void clearSelection() { selection.clear(); selection_changed = true; }
		void clearFilteredOutFromSelection();
		bool isSelected( const Glyph3DNode* glyph ) const;
		bool selectionEmpty() const { return selection.size() == 0; }
		unsigned int selectionSize() const { return static_cast<unsigned int>(selection.size()); }
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
		void setFilterApplied() { filter_applied = true; }
		void setPassedFilter( unsigned int index );
		void clearFilter() { filtered.clear(); scene_changed = true; filter_applied = false; }
		bool passedFilter( const Glyph3DNode* node ) const;
		bool filterApplied() const { return filter_applied; }	// true if any filters have been applied since clearFilter() was called

		Glyph3DNode* getGlyph3D( Glyph3DHandle handle );

		bool getChanged() { return scene_changed; }
		void clearChangedFlag() { scene_changed = false; }
		void flagChanged() { scene_changed = true; }
		void updateBound() { bound_update_needed = true; }

		bool getSelectionChanged() { return selection_changed; }
		void clearSelectionChangedFlag() { selection_changed = false; }
		void flagSelectionChanged() { selection_changed = true; }

		render::box_bound& getBound() const;

		bool hasAnimation() const { return has_animation; }

		unsigned int getGroupSize( unsigned int group ) { return static_cast<unsigned int>(groups[group - 1].nodes.size()); }
		float getGroupStatus() const { return group_status; }
		unsigned int getActiveGroup() const { return active_group; }
		glm::vec3 getExplodedPosition( const Glyph3DNode* node ) const;
		glm::vec3 getExplodedPositionOffset( const Glyph3DNode* node ) const;
		bool isExploded( const Glyph3DNode* node ) const { return node->getRootParent()->getExplodedPositionGroup() == getActiveGroup() && group_status > 0.f; }
		bool isExploded( unsigned int group ) const { return group != 0 && group == active_group && group_status > 0.f; }
		void enumGroups( std::function<void( const std::vector< const Glyph3DNode*>&, unsigned int )> );
		bool anyGlyphInGroupVisible( unsigned int group ) const;

		void debugPrint( const Glyph3DNode* node );

		void toggleExplode( unsigned int group );
		void explode( unsigned int group );
		void collapse( unsigned int group );

		const GlyphGeometryDB& getGeomDB() { return db; }

		void setUseSuperimposed(bool useSuperimposed){ m_useSuperimposed = useSuperimposed; }

		void enumGlyphStrings( std::function<void( int id, int parent_id, int filtering_idx, const char* tag, const char* url, const char* desc )> fn ) const override;

		static const unsigned int NO_GROUP = 0u;

		std::vector<int> getSelectionIds();

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

		std::unordered_set<std::string> string_pool;

		bool m_useSuperimposed;
	};
}
