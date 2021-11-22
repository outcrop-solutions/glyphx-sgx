#pragma once
#ifndef BASEIMAGE_H
#define BASEIMAGE_H

#include "GlyphEngine_Exports.h"
#include "utility_types.h"
#include "ObjectColor.h"
SGX_PRAGMA_WARNING_PUSH
SGX_PRAGMA_WARNING_DISABLE(4512)
#include <boost/bimap.hpp>
SGX_PRAGMA_WARNING_POP
#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>

namespace GlyphEngine
{
	class GLYPHENGINE BaseObject {

	public:
		typedef boost::property_tree::wptree PropertyTree;

		enum Type {
			Default = 0,
			DownloadedMap,
			UserImage
		};

		//BaseObject(BaseImageProperties::ConstSharedPtr properties = nullptr);
		BaseObject(const PropertyTree& propertyTree);
		BaseObject(const BaseObject& baseImage);
		~BaseObject();

		BaseObject& operator=(const BaseObject& baseImage);
		bool operator==(const BaseObject& baseImage) const;
		bool operator!=(const BaseObject& baseImage) const;

		Type GetType() const;

		//BaseImageProperties::ConstSharedPtr GetProperties() const;
		//void SetProperties(BaseImageProperties::ConstSharedPtr properties);

		void SetPosition(const Vector3& position);
		const Vector3& GetPosition() const;

		void SetRotation(const Vector3& angles);
		const Vector3& GetRotationAngles() const;

		void SetWorldSize(const DoubleSize& worldSize);
		const DoubleSize& GetWorldSize() const;

		void SetGridLineCounts(const IntSize& gridLineCounts);
		IntSize GetGridLineCounts() const;

		void SetGridLinesColor(const ObjectColor& color);
		ObjectColor GetGridLinesColor() const;

		void ExportToPropertyTree(PropertyTree& parentPropertyTree) const;

		//static const boost::bimap<Type, std::wstring> s_baseImageTypeStrings;

	private:
		Vector3 m_position;
		Vector3 m_rotationAngles;
		Type m_type;
		//std::shared_ptr<BaseImageProperties> m_properties;
		DoubleSize m_worldSize;
		IntSize m_gridLineCounts;
		ObjectColor m_gridLinesColor;
	};
}
#endif // BASEIMAGE_H

