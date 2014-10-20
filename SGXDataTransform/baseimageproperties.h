#ifndef SYNGLYPHX_BASEIMAGEPROPERTIES_H
#define SYNGLYPHX_BASEIMAGEPROPERTIES_H

#include "sgxdatatransform_global.h"
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT BaseImageProperties
	{
	public:
		BaseImageProperties();
		BaseImageProperties(const boost::property_tree::wptree& propertyTree);
		BaseImageProperties(const BaseImageProperties& properties);
		virtual ~BaseImageProperties();

		virtual bool IsGeographic() const;
		virtual void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_BASEIMAGEPROPERTIES_H