
#pragma once

#include "../platform.h"
#include <glm/glm.hpp>

namespace SynGlyphX
{
	class Glyph3DNode;

	class SGXSCENEVIEWER_API PlacementPolicy
	{
	public:
        virtual ~PlacementPolicy() { }
		virtual void repositionChildren( const Glyph3DNode& glyph, const glm::mat4& parentTransform ) = 0;
	};

	class SGXSCENEVIEWER_API TestPlacementPolicy : public PlacementPolicy
	{
	public:
		TestPlacementPolicy( ) : direction( glm::vec3( 0.f, 1.f, 0.f ) ), step( 1.f ) { }
		virtual ~TestPlacementPolicy() { }
		void repositionChildren( const Glyph3DNode& glyph, const glm::mat4& parentTransform ) override;
		void setDirection( const glm::vec3& dir ) { direction = glm::normalize( dir ); }
		void setStep( float _step ) { step = _step; }
	private:
		glm::vec3 direction;
		float step;
	};

}

