#ifndef DATASOURCEMAPS_H
#define DATASOURCEMAPS_H

#include "sgxdatatransform_global.h"
#include <string>
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include "UUID.h"
#include "filedatasource.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DatasourceMaps
	{
	public:
		typedef boost::property_tree::wptree PropertyTree;
		typedef std::unordered_map<boost::uuids::uuid, FileDatasource, SynGlyphX::UUIDHash> FileDatasourceMap;

		DatasourceMaps();
		DatasourceMaps(const PropertyTree& propertyTree);
		~DatasourceMaps();

		bool operator==(const DatasourceMaps& maps) const;
		bool operator!=(const DatasourceMaps& maps) const;

		bool HasDatasources() const;
		void Clear();
		bool EnableTables(const boost::uuids::uuid& id, const Datasource::TableSet& tables, bool enable = true);

		void ChangeDatasourceName(const boost::uuids::uuid& id, const std::wstring& name);

		PropertyTree& ExportToPropertyTree(PropertyTree& propertyTreeParent) const;

		void RemoveDatasource(const boost::uuids::uuid& id);

		const FileDatasourceMap& GetFileDatasources() const;
		boost::uuids::uuid AddFileDatasource(FileDatasource::SourceType type,
			const std::wstring& name,
			const std::wstring& host = L"localhost",
			unsigned int port = 0,
			const std::wstring& username = L"",
			const std::wstring& password = L"");

	private:
		FileDatasourceMap m_fileDatasources;
	};

} //namespace SynGlyphX

#endif //DATASOURCEMAPS_H