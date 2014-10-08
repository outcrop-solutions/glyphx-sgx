#ifndef FILEDATASOURCE_H
#define FILEDATASOURCE_H

#include "sgxdatatransform_global.h"
#include "datasource.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT FileDatasource : public Datasource
	{
	public:
		enum SourceType {
			SQLITE3,
			CSV,
			KML,
		};

		typedef boost::bimap<SourceType, std::wstring> SourceTypeBimap;
		typedef boost::property_tree::wptree PropertyTree;

		FileDatasource(SourceType type, const std::wstring& filename, const std::wstring& host = L"localhost", unsigned int port = 0, const std::wstring& username = L"", const std::wstring& password = L"");
		FileDatasource(const PropertyTree& propertyTree);
		FileDatasource(const FileDatasource& datasource);
		virtual ~FileDatasource();

		FileDatasource& operator=(const FileDatasource& datasource);
		bool operator==(const FileDatasource& datasource) const;
		bool operator!=(const FileDatasource& datasource) const;

		SourceType GetType() const;

		const std::wstring& GetFilename() const;
		void ChangeFilename(const std::wstring& filename);

		bool RequiresConversionToDB() const;

		virtual bool IsOriginalDatasourceADatabase() const;
		virtual bool CanDatasourceHaveMultipleTables() const;
		virtual bool IsFile() const;
		virtual bool CanDatasourceBeFound() const;
		virtual const std::wstring& GetFormattedName() const;

		virtual PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree);

		static const SourceTypeBimap s_sourceTypeStrings;

	private:
		SourceType m_type;
		std::wstring m_formattedName;
	};

} //namespace SynGlyphX

#endif //FILEDATASOURCE_H