#include "transformer.h"
#include "datatransformmapping.h"
#include "databaseservices.h"
#include <QtCore/QVariant>
#include <stdexcept>
#include <boost/filesystem.hpp>

namespace SynGlyphX {

	Transformer::Transformer()
	{
	}


	Transformer::~Transformer()
	{
	}

	void Transformer::Transform(DataTransformMapping& mapping) {

		if (mapping.IsTransformable()) {

			CreateGlyphsFromMapping(mapping);
		}
		else {

			throw std::invalid_argument("DataTransformMapping was not transformable.");
		}
	}

	GlyphTree::ConstSharedVector Transformer::CreateGlyphTreesFromMinMaxTrees(const DataTransformMapping& mapping) const {

		SynGlyphX::GlyphTree::ConstSharedVector allTrees;

		for (auto minMaxTree : mapping.GetGlyphTrees()) {

			SynGlyphX::GlyphTree::ConstSharedVector newTrees = CreateGlyphTreesFromMinMaxTree(minMaxTree.second);
			allTrees.insert(allTrees.end(), newTrees.begin(), newTrees.end());
		}

		return allTrees;
	}

	GlyphTree::ConstSharedVector Transformer::CreateGlyphTreesFromMinMaxTree(MinMaxGlyphTree::ConstSharedPtr minMaxTree) const {

		SynGlyphX::GlyphTree::ConstSharedVector trees;

		SynGlyphX::MinMaxGlyphTree::const_iterator minMaxGlyph = minMaxTree->root().constify();

		std::unordered_map<SynGlyphX::InputField::HashID, QVariantList> queryResultData;
		const SynGlyphX::MinMaxGlyphTree::InputFieldMap& inputFields = minMaxTree->GetInputFields();
		for (SynGlyphX::MinMaxGlyphTree::InputFieldMap::const_iterator iterator = inputFields.begin(); iterator != inputFields.end(); ++iterator) {

			queryResultData[iterator->first] = SynGlyphX::DatabaseServices::RunSqlQuery(iterator->second);
		}

		size_t numGlyphs = queryResultData.begin()->second.length();

		for (unsigned int i = 0; i < numGlyphs; ++i) {

			SynGlyphX::GlyphTree::SharedPtr glyphTree(new SynGlyphX::GlyphTree());

			glyphTree->insert(ProcessMinMaxGlyph(minMaxGlyph, queryResultData, i));

			AddChildrenToGlyphTree(glyphTree, glyphTree->root(), minMaxTree, minMaxGlyph, queryResultData, i);

			trees.push_back(glyphTree);
		}

		return trees;
	}

	GlyphProperties Transformer::ProcessMinMaxGlyph(const MinMaxGlyphTree::const_iterator& minMaxGlyph, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const {

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
		color.Set(3, LinearInterpolate(minMaxGlyph->GetInputBinding(10), glyph.GetColor()[3], difference.GetColor()[3], queryResultData, index));
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

	void Transformer::AddChildrenToGlyphTree(GlyphTree::SharedPtr tree, GlyphTree::iterator newNode, MinMaxGlyphTree::ConstSharedPtr minMaxTree, MinMaxGlyphTree::const_iterator node, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const {

		for (int i = 0; i < minMaxTree->children(node); ++i) {

			MinMaxGlyphTree::const_iterator child = minMaxTree->child(node, i);
			GlyphTree::iterator newChild = tree->insert(newNode, ProcessMinMaxGlyph(child, queryResultData, index));
			AddChildrenToGlyphTree(tree, newChild, minMaxTree, child, queryResultData, index);
		}
	}

	Color Transformer::ColorRGBInterpolate(const InputBinding& binding, const Color& min, const Color& difference, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const {

		Color output = min;

		InputField::HashID id = binding.GetInputFieldID();
		if (id != 0) {

			std::unordered_map<InputField::HashID, QVariantList>::const_iterator dataList = queryResultData.find(id);
			if (dataList != queryResultData.end()) {

				output.Set(0, (dataList->second[index].toDouble() - binding.GetMin()) / (binding.GetMax() - binding.GetMin()) * difference[0]);
				output.Set(1, (dataList->second[index].toDouble() - binding.GetMin()) / (binding.GetMax() - binding.GetMin()) * difference[1]);
				output.Set(2, (dataList->second[index].toDouble() - binding.GetMin()) / (binding.GetMax() - binding.GetMin()) * difference[2]);
			}
		}

		return output;
	}

	double Transformer::LinearInterpolate(const InputBinding& binding, double min, double difference, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const {

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

	bool Transformer::HaveDatasourcesBeenUpdated(const DataTransformMapping& mapping, std::time_t lastUpdateTime) const {

		for (auto datasource : mapping.GetDatasources()) {

			//For now our only datasources are files.  We will need a better way to handle this for non-file datasources
			if (datasource.second.IsFile()) {

				boost::filesystem::path datasourcePath(datasource.second.GetDBName());
				if (boost::filesystem::last_write_time(datasourcePath) > lastUpdateTime) {
					
					return true;
				}
			}
		}

		return false;
	}

} //namespace SynGlyphX