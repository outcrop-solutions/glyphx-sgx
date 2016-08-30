
#pragma once

namespace SynGlyphX
{
	class Glyph3DNode;
	class PlacementPolicy;

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
		ErrorModel,
	};

	enum class FilteredResultsDisplayMode : unsigned int
	{
		ShowUnfiltered,
		HideUnfiltered,
		TranslucentUnfiltered,
	};
}
