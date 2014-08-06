#ifndef SYNGLYPHX_BASEIMAGEPROPERTIES_H
#define SYNGLYPHX_BASEIMAGEPROPERTIES_H

#include "sgxdatatransform_global.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT BaseImageProperties
	{
	public:
		BaseImageProperties();
		BaseImageProperties(const BaseImageProperties& properties);
		virtual ~BaseImageProperties();

		virtual bool IsGeographic() const;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_BASEIMAGEPROPERTIES_H