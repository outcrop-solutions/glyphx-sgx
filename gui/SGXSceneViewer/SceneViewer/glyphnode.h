
#pragma once

#include "../platform.h"
#include <render/types.h>
#include <render/bounds.h>
#include <render/color.h>
#include <functional>
#include <vector>
#include "types.h"

namespace SynGlyphX
{
	class Glyph3DNode;
	class GlyphGeometryDB;
	class PlacementPolicy;

	enum class Glyph3DNodeType : unsigned int
	{
		GlyphElement,
		Link,
	};

	enum class GlyphStringType
	{
		Tag,
		Url,
		Desc,
		Count,
	};

	class SGXSCENEVIEWER_API Glyph3DNode
	{
	public:
		Glyph3DNode( const Glyph3DNode& ) = delete;
		~Glyph3DNode();

		Glyph3DNodeType getType() const { return type; }
		int getFilteringIndex() const { return filtering_index; }

		void setPlacementPolicy( PlacementPolicy* policy ) { placement = policy; }
		const PlacementPolicy* const getPlacementPolicy() const { return placement; }
		void updateCachedTransforms( const GlyphGeometryDB& db ) const;

		void setGeometry( const GlyphShape& geom, bool _wireframe = false ) { geometry = geom; setWireframe( _wireframe ); }
		GlyphShape getGeometry() const { return geometry; }

		void setLinkTargets( const Glyph3DNode* a, const Glyph3DNode* b ) {
			assert( type == Glyph3DNodeType::Link );
			link_a = a; link_b = b;
		}
		const Glyph3DNode* getLinkTarget1() const { return link_a; }
		const Glyph3DNode* getLinkTarget2() const { return link_b; }

		const glm::vec3& getLocalPosition() const { return local_position; }
		void setLocalPosition( const glm::vec3& position ) { local_position = position; }

		const glm::vec3& getLocalRotation() const { return local_rotation; }
		void setLocalRotation( const glm::vec3& rotation ) { local_rotation = rotation; }

		const glm::vec3& getLocalScale() const { return local_scale; }
		void setLocalScale( const glm::vec3& scale ) { local_scale = scale; }

		float getTorusRatio() const { return torus_ratio; }
		void setTorusRatio( float ratio ) { torus_ratio = ratio; }

		const glm::vec3& getAnimationAxis() const { return animation_axis; }
		void setAnimationAxis( const glm::vec3& axis ) { animation_axis = axis; }

		float getAnimationRate() const { return animation_rate; }
		void setAnimationRate( float val ) const { animation_rate = val; }
		glm::vec3 getAnimationCenter() const;

		// additional transform that's applied during rendering and picking, but that isn't inherited by
		// this node's children.
		glm::mat4 getVisualTransform() const;
		void setVisualScale( const glm::vec3& vscale ) const { visual_scale = vscale; }

		void setExplodedPosition( float group, const glm::vec3& pos ) const { exploded_position = pos; exploded_position_group = group; }
		const glm::vec3& getExplodedPosition() const { return getRootParent()->exploded_position; }
		const unsigned int getExplodedPositionGroup() const { return getRootParent()->exploded_position_group; }

		const glm::mat4& getCachedTransform() const { return cached_transform; }
		glm::vec3 getCachedPosition() const { return glm::vec3( cached_transform[3] ); }
		void setCachedTransform( const glm::mat4& trans ) const { cached_transform = trans; }
		render::sphere_bound getCachedBound() const { return bound; }
		render::sphere_bound getCachedCombinedBound() const { return combined_bound; }

		const std::vector<Glyph3DNode*>& getChildren() const { return children; }
		void setChild( Glyph3DNode* child ) { children.push_back( child ); child->parent = this; }
		const Glyph3DNode* getParent() const { return parent; };
		const Glyph3DNode* getRootParent() const;

		glm::vec4 getColor() const { return render::unpack_color( color ); }
		void setColor( const glm::vec4& _color ) { color = render::pack_color( _color ); }
		void setColor( const render::packed_color& _color ) { color = _color; }

		bool getWireframe() const { return wireframe; }
		void setWireframe( bool _wireframe ) { wireframe = _wireframe; }

		// NOTE: this class doesn't keep its own copy of _tag; tags passed to this must be stored elsewhere
		// (use GlyphScene::createTag to store them into the scene's string pool)
		void setString( GlyphStringType type, const char* _str );
		const char* getString( GlyphStringType type ) const { return strings[int(type)]; }

		int getID() const { return id; }
		bool isRoot() const { return root; }

		int32_t getLabel() const { return label; }

		void setHiddenByLegend(bool v) const { hidden_by_legend = v; }
		bool getHiddenByLegend() const { return hidden_by_legend; }

		void enumNodes( std::function<bool( const Glyph3DNode& )> fn ) const;

	private:
		Glyph3DNode( unsigned int _id, bool _isRoot, Glyph3DNodeType _type, int _filtering_index, uint32_t _label ) : placement( nullptr ), torus_ratio( 0.1f ), parent( nullptr ), id( _id ),
			filtering_index( _filtering_index ), exploded_position_group( 0.f ), animation_axis( 1.f, 0.f, 0.f ), animation_rate( 0.f ), root( _isRoot ), type( _type ), animation_root( false ),
			animation_child( false ), grouped( false ), label( _label ), hidden_by_legend( false ) {
			for ( auto i = 0; i < int( GlyphStringType::Count ); ++i ) strings[i] = nullptr;
		}

		PlacementPolicy* placement;

		glm::vec3 local_position;
		glm::vec3 local_rotation;
		glm::vec3 local_scale;
		float torus_ratio;

		render::packed_color color;
		std::vector<Glyph3DNode*> children;
		const Glyph3DNode* link_a, *link_b;
		Glyph3DNode* parent;
		const char* strings[int(GlyphStringType::Count)];
		unsigned int id;
		int filtering_index;

		// Cached and temporary settings that don't affect logical const-ness of object.
		mutable glm::mat4 cached_transform;
		mutable render::sphere_bound bound, combined_bound;
		mutable glm::vec3 visual_scale;
		mutable glm::vec3 exploded_position;
		mutable unsigned int exploded_position_group;
		mutable glm::vec3 animation_axis;
		mutable float animation_rate;

		uint16_t label;

		// flags( packed )
		GlyphShape geometry : 4;
		bool wireframe : 1;
		bool root : 1;
		Glyph3DNodeType type : 1;
		mutable bool animation_root : 1;
		mutable bool animation_child : 1;
		mutable bool grouped : 1;
		mutable bool hidden_by_legend : 1;

		friend class GlyphScene;	// so glyphscene can instantiate
	};
}
