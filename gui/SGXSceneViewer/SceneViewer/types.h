
#pragma once

namespace SynGlyphX
{
	class Glyph3DNode;
	class PlacementPolicy;

	// Note: if you add shapes to this you may need to increase the number of bits
	// in Glyph3DNode::geometry
	enum class GlyphShape : unsigned int
	{
		Cube,
		Sphere,
		Cone,
		Torus,
		Dodecahedron,
		Octahedron,
		Tetrahedron,
		Icosahedron,
		Pin,
		Cylinder,
		Link_Cylinder,
		Link_Cube,
		ErrorModel,
	};

	enum class LinkProfile
	{
		Circle,
		Square,
	};

	enum class FilteredResultsDisplayMode : unsigned int
	{
		ShowUnfiltered,
		HideUnfiltered,
		TranslucentUnfiltered,
	};
}
