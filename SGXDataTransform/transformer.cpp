#include "transformer.h"
#include "datatransformmapping.h"
#include "sourcedatamanager.h"
#include <QtCore/QVariant>
#include <QtCore/QFile>
#include <stdexcept>
#include <boost/filesystem.hpp>

namespace SynGlyphX {

	Transformer::Transformer()
	{
	}


	Transformer::~Transformer()
	{
	}

	void Transformer::Transform(const DataTransformMapping& mapping) {

		if (mapping.IsTransformable()) {

			//Before running this code, the static function SourceDataManager::SetIntermediateDirectory must be called with a valid
			//directory to store intermediate files
			m_sourceDataManager.AddDatabaseConnections(mapping.GetDatasources());
			CreateGlyphsFromMapping(mapping);
			m_sourceDataManager.ClearDatabaseConnections();

			if (m_overrideRootXYBoundingBox.IsValid()) {

				m_overrideRootXYBoundingBox = GeographicBoundingBox();
			}
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

		InputFieldDataMap queryResultData;
		const SynGlyphX::MinMaxGlyphTree::InputFieldMap& inputFields = minMaxTree->GetInputFields();
		for (SynGlyphX::MinMaxGlyphTree::InputFieldMap::const_iterator iterator = inputFields.begin(); iterator != inputFields.end(); ++iterator) {

			InputFieldData::SharedPtr fieldData;

			if (iterator->second.IsNumeric()) {

				QVariantList minAndMax = m_sourceDataManager.GetMinMaxSqlQuery(iterator->second);
				fieldData.reset(new InputFieldData(m_sourceDataManager.RunSelectSqlQuery(iterator->second), minAndMax.value(0).toDouble(), minAndMax.value(1).toDouble()));
			}
			else {

				fieldData.reset(new InputFieldData(m_sourceDataManager.RunSelectSqlQuery(iterator->second)));
			}

			queryResultData[iterator->first] = fieldData;
		}

		size_t numGlyphs = queryResultData.begin()->second->GetData().length();

		for (unsigned int i = 0; i < numGlyphs; ++i) {

			SynGlyphX::GlyphTree::SharedPtr glyphTree(new SynGlyphX::GlyphTree());

			glyphTree->insert(ProcessMinMaxGlyph(minMaxGlyph, queryResultData, i));

			AddChildrenToGlyphTree(glyphTree, glyphTree->root(), minMaxTree, minMaxGlyph, queryResultData, i);

			trees.push_back(glyphTree);
		}

		return trees;
	}

	GlyphProperties Transformer::ProcessMinMaxGlyph(const MinMaxGlyphTree::const_iterator& minMaxGlyph, const InputFieldDataMap& queryResultData, unsigned int index) const {

		const MinMaxGlyphTree* minMaxGlyphTree = static_cast<const MinMaxGlyphTree*>(minMaxGlyph.owner());

		const GlyphMappableProperties& difference = minMaxGlyph->GetDifference();
		GlyphProperties glyph = minMaxGlyph->GetMinGlyph();

		Vector3 mappedVector3;

		if ((minMaxGlyphTree->root() == minMaxGlyph) && (m_overrideRootXYBoundingBox.IsValid())){

			InputBinding xBinding(minMaxGlyph->GetInputBinding(0).GetInputFieldID());
			xBinding.SetMinMaxOverride(m_overrideRootXYBoundingBox.GetSWCorner().get<0>(), m_overrideRootXYBoundingBox.GetNECorner().get<0>());
			mappedVector3[0] = LinearInterpolate(xBinding, glyph.GetPosition()[0], difference.GetPosition()[0], queryResultData, index);

			InputBinding yBinding(minMaxGlyph->GetInputBinding(1).GetInputFieldID());
			yBinding.SetMinMaxOverride(m_overrideRootXYBoundingBox.GetSWCorner().get<1>(), m_overrideRootXYBoundingBox.GetNECorner().get<1>());
			mappedVector3[1] = LinearInterpolate(yBinding, glyph.GetPosition()[1], difference.GetPosition()[1], queryResultData, index);
		}
		else {

			mappedVector3[0] = LinearInterpolate(minMaxGlyph->GetInputBinding(0), glyph.GetPosition()[0], difference.GetPosition()[0], queryResultData, index);
			mappedVector3[1] = LinearInterpolate(minMaxGlyph->GetInputBinding(1), glyph.GetPosition()[1], difference.GetPosition()[1], queryResultData, index);
		}

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

			InputFieldDataMap::const_iterator dataList = queryResultData.find(id);
			if (dataList != queryResultData.end()) {

				glyph.SetTag(dataList->second->GetData()[index].toString().toStdWString());
			}
		}

		return glyph;
	}

	void Transformer::AddChildrenToGlyphTree(GlyphTree::SharedPtr tree, GlyphTree::iterator newNode, MinMaxGlyphTree::ConstSharedPtr minMaxTree, MinMaxGlyphTree::const_iterator node, const InputFieldDataMap& queryResultData, unsigned int index) const {

		for (int i = 0; i < minMaxTree->children(node); ++i) {

			MinMaxGlyphTree::const_iterator child = minMaxTree->child(node, i);
			GlyphTree::iterator newChild = tree->insert(newNode, ProcessMinMaxGlyph(child, queryResultData, index));
			AddChildrenToGlyphTree(tree, newChild, minMaxTree, child, queryResultData, index);
		}
	}

	void Transformer::GetDataMinAndDifference(const InputBinding& binding, const InputFieldData& fieldData, double& dataMin, double& dataDifference) const {

		if (binding.IsMinMaxOverrideInUse()) {

			dataMin = binding.GetMinOverride();
			dataDifference = binding.GetMaxMinOverrideDifference();
		}
		else {

			dataMin = fieldData.GetMin();
			dataDifference = fieldData.GetMaxMinDifference();
		}
	}

	Color Transformer::ColorRGBInterpolate(const InputBinding& binding, const Color& min, const Color& difference, const InputFieldDataMap& queryResultData, unsigned int index) const {

		Color output = min;

		InputField::HashID id = binding.GetInputFieldID();
		if (id != 0) {

			InputFieldDataMap::const_iterator fieldData = queryResultData.find(id);
			if (fieldData != queryResultData.end()) {

				/*output.Set(0, (fieldData->second->GetData()[index].toDouble() - binding.GetMin()) / (binding.GetMax() - binding.GetMin()) * difference[0]);
				output.Set(1, (fieldData->second->GetData()[index].toDouble() - binding.GetMin()) / (binding.GetMax() - binding.GetMin()) * difference[1]);
				output.Set(2, (fieldData->second->GetData()[index].toDouble() - binding.GetMin()) / (binding.GetMax() - binding.GetMin()) * difference[2]);*/

				double currentData = fieldData->second->GetData()[index].toDouble();
				double dataMin;
				double dataMaxMinDiff;
				GetDataMinAndDifference(binding, *(fieldData->second), dataMin, dataMaxMinDiff);

				output.Set(0, LinearInterpolate(min[0], difference[0], dataMin, dataMaxMinDiff, currentData));
				output.Set(1, LinearInterpolate(min[1], difference[1], dataMin, dataMaxMinDiff, currentData));
				output.Set(2, LinearInterpolate(min[2], difference[2], dataMin, dataMaxMinDiff, currentData));
			}
		}

		return output;
	}

	double Transformer::LinearInterpolate(const InputBinding& binding, double min, double difference, const InputFieldDataMap& queryResultData, unsigned int index) const {

		InputField::HashID id = binding.GetInputFieldID();
		if (id != 0) {

			InputFieldDataMap::const_iterator fieldData = queryResultData.find(id);
			if (fieldData != queryResultData.end()) {

				double dataMin;
				double dataMaxMinDiff;
				GetDataMinAndDifference(binding, *(fieldData->second), dataMin, dataMaxMinDiff);

				//output += ((fieldData->second->GetData()[index].toDouble() - dataMin) / dataMaxMinDiff) * difference;
				return LinearInterpolate(min, difference, dataMin, dataMaxMinDiff, fieldData->second->GetData()[index].toDouble());
			}
		}
		
		return min;
	}

	double Transformer::LinearInterpolate(double min, double difference, double dataMin, double dataDifference, double currentData) const {

		return (min + (((currentData - dataMin) / dataDifference) * difference));
	}

	bool Transformer::HaveDatasourcesBeenUpdated(const DataTransformMapping& mapping, std::time_t lastUpdateTime) const {

		for (auto datasource : mapping.GetDatasources().GetFileDatasources()) {

			boost::filesystem::path datasourcePath(datasource.second.GetDBName());
			if (boost::filesystem::last_write_time(datasourcePath) > lastUpdateTime) {
					
				return true;
			}
		}

		return false;
	}

	void Transformer::GetPositionXYForAllGlyphTrees(const SynGlyphX::DataTransformMapping& mapping, std::vector<GeographicPoint>& points) const {

		for (auto minMaxTree : mapping.GetGlyphTrees()) {

			MinMaxGlyphTree::iterator& rootGlyph = minMaxTree.second->root();

			QVariantList queryResultDataX;
			QVariantList queryResultDataY;
			const MinMaxGlyphTree::InputFieldMap& inputFields = minMaxTree.second->GetInputFields();

			queryResultDataX = m_sourceDataManager.RunSelectSqlQuery(inputFields.at(rootGlyph->GetInputBinding(0).GetInputFieldID()));
			queryResultDataY = m_sourceDataManager.RunSelectSqlQuery(inputFields.at(rootGlyph->GetInputBinding(1).GetInputFieldID()));

			size_t numGlyphs = queryResultDataX.length();
			for (int i = 0; i < numGlyphs; ++i) {
				points.push_back(GeographicPoint(queryResultDataX[i].toDouble(), queryResultDataY[i].toDouble()));
			}
		}
	}

} //namespace SynGlyphX