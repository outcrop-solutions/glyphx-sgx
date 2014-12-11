#ifndef SYNGLYPX_ANTZGRID_H
#define SYNGLYPX_ANTZGRID_H

#include "ANTzBridge.h"
#include "glyphnumericmappableproperties.h"

namespace SynGlyphX {

	class ANTZBRIDGE_API ANTzGrid : public GlyphNumericMappableProperties
	{
	public:
		ANTzGrid();
		ANTzGrid(const ANTzGrid& grid);
		~ANTzGrid();

		ANTzGrid& operator=(const ANTzGrid& grid);

		unsigned int GetTextureID() const;
		void SetTextureID(unsigned int textureID);

	private:
		unsigned int m_textureID;
	};

} //namespace SynGlyphX

#endif //SYNGLYPX_ANTZGRID_H