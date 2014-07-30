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

	void DataTransform::RunSqlQuery(const InputField& inputfield, std::vector<std::wstring>& results) const {

		QSqlDatabase db = QSqlDatabase::database(QString::fromStdWString(boost::uuids::to_wstring(inputfield.GetDatasourceID())));
		QSqlQuery query(db);
		query.prepare(QString("SELECT %1 FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + QString::fromStdWString(inputfield.GetTable()));
		query.exec();
		while (query.next()) {
			results.push_back(query.value(0).toString().toStdWString());
		}
	}

	void DataTransform::TransformToCSV(const std::string& filename) const {

		GlyphTree::ConstSharedVector trees;

		for (auto minMaxTree : m_glyphTrees) {

			MinMaxGlyphTree::const_iterator minMaxGlyph = minMaxTree.second->root().constify();

			//subtract 1 since tag input field needs to be handled separately with strings
			std::vector<double> doubleValues[MinMaxGlyph::NumInputFields - 1];
			std::vector<std::wstring> tagValues;

			for (int k = 0; k < MinMaxGlyph::NumInputFields - 1; ++k) {

				const InputField& inputField = minMaxGlyph->GetInputField(k);
				if (inputField.IsValid()) {
					RunSqlQuery(inputField, doubleValues[k]);
				}
			}

			//Run Tag Sql Query separately
			const InputField& inputField = minMaxGlyph->GetInputField(MinMaxGlyph::NumInputFields - 1);
			if (inputField.IsValid()) {
				RunSqlQuery(inputField, tagValues);
			}

			size_t numGlyphs = tagValues.size();
			for (int j = 0; j < MinMaxGlyph::NumInputFields - 1; ++j) {
				
				numGlyphs = std::max(numGlyphs, doubleValues[j].size());
			}

			const GlyphMappableProperties& difference = minMaxGlyph->GetDifference();

			for (unsigned int i = 0; i < numGlyphs; ++i) {

				GlyphTree::SharedPtr glyphTree(new GlyphTree());
				
				GlyphProperties glyph = minMaxGlyph->GetMinGlyph();

				Vector3 mappedVector3;

				mappedVector3[0] = LinearInterpolate(minMaxGlyph->GetInputField(0), glyph.GetPosition()[0], difference.GetPosition()[0], doubleValues[0], i);
				mappedVector3[1] = LinearInterpolate(minMaxGlyph->GetInputField(1), glyph.GetPosition()[1], difference.GetPosition()[1], doubleValues[1], i);
				mappedVector3[2] = LinearInterpolate(minMaxGlyph->GetInputField(2), glyph.GetPosition()[2], difference.GetPosition()[2], doubleValues[2], i);
				glyph.SetPosition(mappedVector3);

				mappedVector3[0] = LinearInterpolate(minMaxGlyph->GetInputField(3), glyph.GetRotation()[0], difference.GetRotation()[0], doubleValues[3], i);
				mappedVector3[1] = LinearInterpolate(minMaxGlyph->GetInputField(4), glyph.GetRotation()[1], difference.GetRotation()[1], doubleValues[4], i);
				mappedVector3[2] = LinearInterpolate(minMaxGlyph->GetInputField(5), glyph.GetRotation()[2], difference.GetRotation()[2], doubleValues[5], i);
				glyph.SetRotation(mappedVector3);

				mappedVector3[0] = LinearInterpolate(minMaxGlyph->GetInputField(6), glyph.GetScale()[0], difference.GetScale()[0], doubleValues[6], i);
				mappedVector3[1] = LinearInterpolate(minMaxGlyph->GetInputField(7), glyph.GetScale()[1], difference.GetScale()[1], doubleValues[7], i);
				mappedVector3[2] = LinearInterpolate(minMaxGlyph->GetInputField(8), glyph.GetScale()[2], difference.GetScale()[2], doubleValues[8], i);
				glyph.SetScale(mappedVector3);

				Color color = ColorRGBInterpolate(minMaxGlyph->GetInputField(9), glyph.GetColor(), difference.GetColor(), doubleValues[9], i);
				color[3] = LinearInterpolate(minMaxGlyph->GetInputField(10), glyph.GetColor()[3], difference.GetColor()[3], doubleValues[10], i);
				glyph.SetColor(color);

				glyph.SetRatio(LinearInterpolate(minMaxGlyph->GetInputField(11), glyph.GetRatio(), difference.GetRatio(), doubleValues[11], i));

				glyph.SetTag(tagValues[i]);

				glyphTree->insert(glyph);

				AddChildrenToGlyphTree(glyphTree, glyphTree->root(), minMaxTree.second, minMaxGlyph);

				trees.push_back(glyphTree);
			}
		}

		CSVReaderWriter& writer = CSVReaderWriter::GetInstance();
		writer.Write(filename, trees);
	}

	GlyphProperties DataTransform::ProcessMinMaxGlyph(const MinMaxGlyphTree::const_iterator& glyph) const {

		return GlyphProperties();
	}

	Color DataTransform::ColorRGBInterpolate(const InputField& inputfield, const Color& min, const Color& difference, const std::vector<double>& input, int index) const {

		Color output = min;
		if (!input.empty()) {
			output[0] += (input[index] - inputfield.GetMin()) / (inputfield.GetMax() - inputfield.GetMin()) * difference[0];
			output[1] += (input[index] - inputfield.GetMin()) / (inputfield.GetMax() - inputfield.GetMin()) * difference[1];
			output[2] += (input[index] - inputfield.GetMin()) / (inputfield.GetMax() - inputfield.GetMin()) * difference[2];
		}

		return output;
	}

	double DataTransform::LinearInterpolate(const InputField& inputfield, double min, double difference, const std::vector<double>& input, int index) const {

		double output = min;
		if (!input.empty()) {
			output += (input[index] - inputfield.GetMin()) / (inputfield.GetMax() - inputfield.GetMin()) * difference;
		}

		return output;
	}

	void DataTransform::AddChildrenToGlyphTree(GlyphTree::SharedPtr tree, GlyphTree::iterator newNode, MinMaxGlyphTree::SharedPtr minMaxTree, MinMaxGlyphTree::const_iterator node) const {

		for (int i = 0; i < minMaxTree->children(node); ++i) {

			MinMaxGlyphTree::const_iterator child = minMaxTree->child(node, i);
			GlyphTree::iterator newChild = tree->insert(newNode, child->GetMinGlyph());
			AddChildrenToGlyphTree(tree, newChild, minMaxTree, child);
		}
	}

} //namespace SynGlyphX