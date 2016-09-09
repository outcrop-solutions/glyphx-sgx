
#include "placementpolicy.h"
#include "glyphnode.h"

namespace SynGlyphX
{
	void TestPlacementPolicy::repositionChildren( const Glyph3DNode& glyph, const glm::mat4& parentTransform )
	{
		// Test placement policy that just puts the glyphs on a row along the provided direction vector,
		// and scales children by 1/2.
		int idx = 1;
		for ( auto& c : glyph.getChildren() )
		{
			float dist = 1.f * float( idx++ );
			glm::vec3 offset = dist * direction;
			glm::mat4 translate = glm::translate( glm::mat4(), offset );
			glm::mat4 scale = glm::scale( glm::mat4(), glm::vec3( 0.5f, 0.5f, 0.5f ) );
			c->setCachedTransform( parentTransform * translate * scale );
		}
	}
}
