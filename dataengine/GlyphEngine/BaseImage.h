#pragma once
#ifndef BASEIMAGE_H
#define BASEIMAGE_H

#include "GlyphEngine_Exports.h"
#include "UtilityTypes.h"
#include "BaseImageProperties.h"
#include "GlyphColor.h"
SGX_PRAGMA_WARNING_PUSH
SGX_PRAGMA_WARNING_DISABLE(4512)
#include <boost/bimap.hpp>
SGX_PRAGMA_WARNING_POP
#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>

namespace GlyphEngine
{
	class GLYPHENGINE BaseImage {

	public:
		typedef boost::property_tree::wptree PropertyTree;

		enum Type {
			Default = 0,
			DownloadedMap,
			UserImage
		};

		BaseImage(BaseImageProperties::ConstSharedPtr properties = nullptr);
		BaseImage(const PropertyTree& propertyTree);
		BaseImage(const BaseImage& baseImage);
		~BaseImage();

		BaseImage& operator=(const BaseImage& baseImage);
		bool operator==(const BaseImage& baseImage) const;
		bool operator!=(const BaseImage& baseImage) const;

		Type GetType() const;

		BaseImageProperties::ConstSharedPtr GetProperties() const;
		void SetProperties(BaseImageProperties::ConstSharedPtr properties);

		void SetPosition(const Vector3& position);
		const Vector3& GetPosition() const;

		void SetRotation(const Vector3& angles);
		const Vector3& GetRotationAngles() const;

		void SetWorldSize(const DoubleSize& worldSize);
		const DoubleSize& GetWorldSize() const;

		void SetGridLineCounts(const IntSize& gridLineCounts);
		IntSize GetGridLineCounts() const;

		void SetGridLinesColor(const GlyphColor& color);
		GlyphColor GetGridLinesColor() const;

		void ExportToPropertyTree(PropertyTree& parentPropertyTree) const;

		static const boost::bimap<Type, std::wstring> s_baseImageTypeStrings;

	private:
		Vector3 m_position;
		Vector3 m_rotationAngles;
		Type m_type;
		std::shared_ptr<BaseImageProperties> m_properties;
		DoubleSize m_worldSize;
		IntSize m_gridLineCounts;
		GlyphColor m_gridLinesColor;
	};
}
#endif // BASEIMAGE_H

