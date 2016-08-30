
#pragma once

#include "../platform.h"
#include <render/types.h>
#include <render/bounds.h>
#include <functional>
#include "types.h"

namespace SynGlyphX
{
	class Glyph3DNode;
	class PlacementPolicy;

	class SGXSCENEVIEWER_API Glyph3DNode
	{
	public:
		Glyph3DNode( const Glyph3DNode& ) = delete;
		~Glyph3DNode();

		int getFilteringIndex() { return filtering_index; }

		void setPlacementPolicy( PlacementPolicy* policy ) { placement = policy; }
		const PlacementPolicy* const getPlacementPolicy() const { return placement; }
		void updateCachedTransforms() const;

		void setGeometry( const GlyphShape& geom, bool _wireframe = false ) { geometry = geom; setWireframe( _wireframe ); }
		GlyphShape getGeometry() const { return geometry; }
		render::model* getModel( float dist = 0.f ) const;

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
		void setAnimationRate( float val ) { animation_rate = val; }

		// additional transform that's applied during rendering and picking, but that isn't inherited by
		// this node's children.
		const glm::mat4& getVisualTransform() const { return visual_transform; }
		void setVisualTransform( const glm::mat4& vtransform ) const { visual_transform = vtransform; }

		const glm::mat4& getCachedTransform() const { return cached_transform; }
		glm::vec3 getCachedPosition() const { return glm::vec3( cached_transform[3] ); }
		void setCachedTransform( const glm::mat4& trans ) const { cached_transform = trans; }
		render::sphere_bound getCachedBound() const { return bound; }
		render::sphere_bound getCachedCombinedBound() const { return combined_bound; }

		const std::vector<Glyph3DNode*>& getChildren() const { return children; }
		void setChild( Glyph3DNode* child ) { children.push_back( child ); child->parent = this; }
		const Glyph3DNode* getParent() const { return parent; };
		const Glyph3DNode* getRootParent() const;

		const glm::vec4& getColor() const { return color; }
		void setColor( const glm::vec4& _color ) { color = _color; }

		bool getWireframe() const { return wireframe; }
		void setWireframe( bool _wireframe ) { wireframe = _wireframe; }

		const char* getTag() const { return tag; }
		void setTag( const char* _tag );

		int getID() const { return id; }
		bool isRoot() const { return root; }

		void enumNodes( std::function<bool( const Glyph3DNode& )> fn ) const;

		static void clearTagPool();	// call when loading a new scene

	private:
		Glyph3DNode( unsigned int _id, bool _isRoot, int _filtering_index = -1 ) : placement( nullptr ), id( _id ), root( _isRoot ), tag( nullptr ), animation_axis( 1.f, 0.f, 0.f ), animation_rate( 0.f ),
			torus_ratio( 0.1f ), filtering_index( _filtering_index ), parent( nullptr ) { }

		PlacementPolicy* placement;
		GlyphShape geometry;

		glm::vec3 local_position;
		glm::vec3 local_rotation;
		glm::vec3 local_scale;
		float torus_ratio;

		glm::vec4 color;
		glm::vec3 animation_axis;
		float animation_rate;
		bool wireframe, root;
		std::vector<Glyph3DNode*> children;
		Glyph3DNode* parent;
		const char* tag;
		unsigned int id;
		int filtering_index;

		friend class GlyphScene;	// so glyphscene can instantiate

		// Cached and temporary settings that don't affect logical const-ness of object.
		mutable glm::mat4 cached_transform;
		mutable render::sphere_bound bound, combined_bound;
		mutable glm::mat4 visual_transform;
	};
}
