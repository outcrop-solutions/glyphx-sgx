#include "datatransform.h"
#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
#include <boost/uuid/uuid_io.hpp>
#include "csvreaderwriter.h"
#include <QtCore/QVariant>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

namespace SynGlyphX {

    DataTransform::DataTransform() :
		m_baseImage(nullptr)
    {
    }

    DataTransform::~DataTransform()
    {
    }

    void DataTransform::ReadFromFile(const std::string& filename) {

        Clear();

        boost::property_tree::wptree dataTransformPropertyTree;
		boost::property_tree::read_xml(filename, dataTransformPropertyTree);

		//boost::property_tree::wptree& baseMapPropertyTree = dataTransformPropertyTree.get_child(L"BaseImage");
		//m_baseImageType = baseMapPropertyTree.get<BaseImageType>(L"<xmlattr>.type");


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

	void DataTransform::SetBaseImage(const BaseImage& baseImage) {

		m_baseImage = baseImage;
	}

	const BaseImage& DataTransform::GetBaseImage() const {

		return m_baseImage;
	}

	QVariantList DataTransform::RunSqlQuery(const InputField& inputfield) const {

		QVariantList results;
		QSqlDatabase db= QSqlDatabase::database(QString::fromStdWString(boost::uuids::to_wstring(inputfield.GetDatasourceID())));
		QSqlQuery query(db);
		query.prepare(QString("SELECT %1 FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + QString::fromStdWString(inputfield.GetTable()));
		query.exec();
		while (query.next()) {
			results.push_back(query.value(0));
		}

		return results;
	}

	void DataTransform::TransformToCSV(const std::string& filename, const std::string& tagFilename) const {

		GlyphTree::ConstSharedVector trees;

		for (auto minMaxTree : m_glyphTrees) {

			MinMaxGlyphTree::const_iterator minMaxGlyph = minMaxTree.second->root().constify();

			std::unordered_map<InputField::HashID, QVariantList> queryResultData;
			const MinMaxGlyphTree::InputFieldMap& inputFields = minMaxTree.second->GetInputFields();
			for (MinMaxGlyphTree::InputFieldMap::const_iterator iterator = inputFields.begin(); iterator != inputFields.end(); ++iterator) {

				queryResultData[iterator->first] = RunSqlQuery(iterator->second);
			}

			size_t numGlyphs = queryResultData.begin()->second.length();

			for (unsigned int i = 0; i < numGlyphs; ++i) {

				GlyphTree::SharedPtr glyphTree(new GlyphTree());
				
				glyphTree->insert(ProcessMinMaxGlyph(minMaxGlyph, queryResultData, i));

				AddChildrenToGlyphTree(glyphTree, glyphTree->root(), minMaxTree.second, minMaxGlyph, queryResultData, i);

				trees.push_back(glyphTree);
			}
		}

		CSVReaderWriter& writer = CSVReaderWriter::GetInstance();
		writer.Write(filename, tagFilename, trees);
	}

	GlyphProperties DataTransform::ProcessMinMaxGlyph(const MinMaxGlyphTree::const_iterator& minMaxGlyph, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const {

		const MinMaxGlyphTree* minMaxGlyphTree = static_cast<const MinMaxGlyphTree*>(minMaxGlyph.owner());

		const GlyphMappableProperties& difference = minMaxGlyph->GetDifference();
		GlyphProperties glyph = minMaxGlyph->GetMinGlyph();

		Vector3 mappedVector3;

		mappedVector3[0] = LinearInterpolate(minMaxGlyph->GetInputBinding(0), glyph.GetPosition()[0], difference.GetPosition()[0], queryResultData, index);
		mappedVector3[1] = LinearInterpolate(minMaxGlyph->GetInputBinding(1), glyph.GetPosition()[1], difference.GetPosition()[1], queryResultData, index);
		mappedVector3[2] = LinearInterpolate(minMaxGlyph->GetInputBinding(2), glyph.GetPosition()[2], difference.GetPosition()[2], queryResultData, index);
		glyph.SetPosition(mappedVector3);

		mappedVector3[0] = LinearInterpolate(minMaxGlyph->GetInputBinding(3), glyph.GetRotation()[0], difference.GetRotation()[0], queryResultData, index);
		mappedVector3[1] = LinearInterpolate(minMaxGlyph->GetInputBinding(4), glyph.GetRotation()[1], difference.GetRotation()[1], queryResultData, index);
		mappedVector3[2] = LinearInterpolate(minMaxGlyph->GetInputBinding(5), glyph.GetRotation()[2], difference.GetRotation()[2], queryResultData, index);
		glyph.SetRotation(mappedVector3);

		mappedVector3[0] = LinearInterpolate(minMaxGlyph->GetInputBinding(6), glyph.GetScale()[0], difference.GetScale()[0], queryResultData, index);
		mappedVector3[1] = LinearInterpolate(minMaxGlyph->GetInputBinding(7), glyph.GetScale()[1], difference.GetScale()[1], queryResultData, index);
		mappedVector3[2] = LinearInterpolate(minMaxGlyph->GetInputBinding(8), glyph.GetScale()[2], difference.GetScale()[2], queryResultData, index);
		glyph.SetScale(mappedVector3);

		Color color = ColorRGBInterpolate(minMaxGlyph->GetInputBinding(9), glyph.GetColor(), difference.GetColor(), queryResultData, index);
		color[3] = LinearInterpolate(minMaxGlyph->GetInputBinding(10), glyph.GetColor()[3], difference.GetColor()[3], queryResultData, index);
		glyph.SetColor(color);

		//For now since we aren't mapping torus ratio just set it to 0.1
		glyph.SetRatio(0.1);
		//glyph.SetRatio(LinearInterpolate(minMaxGlyph->GetInputBinding(11), glyph.GetRatio(), difference.GetRatio(), queryResultData, index));

		InputField::HashID id = minMaxGlyph->GetInputBinding(12).GetInputFieldID();
		if (id != 0) {

			std::unordered_map<InputField::HashID, QVariantList>::const_iterator dataList = queryResultData.find(id);
			if (dataList != queryResultData.end()) {

				glyph.SetTag(dataList->second[index].toString().toStdWString());
			}
		}

		return glyph;
	}

	Color DataTransform::ColorRGBInterpolate(const InputBinding& binding, const Color& min, const Color& difference, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const {

		Color output = min;

		InputField::HashID id = binding.GetInputFieldID();
		if (id != 0) {

			std::unordered_map<InputField::HashID, QVariantList>::const_iterator dataList = queryResultData.find(id);
			if (dataList != queryResultData.end()) {

				output[0] += (dataList->second[index].toDouble() - binding.GetMin()) / (binding.GetMax() - binding.GetMin()) * difference[0];
				output[1] += (dataList->second[index].toDouble() - binding.GetMin()) / (binding.GetMax() - binding.GetMin()) * difference[1];
				output[2] += (dataList->second[index].toDouble() - binding.GetMin()) / (binding.GetMax() - binding.GetMin()) * difference[2];
			}
		}

		return output;
	}

	double DataTransform::LinearInterpolate(const InputBinding& binding, double min, double difference, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const {

		double output = min;

		InputField::HashID id = binding.GetInputFieldID();
		if (id != 0) {

			std::unordered_map<InputField::HashID, QVariantList>::const_iterator dataList = queryResultData.find(id);
			if (dataList != queryResultData.end()) {

				output += (dataList->second[index].toDouble() - binding.GetMin()) / (binding.GetMax() - binding.GetMin()) * difference;
			}
		}

		return output;
	}

	void DataTransform::AddChildrenToGlyphTree(GlyphTree::SharedPtr tree, GlyphTree::iterator newNode, MinMaxGlyphTree::SharedPtr minMaxTree, MinMaxGlyphTree::const_iterator node, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const {

		for (int i = 0; i < minMaxTree->children(node); ++i) {

			MinMaxGlyphTree::const_iterator child = minMaxTree->child(node, i);
			GlyphTree::iterator newChild = tree->insert(newNode, ProcessMinMaxGlyph(child, queryResultData, index));
			AddChildrenToGlyphTree(tree, newChild, minMaxTree, child, queryResultData, index);
		}
	}
	
	void DataTransform::SetInputField(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index, const InputField& inputfield) {

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
	}

	void DataTransform::ClearInputBinding(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index) {

		MinMaxGlyphTree::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->ClearInputBinding(node, index);
	}

} //namespace SynGlyphX