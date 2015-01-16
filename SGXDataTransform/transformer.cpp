#include "transformer.h"
#include "datatransformmapping.h"
#include "sourcedatamanager.h"
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

	void Transformer::Transform(const DataTransformMapping& mapping) {

		if (mapping.IsTransformable()) {

			//Before running this code, the static function SourceDataManager::SetIntermediateDirectory must be called with a valid
			//directory to store intermediate files
			m_defaults = mapping.GetDefaults();
			m_sourceDataManager.AddDatabaseConnections(mapping.GetDatasources());
			CreateGlyphsFromMapping(mapping);
			m_sourceDataManager.ClearDatabaseConnections();
			m_defaults.Clear();

			if (m_overrideRootXYBoundingBox.IsValid()) {

				m_overrideRootXYBoundingBox = GeographicBoundingBox();
			}
		}
		else {

			throw std::invalid_argument("DataTransformMapping was not transformable.");
		}
	}

	GlyphGraph::ConstSharedVector Transformer::CreateGlyphTreesFromMinMaxTrees(const DataTransformMapping& mapping) const {

		GlyphGraph::ConstSharedVector allTrees;

		for (auto minMaxTree : mapping.GetGlyphGraphs()) {

			GlyphGraph::ConstSharedVector newTrees = CreateGlyphTreesFromMinMaxTree(minMaxTree.second);
			allTrees.insert(allTrees.end(), newTrees.begin(), newTrees.end());
		}

		return allTrees;
	}

	GlyphGraph::ConstSharedVector Transformer::CreateGlyphTreesFromMinMaxTree(DataMappingGlyphGraph::ConstSharedPtr minMaxTree) const {

		GlyphGraph::ConstSharedVector trees;

		DataMappingGlyphGraph::const_iterator minMaxGlyph = minMaxTree->root().constify();

		InputFieldDataMap queryResultData;
		const DataMappingGlyphGraph::InputFieldMap& inputFields = minMaxTree->GetInputFields();
		for (DataMappingGlyphGraph::InputFieldMap::const_iterator iterator = inputFields.begin(); iterator != inputFields.end(); ++iterator) {

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

			GlyphGraph::SharedPtr glyphTree = std::make_shared<GlyphGraph>();

			glyphTree->insert(ProcessMinMaxGlyph(minMaxGlyph, queryResultData, i));

			AddChildrenToGlyphTree(glyphTree, glyphTree->root(), minMaxTree, minMaxGlyph, queryResultData, i);

			trees.push_back(glyphTree);
		}

		return trees;
	}

	Glyph Transformer::ProcessMinMaxGlyph(const DataMappingGlyphGraph::const_iterator& minMaxGlyph, const InputFieldDataMap& queryResultData, unsigned int index) const {

		const DataMappingGlyphGraph* minMaxGlyphTree = static_cast<const DataMappingGlyphGraph*>(minMaxGlyph.owner());

		Glyph difference = minMaxGlyph->GetDifference();
		Glyph glyph = minMaxGlyph->GetMinGlyph();

		Vector3 mappedVector3;

		if ((minMaxGlyphTree->root() == minMaxGlyph) && (m_overrideRootXYBoundingBox.IsValid())){

			InputBinding xBinding(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::PositionX).GetInputFieldID());
			xBinding.SetMinMaxOverride(m_overrideRootXYBoundingBox.GetSWCorner().get<0>(), m_overrideRootXYBoundingBox.GetNECorner().get<0>());
			glyph.GetPosition()[0] = LinearInterpolate(xBinding, glyph.GetPosition()[0], difference.GetPosition()[0], queryResultData, index);

			InputBinding yBinding(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::PositionY).GetInputFieldID());
			yBinding.SetMinMaxOverride(m_overrideRootXYBoundingBox.GetSWCorner().get<1>(), m_overrideRootXYBoundingBox.GetNECorner().get<1>());
			glyph.GetPosition()[1] = LinearInterpolate(yBinding, glyph.GetPosition()[1], difference.GetPosition()[1], queryResultData, index);
		}
		else {

			glyph.GetPosition()[0] = LinearInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::PositionX), glyph.GetPosition()[0], difference.GetPosition()[0], queryResultData, index);
			glyph.GetPosition()[1] = LinearInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::PositionY), glyph.GetPosition()[1], difference.GetPosition()[1], queryResultData, index);
		}

		glyph.GetPosition()[2] = LinearInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::PositionZ), glyph.GetPosition()[2], difference.GetPosition()[2], queryResultData, index);

		glyph.GetRotation()[0] = LinearInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::RotationX), glyph.GetRotation()[0], difference.GetRotation()[0], queryResultData, index);
		glyph.GetRotation()[1] = LinearInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::RotationY), glyph.GetRotation()[1], difference.GetRotation()[1], queryResultData, index);
		glyph.GetRotation()[2] = LinearInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::RotationZ), glyph.GetRotation()[2], difference.GetRotation()[2], queryResultData, index);

		glyph.GetScale()[0] = LinearInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::ScaleX), glyph.GetScale()[0], difference.GetScale()[0], queryResultData, index);
		glyph.GetScale()[1] = LinearInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::ScaleY), glyph.GetScale()[1], difference.GetScale()[1], queryResultData, index);
		glyph.GetScale()[2] = LinearInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::ScaleZ), glyph.GetScale()[2], difference.GetScale()[2], queryResultData, index);

		glyph.GetColor() = ColorRGBInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::Color), glyph.GetColor(), difference.GetColor(), queryResultData, index);
		glyph.GetTransparency() = LinearInterpolate(minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::Transparency), glyph.GetColor()[3], difference.GetColor()[3], queryResultData, index);

		glyph.GetTag() = GenerateTag(minMaxGlyph, queryResultData, index);

		return glyph;
	}

	void Transformer::AddChildrenToGlyphTree(GlyphGraph::SharedPtr tree, GlyphGraph::iterator newNode, DataMappingGlyphGraph::ConstSharedPtr minMaxTree, DataMappingGlyphGraph::const_iterator node, const InputFieldDataMap& queryResultData, unsigned int index) const {

		for (int i = 0; i < minMaxTree->children(node); ++i) {

			DataMappingGlyphGraph::const_iterator child = minMaxTree->child(node, i);
			GlyphGraph::iterator newChild = tree->insert(newNode, ProcessMinMaxGlyph(child, queryResultData, index));
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

	GlyphColor Transformer::ColorRGBInterpolate(const InputBinding& binding, const GlyphColor& min, const GlyphColor& difference, const InputFieldDataMap& queryResultData, unsigned int index) const {

		GlyphColor output = min;

		InputField::HashID id = binding.GetInputFieldID();
		if (id != 0) {

			InputFieldDataMap::const_iterator fieldData = queryResultData.find(id);
			if (fieldData != queryResultData.end()) {

				GlyphColor max;
				max.Set(min[0] + difference[0], min[1] + difference[1], min[2] + difference[2]);

				GlyphColor minHSV = GlyphColor::ConvertRGBtoHSV(min);
				GlyphColor maxHSV = GlyphColor::ConvertRGBtoHSV(max);
				GlyphColor diffHSV;
				diffHSV.Set(maxHSV[0] - minHSV[0], maxHSV[1] - minHSV[1], maxHSV[2] - minHSV[2]);

				double currentData = fieldData->second->GetData()[index].toDouble();
				double dataMin;
				double dataMaxMinDiff;
				GetDataMinAndDifference(binding, *(fieldData->second), dataMin, dataMaxMinDiff);

				GlyphColor outputHSV;
				outputHSV.Set(0, LinearInterpolate(minHSV[0], diffHSV[0], dataMin, dataMaxMinDiff, currentData));
				outputHSV.Set(1, LinearInterpolate(minHSV[1], diffHSV[1], dataMin, dataMaxMinDiff, currentData));
				outputHSV.Set(2, LinearInterpolate(minHSV[2], diffHSV[2], dataMin, dataMaxMinDiff, currentData));

				return GlyphColor::ConvertHSVtoRGB(outputHSV);
			}
		}
		
		output += difference;
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
		
		return min + difference;
	}

	double Transformer::LinearInterpolate(double min, double difference, double dataMin, double dataDifference, double currentData) const {

		if (dataDifference != 0.0) {

			return (min + (((currentData - dataMin) / dataDifference) * difference));
		}
		else {

			return min;
		}
	}

	bool Transformer::HaveDatasourcesBeenUpdated(const DataTransformMapping& mapping, std::time_t lastUpdateTime) const {

		for (auto datasource : mapping.GetDatasources().GetFileDatasources()) {

			if (HasFileBeenUpdated(datasource.second.GetDBName(), lastUpdateTime)) {

				return true;
			}
		}

		return false;
	}

	bool Transformer::HasFileBeenUpdated(const std::wstring& filename, std::time_t lastUpdateTime) const {

		boost::filesystem::path filePath(filename);
		if (boost::filesystem::last_write_time(filePath) > lastUpdateTime) {

			return true;
		}
		else {

			return false;
		}
	}

	void Transformer::GetPositionXYForAllGlyphTrees(const SynGlyphX::DataTransformMapping& mapping, std::vector<GeographicPoint>& points) const {

		for (auto minMaxTree : mapping.GetGlyphGraphs()) {

			DataMappingGlyphGraph::iterator& rootGlyph = minMaxTree.second->root();

			QVariantList queryResultDataX;
			QVariantList queryResultDataY;
			const DataMappingGlyphGraph::InputFieldMap& inputFields = minMaxTree.second->GetInputFields();

			queryResultDataX = m_sourceDataManager.RunSelectSqlQuery(inputFields.at(rootGlyph->GetInputBinding(DataMappingGlyph::MappableField::PositionX).GetInputFieldID()));
			queryResultDataY = m_sourceDataManager.RunSelectSqlQuery(inputFields.at(rootGlyph->GetInputBinding(DataMappingGlyph::MappableField::PositionY).GetInputFieldID()));

			size_t numGlyphs = queryResultDataX.length();
			for (int i = 0; i < numGlyphs; ++i) {
				points.push_back(GeographicPoint(queryResultDataX[i].toDouble(), queryResultDataY[i].toDouble()));
			}
		}
	}

	std::wstring Transformer::GenerateTag(const DataMappingGlyphGraph::const_iterator& minMaxGlyph, const InputFieldDataMap& queryResultData, unsigned int index) const {

		InputField::HashID id = minMaxGlyph->GetInputBinding(DataMappingGlyph::MappableField::Tag).GetInputFieldID();

		if (id == 0) {

			if (m_defaults.IsDefaultTagFieldSet()) {

				id = minMaxGlyph->GetInputBinding(m_defaults.GetTagField()).GetInputFieldID();
			}
		}

		if (id != 0) {

			const DataMappingGlyphGraph* minMaxGlyphTree = static_cast<const DataMappingGlyphGraph*>(minMaxGlyph.owner());
			InputFieldDataMap::const_iterator dataList = queryResultData.find(id);
			if (dataList != queryResultData.end()) {

				std::wstring tag = minMaxGlyphTree->GetInputFields().find(id)->second.GetField() + L": " + dataList->second->GetData()[index].toString().toStdWString();
				return tag;
			}
		}

		return m_defaults.GetDefaultTagValue();
	}

} //namespace SynGlyphX