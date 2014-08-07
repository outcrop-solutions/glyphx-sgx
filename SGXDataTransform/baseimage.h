#ifndef SYNGLYPHX_BASEIMAGE_H
#define SYNGLYPHX_BASEIMAGE_H

#include "sgxdatatransform_global.h"
#include "baseimageproperties.h"
#include <boost/bimap.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT BaseImage
	{
	public:
		typedef boost::property_tree::wptree PropertyTree;

		enum Type {
			Default = 0,
			DownloadedMap,
			UserImage
		};

		BaseImage(const BaseImageProperties* const properties);
		BaseImage(const PropertyTree& propertyTree);
		BaseImage(const BaseImage& baseImage);
		~BaseImage();

		BaseImage& operator=(const BaseImage& baseImage);

		Type GetType() const;

		const BaseImageProperties* const GetProperties() const;

		void ExportToPropertyTree(PropertyTree& parentPropertyTree) const;

		static const boost::bimap<Type, std::wstring> s_baseImageTypeStrings;

	private:
		void ChangeProperties(const BaseImageProperties* const properties);

		Type m_type;
		BaseImageProperties* m_properties;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_BASEIMAGE_H