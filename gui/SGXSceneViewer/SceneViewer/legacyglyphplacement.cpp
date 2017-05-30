
#include "legacyglyphplacement.h"
#include "glyphnode.h"
#include "placementpolicy.h"
#include "../../../antzplus/ANTzTopology/ANTzTopology.h"

namespace SynGlyphX
{
	PlacementPolicy* ChoosePlacementPolicy( const GlyphPlacementData& glyphData )
	{
		SynGlyphXANTz::ANTzPlacementPolicy::TopologyType topo = static_cast<SynGlyphXANTz::ANTzPlacementPolicy::TopologyType>(glyphData.topo);
		if (topo == SynGlyphXANTz::ANTzPlacementPolicy::TopologyType::Cube) {

			return new SynGlyphXANTz::ANTzCubePlacementPolicy();
		}
		else if ((topo == SynGlyphXANTz::ANTzPlacementPolicy::TopologyType::Sphere) ||
			(topo == SynGlyphXANTz::ANTzPlacementPolicy::TopologyType::Point)) {

			return new SynGlyphXANTz::ANTzSpherePlacementPolicy(topo == SynGlyphXANTz::ANTzPlacementPolicy::TopologyType::Point);
		}
		else if (topo == SynGlyphXANTz::ANTzPlacementPolicy::TopologyType::Torus) {

			return new SynGlyphXANTz::ANTzTorusPlacementPolicy();
		}
		else if (topo == SynGlyphXANTz::ANTzPlacementPolicy::TopologyType::Cylinder) {

			return new SynGlyphXANTz::ANTzCylinderPlacementPolicy();
		}
		else if ((topo == SynGlyphXANTz::ANTzPlacementPolicy::TopologyType::Pin) ||
			(topo == SynGlyphXANTz::ANTzPlacementPolicy::TopologyType::Rod)) {

			return new SynGlyphXANTz::ANTzPinPlacementPolicy(topo == SynGlyphXANTz::ANTzPlacementPolicy::TopologyType::Rod);
		}
		else {

			return new TestPlacementPolicy();
		}
	}
}
