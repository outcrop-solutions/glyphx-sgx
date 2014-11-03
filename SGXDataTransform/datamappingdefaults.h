#ifndef SYNGLYPHX_DATAMAPPINGDEFAULTS_H
#define SYNGLYPHX_DATAMAPPINGDEFAULTS_H

#include "sgxdatatransform_global.h"
#include <boost/bimap.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DataMappingDefaults
	{
	public:
		typedef boost::property_tree::wptree PropertyTree;

		//This enum may be moved once the glyph property system is written
		enum TagFieldPropertyDefault {

			None = -1,
			PositionX = 0,
			PositionY = 1,
			PositionZ = 2,
			RotationX = 3,
			RotationY = 4,
			RotationZ = 5,
			ScaleX = 6,
			ScaleY = 7,
			ScaleZ = 8,
			Color = 9,
			Transparency = 10
		};

		DataMappingDefaults();
		DataMappingDefaults(const PropertyTree& propertyTree);
		DataMappingDefaults(const DataMappingDefaults& defaults);
		~DataMappingDefaults();

		DataMappingDefaults& operator=(const DataMappingDefaults& defaults);
		bool operator==(const DataMappingDefaults& defaults) const;
		bool operator!=(const DataMappingDefaults& defaults) const;

		void Clear();

		void ExportToPropertyTree(PropertyTree& parentPropertyTree) const;

		void SetTagField(TagFieldPropertyDefault tagField);
		TagFieldPropertyDefault GetTagField() const;

		void SetDefaultTagValue(const std::wstring& value);
		const std::wstring& GetDefaultTagValue() const;

		static const boost::bimap<TagFieldPropertyDefault, std::wstring> s_tagFieldStrings;
		static const std::wstring s_propertyTreeName;

	private:
		TagFieldPropertyDefault m_tagField;
		std::wstring m_defaultTagValue;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGDEFAULTS_H