#include "antzgrid.h"

namespace SynGlyphXANTz {

	ANTzGrid::ANTzGrid() :
		ANTzObject(),
		m_textureID(0)
	{
		m_color.Set(255, 0, 0);
	}

	ANTzGrid::ANTzGrid(const ANTzGrid& grid) :
		ANTzObject(grid),
		m_textureID(grid.m_textureID) {
	
	}

	ANTzGrid::~ANTzGrid()
	{
	}

	ANTzGrid& ANTzGrid::operator=(const ANTzGrid& grid) {

		ANTzObject::operator=(grid);
		m_textureID = grid.m_textureID;

		return *this;
	}

	unsigned int ANTzGrid::GetTextureID() const {

		return m_textureID;
	}

	void ANTzGrid::SetTextureID(unsigned int textureID) {

		m_textureID = textureID;
	}

} //namespace SynGlyphXANTz
