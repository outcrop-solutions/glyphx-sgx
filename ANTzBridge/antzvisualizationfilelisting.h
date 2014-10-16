#ifndef ANTZVISUALIZATIONFILELISTING_H
#define ANTZVISUALIZATIONFILELISTING_H

#include "ANTzBridge.h"
#include "xmlpropertytreefile.h"
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "uuid.h"

namespace SynGlyphX {

	class ANTZBRIDGE_API ANTzVisualizationFileListing : public XMLPropertyTreeFile
	{
	public:
		ANTzVisualizationFileListing();
		ANTzVisualizationFileListing(const std::wstring& nodeFilename,
			const std::wstring& tagFilename,
			const std::wstring& channelFilename,
			const std::wstring& channelMapFilename,
			const std::wstring& baseImageFilename = L"");
		ANTzVisualizationFileListing(const ANTzVisualizationFileListing& fileListing);
		~ANTzVisualizationFileListing();

		ANTzVisualizationFileListing& operator=(const ANTzVisualizationFileListing& fileListing);
		
		bool UsesDefaultImage() const;
		bool IsValid() const;

		void Clear();

		const std::wstring& GetNodeFilename() const;
		const std::wstring& GetTagFilename() const;
		const std::wstring& GetChannelFilename() const;
		const std::wstring& GetChannelMapFilename() const;
		const std::wstring& GetBaseImageFilename() const;

		const boost::uuids::uuid& GetID() const;

	protected:
		virtual void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree);
		virtual void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const;

		std::wstring m_nodeFilename;
		std::wstring m_tagFilename;
		std::wstring m_channelFilename;
		std::wstring m_channelMapFilename;
		std::wstring m_baseImageFilename;
		boost::uuids::uuid m_id;
	};

} //namespace SynGlyphX

#endif //ANTZVISUALIZATIONFILELISTING_H