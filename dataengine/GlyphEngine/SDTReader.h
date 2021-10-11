#pragma once
#ifndef SDTREADER_H
#define SDTREADER_H

#include "GlyphEngine_Exports.h"
#include "BaseImage.h"
#include "DataSource.h"
#include "InputFieldManager.h"
#include "DataMappingGlyph.h"
#include <iostream>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace GlyphEngine
{
	struct UUIDHash {
		std::size_t operator()(const boost::uuids::uuid& uuid) const
		{
			return boost::uuids::hash_value(uuid);
		}
	};

	class GLYPHENGINE SDTReader {

	public:
		typedef std::unordered_map<boost::uuids::uuid, GlyphEngine::DataSource::SharedPtr, GlyphEngine::UUIDHash> DatasourceMap;
		typedef std::unordered_map<boost::uuids::uuid, DataMappingGlyph*, UUIDHash> DataMappingGlyphMap;

		SDTReader();
		~SDTReader(){};

		void ReadSDTFile(const std::string& filename);

	private:
		void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree);

		boost::uuids::uuid m_id;
		std::vector<BaseImage> m_baseObjects;
		DatasourceMap m_datasources;
		InputFieldManager m_inputFieldManager;
		DataMappingGlyphMap m_glyphs;

	};
}
#endif // SDTREADER_H

