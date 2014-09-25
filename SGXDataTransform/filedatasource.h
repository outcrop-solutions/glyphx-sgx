#ifndef FILEDATASOURCE_H
#define FILEDATASOURCE_H

#include "datasource.h"

namespace SynGlyphX {

	class FileDatasource : public Datasource
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

		SourceType GetType() const;

		const std::wstring& GetFilename() const;
		void ChangeFilename(const std::wstring& filename);

		bool RequiresConversionToDB() const;

		virtual bool IsOriginalDatasourceADatabase() const;
		virtual bool IsFile() const;
		virtual bool CanDatasourceBeFound() const;

		virtual PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree);

		static const SourceTypeBimap s_sourceTypeStrings;

	private:
		SourceType m_type;
	};

} //namespace SynGlyphX

#endif //FILEDATASOURCE_H