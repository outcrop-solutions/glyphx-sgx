#include "antzgrid.h"

namespace SynGlyphXANTz {

	ANTzGrid::ANTzGrid() :
		ANTzObject(),
		m_textureID(0),
		m_segments({ { 12, 6 } }),
		m_size({ { 360.0, 180.0 } })
	{
		m_color.Set(255, 0, 0);
	}

	ANTzGrid::ANTzGrid(const ANTzGrid& grid) :
		ANTzObject(grid),
		m_textureID(grid.m_textureID),
		m_segments(grid.m_segments),
		m_size(grid.m_size) {
	
	}

	ANTzGrid::~ANTzGrid()
	{
	}

	ANTzGrid& ANTzGrid::operator=(const ANTzGrid& grid) {

		ANTzObject::operator=(grid);
		m_textureID = grid.m_textureID;
		m_segments = grid.m_segments;
		m_size = grid.m_size;

		return *this;
	}

	unsigned int ANTzGrid::GetTextureID() const {

		return m_textureID;
	}

	void ANTzGrid::SetTextureID(unsigned int textureID) {

		m_textureID = textureID;
	}

	void ANTzGrid::SetSegments(const SynGlyphX::IntSize& segments) {

		if ((segments[0] == 0) || (segments[1] == 0)) {

			throw std::invalid_argument("Number of segments in ANTzGrid can not be zero");
		}

		m_segments = segments;
	}

	const SynGlyphX::IntSize& ANTzGrid::GetSegments() const {

		return m_segments;
	}

	void ANTzGrid::SetSize(const SynGlyphX::DoubleSize& size) {

		if ((size[0] <= 0.0) || (size[1] <= 0.0)) {

			throw std::invalid_argument("Size in ANTzGrid can not be zero or negative");
		}

		m_size = size;
	}

	const SynGlyphX::DoubleSize& ANTzGrid::GetSize() const {

		return m_size;
	}

	SynGlyphX::DoubleSize ANTzGrid::GetCellSize() const {

		SynGlyphX::DoubleSize cellSize;
		cellSize[0] = m_size[0] / m_segments[0];
		cellSize[1] = m_size[1] / m_segments[1];
		return cellSize;
	}

} //namespace SynGlyphXANTz
