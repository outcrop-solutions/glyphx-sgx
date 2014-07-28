#include "datatransform.h"
#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
#include <boost/uuid/uuid_io.hpp>
#include "csvreaderwriter.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCore/QVariant>

namespace SynGlyphX {

    DataTransform::DataTransform()
    {
    }

    DataTransform::~DataTransform()
    {
    }

    void DataTransform::ReadFromFile(const std::string& filename) {

        Clear();

        boost::property_tree::wptree dataTransformPropertyTree;
		boost::property_tree::read_xml(filename, dataTransformPropertyTree);

		for (boost::property_tree::wptree::value_type& datasourceValue : dataTransformPropertyTree.get_child(L"Datasources")) {

			if (datasourceValue.first == L"Datasource") {

				Datasource datasource(datasourceValue.second);
				m_datasources.insert(std::pair<boost::uuids::uuid, Datasource>(datasourceValue.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), datasource));
			}
		}

		for (boost::property_tree::wptree::value_type& glyphPropertyTree : dataTransformPropertyTree.get_child(L"Glyphs")) {

			if (glyphPropertyTree.first == L"Glyph") {

				MinMaxGlyphTree::SharedPtr glyphTree(new MinMaxGlyphTree(glyphPropertyTree.second));
				m_glyphTrees.insert(std::pair<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr>(glyphPropertyTree.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), glyphTree));
			}
		}
    }

    void DataTransform::WriteToFile(const std::string& filename) const {

		boost::property_tree::wptree dataTransformPropertyTreeRoot;

		boost::property_tree::wptree& datasourcesPropertyTree = dataTransformPropertyTreeRoot.add(L"Datasources", L"");
		for (auto datasource : m_datasources) {

			Datasource::PropertyTree& datasourcePropertyTree = datasource.second.ExportToPropertyTree(datasourcesPropertyTree);
			datasourcePropertyTree.put(L"<xmlattr>.id", datasource.first);
		}

		boost::property_tree::wptree& glyphTreesPropertyTree = dataTransformPropertyTreeRoot.add(L"Glyphs", L"");
		for (auto glyphTree : m_glyphTrees) {
			
			boost::property_tree::wptree& glyphPropertyTree = glyphTree.second->ExportToPropertyTree(glyphTreesPropertyTree);
			glyphPropertyTree.put(L"<xmlattr>.id", glyphTree.first);
		}

		boost::property_tree::write_xml(filename, dataTransformPropertyTreeRoot);
    }

	const DataTransform::DatasourceMap& DataTransform::GetDatasources() const {

        return m_datasources;
    }

    void DataTransform::Clear() {

        m_datasources.clear();
		m_glyphTrees.clear();
    }

	boost::uuids::uuid DataTransform::AddDatasource(const std::wstring& name,
        Datasource::SourceType type,
        const std::wstring& host,
        unsigned int port,
        const std::wstring& username,
        const std::wstring& password) {

		boost::uuids::uuid id = m_uuidGenerator();

        Datasource datasource(name, type, host, port, username, password);

		m_datasources.insert(std::pair<boost::uuids::uuid, Datasource>(id, datasource));

        return id;
    }

	void DataTransform::AddTables(const boost::uuids::uuid& id, const std::vector<std::wstring>& tables) {

        m_datasources.at(id).AddTables(tables);
    }

	boost::uuids::uuid DataTransform::AddGlyphTree(const MinMaxGlyphTree::SharedPtr glyphTree) {

		boost::uuids::uuid id = m_uuidGenerator();

		m_glyphTrees.insert(std::pair<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr>(id, glyphTree));

		return id;
	}

	const DataTransform::MinMaxGlyphTreeMap& DataTransform::GetGlyphTrees() const {

		return m_glyphTrees;
	}

	bool DataTransform::IsTransformable() const {

		return ((!m_datasources.empty()) && (!m_glyphTrees.empty()));
	}

	void DataTransform::RunSqlQuery(const InputField& inputfield, std::vector<double>& results) const {

		QSqlDatabase db= QSqlDatabase::database(QString::fromStdWString(boost::uuids::to_wstring(inputfield.GetDatasourceID())));
		QSqlQuery query(db);
		query.prepare(QString("SELECT %1 FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + QString::fromStdWString(inputfield.GetTable()));
		query.exec();
		while (query.next()) {
			results.push_back(query.value(0).toDouble());
		}
	}

	void DataTransform::TransformToCSV(const std::string& filename) const {

		GlyphTree::ConstSharedVector trees;

		for (auto minMaxTree : m_glyphTrees) {

			MinMaxGlyphTree::iterator minMaxGlyph = minMaxTree.second->root();
			const InputField& positionXInput = minMaxGlyph->GetInputField(0);
			const InputField& positionYInput = minMaxGlyph->GetInputField(1);
			const InputField& positionZInput = minMaxGlyph->GetInputField(2);

			std::vector<double> xPositions;
			std::vector<double> yPositions;
			std::vector<double> zPositions;

			if (positionXInput.IsValid()) {
				RunSqlQuery(positionXInput, xPositions);
			}

			if (positionYInput.IsValid()) {
				RunSqlQuery(positionYInput, yPositions);
			}

			if (positionZInput.IsValid()) {
				RunSqlQuery(positionZInput, zPositions);
			}

			for (unsigned int i = 0; i < std::max(std::max(xPositions.size(), yPositions.size()), zPositions.size()); ++i) {

				Vector3 position;
				Vector3 difference = minMaxGlyph->GetDifference().GetPosition();
				GlyphProperties glyph = minMaxGlyph->GetMinGlyph();
				Vector3 min = glyph.GetPosition();
				position[0] = min[0];
				if (!xPositions.empty()) {
					position[0] += (xPositions[i] - positionXInput.GetMin()) / (positionXInput.GetMax() - positionXInput.GetMin()) * difference[0];
				}
				position[1] = min[1];
				if (!yPositions.empty()) {
					position[1] += (yPositions[i] - positionYInput.GetMin()) / (positionYInput.GetMax() - positionYInput.GetMin()) * difference[1];
				}
				position[2] = min[2];
				if (!yPositions.empty()) {
					position[2] += (zPositions[i] - positionZInput.GetMin()) / (positionZInput.GetMax() - positionZInput.GetMin()) * difference[2];
				}

				glyph.SetPosition(position);

				GlyphTree::SharedPtr glyphTree(new GlyphTree());
				glyphTree->insert(glyph);

				AddChildrenToGlyphTree(glyphTree, glyphTree->root(), minMaxTree.second, minMaxGlyph);

				trees.push_back(glyphTree);
			}
		}

		CSVReaderWriter& writer = CSVReaderWriter::GetInstance();
		writer.Write(filename, trees);
	}

	void DataTransform::AddChildrenToGlyphTree(GlyphTree::SharedPtr tree, GlyphTree::iterator newNode, MinMaxGlyphTree::SharedPtr minMaxTree, MinMaxGlyphTree::iterator node) const {

		for (int i = 0; i < minMaxTree->children(node); ++i) {

			MinMaxGlyphTree::iterator child = minMaxTree->child(node, i);
			GlyphTree::iterator newChild = tree->insert(newNode, child->GetMinGlyph());
			AddChildrenToGlyphTree(tree, newChild, minMaxTree, child);
		}
	}

} //namespace SynGlyphX