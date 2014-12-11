#ifndef SYNGLYPHX_BASEIMAGE_H
#define SYNGLYPHX_BASEIMAGE_H

#include "sgxdatatransform_global.h"
#include "foundationtypes.h"
#include "baseimageproperties.h"
#include <boost/bimap.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT BaseImage
	{
	public:
		typedef std::array<double, 2> Size;
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
		bool operator==(const BaseImage& baseImage) const;
		bool operator!=(const BaseImage& baseImage) const;

		Type GetType() const;

		const BaseImageProperties* const GetProperties() const;

		void SetPosition(const Vector3& position);
		const Vector3& GetPosition() const;

		void SetRotation(const Vector3& angles);
		const Vector3& GetRotationAngles() const;

		void SetWorldSize(const Size& worldSize);
		const Size& GetWorldSize() const;

		void ExportToPropertyTree(PropertyTree& parentPropertyTree) const;

		static const boost::bimap<Type, std::wstring> s_baseImageTypeStrings;

	private:
		void ChangeProperties(const BaseImageProperties* const properties);

		Vector3 m_position;
		Vector3 m_rotationAngles;
		Type m_type;
		BaseImageProperties* m_properties;
		Size m_worldSize;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_BASEIMAGE_H