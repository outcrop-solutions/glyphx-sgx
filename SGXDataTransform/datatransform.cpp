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

	void DataTransform::TransformToCSV(const std::string& filename) const {

		GlyphTree::ConstSharedVector trees;

		for (auto minMaxTree : m_glyphTrees) {

			MinMaxGlyphTree::iterator minMaxGlyph = minMaxTree.second->root();
			const InputField& positionX = minMaxGlyph->GetInputField(0);
			const InputField& positionY = minMaxGlyph->GetInputField(1);

			QSqlDatabase dbPositionX = QSqlDatabase::database(QString::fromStdWString(boost::uuids::to_wstring(positionX.GetDatasourceID())));
			QSqlDatabase dbPositionY = QSqlDatabase::database(QString::fromStdWString(boost::uuids::to_wstring(positionX.GetDatasourceID())));

			QSqlQuery queryPositionX(dbPositionX);
			queryPositionX.prepare(QString("SELECT %1 FROM ").arg("\"" + QString::fromStdWString(positionX.GetField()) + "\"") + QString::fromStdWString(positionX.GetTable()));

			QSqlQuery queryPositionY(dbPositionY);
			queryPositionY.prepare(QString("SELECT %1 FROM ").arg("\"" + QString::fromStdWString(positionY.GetField()) + "\"") + QString::fromStdWString(positionY.GetTable()));

			queryPositionX.exec();
			queryPositionY.exec();

			while (queryPositionX.next()) {

				queryPositionY.next();
				Vector3 position;
				Vector3 difference = minMaxGlyph->GetDifference().GetPosition();
				GlyphProperties glyph = minMaxGlyph->GetMinGlyph();
				Vector3 min = glyph.GetPosition();
				position[0] = (queryPositionX.value(0).toDouble() - positionX.GetMin()) / (positionX.GetMax() - positionX.GetMin()) * difference[0] + min[0];
				position[1] = (queryPositionY.value(0).toDouble() - positionY.GetMin()) / (positionY.GetMax() - positionY.GetMin()) * difference[1] + min[1];
				position[2] = 0.0;
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