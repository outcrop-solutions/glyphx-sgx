#include "antzgrid.h"

namespace SynGlyphX {

	ANTzGrid::ANTzGrid() :
		GlyphNumericMappableProperties(),
		m_textureID(0)
	{
		m_color.Set(255, 0, 0, 150);
	}

	ANTzGrid::ANTzGrid(const ANTzGrid& grid) :
		GlyphNumericMappableProperties(grid),
		m_textureID(grid.m_textureID) {
	
	}

	ANTzGrid::~ANTzGrid()
	{
	}

	ANTzGrid& ANTzGrid::operator=(const ANTzGrid& grid) {

		GlyphNumericMappableProperties::operator=(grid);
		m_textureID = grid.m_textureID;

		return *this;
	}

	unsigned int ANTzGrid::GetTextureID() const {

		return m_textureID;
	}

	void ANTzGrid::SetTextureID(unsigned int textureID) {

		m_textureID = textureID;
	}

} //namespace SynGlyphX