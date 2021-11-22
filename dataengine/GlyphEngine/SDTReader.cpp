#include "SDTReader.h"
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

		ImportBaseObjects(dataTransformPropertyTree);

		ImportDataSources(dataTransformPropertyTree);

		ImportGlyphs(dataTransformPropertyTree);

	}

	void SDTReader::ImportBaseObjects(const boost::property_tree::wptree& dataTransformPropertyTree)
	{
		boost::optional<const boost::property_tree::wptree&> baseObjectsPropertyTree = dataTransformPropertyTree.get_child_optional(L"BaseObjects");
		if (baseObjectsPropertyTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& baseObjectPropertyTree : baseObjectsPropertyTree.get()) {

				if (baseObjectPropertyTree.first == L"BaseObject") {

					m_baseObjects.push_back(BaseObject(baseObjectPropertyTree.second));
				}
			}
		}
		else {

			m_baseObjects.push_back(BaseObject(dataTransformPropertyTree.get_child(L"BaseImage")));
		}
	}
	
	void SDTReader::ImportDataSources(const boost::property_tree::wptree& dataTransformPropertyTree)
	{

		boost::optional<const boost::property_tree::wptree&> datasourcesPropertyTree = dataTransformPropertyTree.get_child_optional(L"Datasources");
		if (datasourcesPropertyTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& datasourcePropertyTree : datasourcesPropertyTree.get()) {

				if (datasourcePropertyTree.first == L"Datasource") {

					m_dataSource = new Data(datasourcePropertyTree.second);
				}
			}
		}
	}
	
	void SDTReader::ImportGlyphs(const boost::property_tree::wptree& dataTransformPropertyTree)
	{
		for (const boost::property_tree::wptree::value_type& glyphPropertyTree : dataTransformPropertyTree.get_child(L"Glyphs")) {

			if (glyphPropertyTree.first == L"Glyph") {
				m_glyph = new GlyphObject(glyphPropertyTree.second);
				//This is for backwards compatibility, TODO: remove in future versions
				boost::optional<const boost::property_tree::wptree&> inputFieldsPropertyTree = glyphPropertyTree.second.get_child_optional(L"InputFields");
				if (inputFieldsPropertyTree.is_initialized()) {

					for (const boost::property_tree::wptree::value_type& inputfieldProperties : inputFieldsPropertyTree.get()) {

						if (inputfieldProperties.first == L"InputField") {

							//InputField inputfield(inputfieldProperties.second);
							//m_inputFieldManager.SetInputField(m_inputFieldManager.GenerateInputFieldID(inputfield), inputfield);
						}
					}
				}
			}
		}
	}
	
	std::vector<BaseObject> SDTReader::GetBaseImages()
	{ 
		return m_baseObjects; 
	};

	Data* SDTReader::GetDataSource()
	{
		return m_dataSource;
	}

	GlyphObject* SDTReader::GetGlyphTemplate()
	{
		return m_glyph;
	}

}



