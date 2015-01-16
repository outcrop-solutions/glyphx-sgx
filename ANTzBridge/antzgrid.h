#ifndef SYNGLYPX_ANTZGRID_H
#define SYNGLYPX_ANTZGRID_H

#include "ANTzBridge.h"
#include "antzobject.h"

namespace SynGlyphXANTz {

	class ANTZBRIDGE_API ANTzGrid : public ANTzObject
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

} //namespace SynGlyphXANTz

#endif //SYNGLYPX_ANTZGRID_H