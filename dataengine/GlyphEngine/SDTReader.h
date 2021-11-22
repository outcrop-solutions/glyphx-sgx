#pragma once
#ifndef SDTREADER_H
#define SDTREADER_H

#include "GlyphEngine_Exports.h"
#include "BaseObject.h"
#include "GlyphObject.h"
#include "Data.h"
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
		//typedef std::unordered_map<boost::uuids::uuid, DataSource::SharedPtr, UUIDHash> DatasourceMap;
		//typedef std::unordered_map<boost::uuids::uuid, DataMappingGlyph*, UUIDHash> DataMappingGlyphMap;

		SDTReader();
		~SDTReader(){};

		void ReadSDTFile(const std::string& filename);
		std::vector<BaseObject> GetBaseImages();
		Data* GetDataSource();
		GlyphObject* GetGlyphTemplate();

	private:
		void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree);
		void ImportBaseObjects(const boost::property_tree::wptree& dataTransformPropertyTree);
		void ImportDataSources(const boost::property_tree::wptree& dataTransformPropertyTree);
		void ImportGlyphs(const boost::property_tree::wptree& dataTransformPropertyTree);


		boost::uuids::uuid m_id;
		std::vector<BaseObject> m_baseObjects;
		Data *m_dataSource;
		GlyphObject *m_glyph;

	};
}
#endif // SDTREADER_H

