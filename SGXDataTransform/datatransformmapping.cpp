#include "datatransformmapping.h"
#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
#include <boost/uuid/uuid_io.hpp>
#include <QtCore/QVariant>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include "databaseservices.h"

namespace SynGlyphX {

	DataTransformMapping::DataTransformMapping() :
		m_baseImage(nullptr),
		m_updated(false),
		m_version(0L)
    {
		m_id = UUIDGenerator::GetNewRandomUUID();
    }

	DataTransformMapping::~DataTransformMapping()
    {
    }

	void DataTransformMapping::ReadFromFile(const std::string& filename) {

        Clear();

        boost::property_tree::wptree filePropertyTree;
		boost::property_tree::read_xml(filename, filePropertyTree);

		boost::property_tree::wptree& dataTransformPropertyTree = filePropertyTree.get_child(L"Transform");

		m_id = dataTransformPropertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id");
		m_version = dataTransformPropertyTree.get<unsigned long>(L"<xmlattr>.version", 0L);

		m_baseImage = BaseImage(dataTransformPropertyTree.get_child(L"BaseImage"));

		m_datasources = DatasourceMaps(dataTransformPropertyTree.get_child(L"Datasources"));

		for (boost::property_tree::wptree::value_type& glyphPropertyTree : dataTransformPropertyTree.get_child(L"Glyphs")) {

			if (glyphPropertyTree.first == L"Glyph") {

				MinMaxGlyphTree::SharedPtr glyphTree(new MinMaxGlyphTree(glyphPropertyTree.second));
				m_glyphTrees.insert(std::pair<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr>(glyphPropertyTree.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), glyphTree));
			}
		}

		m_updated = false;
    }

	void DataTransformMapping::WriteToFile(const std::string& filename, bool resetID) {

		if (m_updated) {
			
			++m_version;
			m_updated = false;
		}

		boost::property_tree::wptree filePropertyTree;

		boost::property_tree::wptree& dataTransformPropertyTreeRoot = filePropertyTree.add(L"Transform", L"");
		dataTransformPropertyTreeRoot.put(L"<xmlattr>.id", m_id);
		dataTransformPropertyTreeRoot.put(L"<xmlattr>.version", m_version);

		m_baseImage.ExportToPropertyTree(dataTransformPropertyTreeRoot);

		m_datasources.ExportToPropertyTree(dataTransformPropertyTreeRoot);

		boost::property_tree::wptree& glyphTreesPropertyTree = dataTransformPropertyTreeRoot.add(L"Glyphs", L"");
		for (auto glyphTree : m_glyphTrees) {
			
			boost::property_tree::wptree& glyphPropertyTree = glyphTree.second->ExportToPropertyTree(glyphTreesPropertyTree);
			glyphPropertyTree.put(L"<xmlattr>.id", glyphTree.first);
		}

		boost::property_tree::write_xml(filename, filePropertyTree);
    }

	const DatasourceMaps& DataTransformMapping::GetDatasources() const {

		return m_datasources;
	}

	void DataTransformMapping::Clear() {

		m_datasources.Clear();
		m_glyphTrees.clear();
		m_baseImage = BaseImage(nullptr);
    }

	void DataTransformMapping::RemoveDatasource(const boost::uuids::uuid& id) {

		try {
			RemoveDatasource(id);
		}
		catch (const std::invalid_argument& e) {
			throw;
		}
	}

	boost::uuids::uuid DataTransformMapping::AddFileDatasource(FileDatasource::SourceType type,
		const std::wstring& name,
        const std::wstring& host,
        unsigned int port,
        const std::wstring& username,
        const std::wstring& password) {

		boost::uuids::uuid id = m_datasources.AddFileDatasource(type, name, host, port, username, password);

		m_updated = true;

        return id;
    }

	void DataTransformMapping::AddTables(const boost::uuids::uuid& id, const std::vector<std::wstring>& tables) {

		if (m_datasources.AddTables(id, tables)) {

			m_updated = true;
		}
    }

	boost::uuids::uuid DataTransformMapping::AddGlyphTree(const MinMaxGlyphTree::SharedPtr glyphTree) {

		boost::uuids::uuid id = UUIDGenerator::GetNewRandomUUID();

		m_glyphTrees.insert(std::pair<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr>(id, glyphTree));

		m_updated = true;

		return id;
	}

	const DataTransformMapping::MinMaxGlyphTreeMap& DataTransformMapping::GetGlyphTrees() const {

		return m_glyphTrees;
	}

	bool DataTransformMapping::IsTransformable() const {

		return (m_datasources.HasDatasources() && (!m_glyphTrees.empty()));
	}

	void DataTransformMapping::SetBaseImage(const BaseImage& baseImage) {

		m_baseImage = baseImage;
		m_updated = true;
	}

	const BaseImage& DataTransformMapping::GetBaseImage() const {

		return m_baseImage;
	}
	
	void DataTransformMapping::SetInputField(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index, const InputField& inputfield) {

		MinMaxGlyphTree::SharedPtr glyphTree = m_glyphTrees[treeID];

		double min = 0.0;
		double max = 0.0;

		if (inputfield.IsNumeric()) {

			QSqlDatabase db = QSqlDatabase::database(QString::fromStdWString(boost::uuids::to_wstring(inputfield.GetDatasourceID())));
			QSqlQuery query(db);
			query.prepare(QString("SELECT  MIN(%1), MAX(%1) FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + QString::fromStdWString(inputfield.GetTable()));

			query.exec();
			query.first();
			QSqlRecord record = query.record();

			min = record.value(0).toDouble();
			max = record.value(1).toDouble();
		}

		glyphTree->SetInputField(node, index, inputfield, min, max);

		m_updated = true;
	}

	void DataTransformMapping::ClearInputBinding(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index) {

		MinMaxGlyphTree::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->ClearInputBinding(node, index);

		m_updated = true;
	}

	void DataTransformMapping::SetPositionXYMinMaxToGeographicForAllGlyphTrees(const GeographicBoundingBox& boundingBox) {

		for (auto minMaxTree : m_glyphTrees) {

			MinMaxGlyphTree::iterator& rootGlyph = minMaxTree.second->root();

			GlyphProperties minProperties = rootGlyph->GetMinGlyph();
			Vector3 position = minProperties.GetPosition();
			position[0] = -180.0;
			position[1] = -90.0;
			minProperties.SetPosition(position);
			rootGlyph->SetMinGlyphProperties(minProperties);

			GlyphMappableProperties diffProperties = rootGlyph->GetDifference();
			Vector3 diffPosition = diffProperties.GetPosition();
			diffPosition[0] = 360.0;
			diffPosition[1] = 180.0;
			diffProperties.SetPosition(diffPosition);
			rootGlyph->SetDifference(diffProperties);

			InputBinding bindingX = rootGlyph->GetInputBinding(0);
			InputBinding bindingY = rootGlyph->GetInputBinding(1);

			bindingX.SetMinMax(boundingBox.GetSWCorner().get<0>(), boundingBox.GetNECorner().get<0>());
			bindingY.SetMinMax(boundingBox.GetSWCorner().get<1>(), boundingBox.GetNECorner().get<1>());

			rootGlyph->SetInputBinding(0, bindingX);
			rootGlyph->SetInputBinding(1, bindingY);
		}
	}

	void DataTransformMapping::GetPositionXYForAllGlyphTrees(std::vector<GeographicPoint>& points) const {

		for (auto minMaxTree : m_glyphTrees) {

			MinMaxGlyphTree::iterator& rootGlyph = minMaxTree.second->root();

			QVariantList queryResultDataX;
			QVariantList queryResultDataY;
			const MinMaxGlyphTree::InputFieldMap& inputFields = minMaxTree.second->GetInputFields();

			queryResultDataX = DatabaseServices::RunSqlQuery(inputFields.at(rootGlyph->GetInputBinding(0).GetInputFieldID()));
			queryResultDataY = DatabaseServices::RunSqlQuery(inputFields.at(rootGlyph->GetInputBinding(1).GetInputFieldID()));

			size_t numGlyphs = queryResultDataX.length();
			for (int i = 0; i < numGlyphs; ++i) {
				points.push_back(GeographicPoint(queryResultDataX[i].toDouble(), queryResultDataY[i].toDouble()));
			}
		}
	}

	const boost::uuids::uuid& DataTransformMapping::GetID() const {

		return m_id;
	}

	const unsigned long DataTransformMapping::GetVersion() const {

		return m_version;
	}

	void DataTransformMapping::UpdateDatasourceName(const boost::uuids::uuid& id, const std::wstring& name) {

		try {

			m_datasources.ChangeDatasourceName(id, name);
		}
		catch (const std::invalid_argument& e) {

			throw;
		}
	}

} //namespace SynGlyphX