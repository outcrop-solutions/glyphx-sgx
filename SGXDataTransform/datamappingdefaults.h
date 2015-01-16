#ifndef SYNGLYPHX_DATAMAPPINGDEFAULTS_H
#define SYNGLYPHX_DATAMAPPINGDEFAULTS_H

#include "sgxdatatransform_global.h"
#include <boost/bimap.hpp>
#include <boost/property_tree/ptree.hpp>
#include "datamappingglyph.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DataMappingDefaults
	{
	public:
		typedef boost::property_tree::wptree PropertyTree;

		DataMappingDefaults();
		DataMappingDefaults(const PropertyTree& propertyTree);
		DataMappingDefaults(const DataMappingDefaults& defaults);
		~DataMappingDefaults();

		DataMappingDefaults& operator=(const DataMappingDefaults& defaults);
		bool operator==(const DataMappingDefaults& defaults) const;
		bool operator!=(const DataMappingDefaults& defaults) const;

		void Clear();

		void ExportToPropertyTree(PropertyTree& parentPropertyTree) const;

		void SetTagField(DataMappingGlyph::MappableField tagField);
		DataMappingGlyph::MappableField GetTagField() const;
		bool IsDefaultTagFieldSet() const;

		void SetDefaultTagValue(const std::wstring& value);
		const std::wstring& GetDefaultTagValue() const;

		static const boost::bimap<DataMappingGlyph::MappableField, std::wstring> s_tagFieldStrings;
		static const std::wstring s_propertyTreeName;

	private:
		DataMappingGlyph::MappableField m_tagField;
		std::wstring m_defaultTagValue;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGDEFAULTS_H