#include "SDTReader.h"
#include "FileDataSource.h"
#include "DatabaseServerDataSource.h"
#include <boost/property_tree/xml_parser.hpp>

namespace GlyphEngine
{
	SDTReader::SDTReader()
	{
		
	}

	void SDTReader::ReadSDTFile(const std::string& filename) 
	{
		boost::property_tree::wptree m_originalPropertyTree;
		boost::property_tree::read_xml(filename, m_originalPropertyTree, boost::property_tree::xml_parser::trim_whitespace);

		ImportFromPropertyTree(m_originalPropertyTree);
	}

	void SDTReader::ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree)
	{
		const boost::property_tree::wptree& dataTransformPropertyTree = filePropertyTree.get_child(L"Transform");

		m_id = dataTransformPropertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id");

		boost::optional<const boost::property_tree::wptree&> baseObjectsPropertyTree = dataTransformPropertyTree.get_child_optional(L"BaseObjects");
		if (baseObjectsPropertyTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& baseObjectPropertyTree : baseObjectsPropertyTree.get()) {

				if (baseObjectPropertyTree.first == L"BaseObject") {

					m_baseObjects.push_back(BaseImage(baseObjectPropertyTree.second));
				}
			}
		}
		else {

			m_baseObjects.push_back(BaseImage(dataTransformPropertyTree.get_child(L"BaseImage")));
		}

		boost::optional<const boost::property_tree::wptree&> datasourcesPropertyTree = dataTransformPropertyTree.get_child_optional(L"Datasources");
		if (datasourcesPropertyTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& datasourcePropertyTree : datasourcesPropertyTree.get()) {

				std::wstring fileSourceString = GlyphEngine::DataSource::s_sourceTypeStrings.left.at(GlyphEngine::DataSource::SourceType::File);
				DataSource::SharedPtr newDataSource = nullptr;
				if ((datasourcePropertyTree.first == fileSourceString) || (datasourcePropertyTree.first == L"Datasource")) {

					DataSource::SourceType source = DataSource::s_sourceTypeStrings.right.at(datasourcePropertyTree.second.get_optional<std::wstring>(L"<xmlattr>.source").get_value_or(fileSourceString));

					if (source == DataSource::SourceType::File) {

						newDataSource = std::make_shared<FileDataSource>(datasourcePropertyTree.second);
					}
					else if (source == DataSource::SourceType::DatabaseServer) {

						newDataSource = std::make_shared<DatabaseServerDataSource>(datasourcePropertyTree.second);
					}
				}

				if (newDataSource != nullptr) {

					m_datasources.insert(std::pair<boost::uuids::uuid, DataSource::SharedPtr>(datasourcePropertyTree.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), newDataSource));
				}
			}
		}

		for (const boost::property_tree::wptree::value_type& glyphPropertyTree : dataTransformPropertyTree.get_child(L"Glyphs")) {

			if (glyphPropertyTree.first == L"Glyph") {
				DataMappingGlyph *glyph = new DataMappingGlyph(glyphPropertyTree.second);
				//This is for backwards compatibility, TODO: remove in future versions
				boost::optional<const boost::property_tree::wptree&> inputFieldsPropertyTree = glyphPropertyTree.second.get_child_optional(L"InputFields");
				if (inputFieldsPropertyTree.is_initialized()) {

					for (const boost::property_tree::wptree::value_type& inputfieldProperties : inputFieldsPropertyTree.get()) {

						if (inputfieldProperties.first == L"InputField") {

							InputField inputfield(inputfieldProperties.second);
							m_inputFieldManager.SetInputField(m_inputFieldManager.GenerateInputFieldID(inputfield), inputfield);
						}
					}
				}
				boost::uuids::uuid treeID = glyphPropertyTree.second.get<boost::uuids::uuid>(L"<xmlattr>.id");
				m_glyphs.insert(std::pair<boost::uuids::uuid, DataMappingGlyph*>(treeID, glyph));

			}
		}

	}


}



