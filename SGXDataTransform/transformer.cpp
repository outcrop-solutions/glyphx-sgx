#include "transformer.h"
#include "datatransformmapping.h"
#include <QtCore/QVariant>
#include <QtCore/QDir>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include "interpolationmappingfunction.h"
#include "valuemappingfunction.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"
#include "networkdownloader.h"
#include "downloadexception.h"

namespace SynGlyphX {

	QString Transformer::s_defaultImagesDirectory;

	Transformer::Transformer() :
		m_sourceDataCacheLocation(QDir::tempPath() + QDir::separator() + "sourcedatacache.db")
	{
	}

	Transformer::~Transformer()
	{
	}

	void Transformer::Transform(const DataTransformMapping& mapping) {

		m_error = QString::null;
		if (mapping.IsTransformable()) {

			Prepare();
			m_defaults = mapping.GetDefaults();
			m_sourceDataCache.Setup(m_sourceDataCacheLocation);
			m_sourceDataCache.AddDatasourcesToCache(mapping.GetDatasourcesInUse());
			GetMinMaxForFieldGroups(mapping);
			CreateGlyphsFromMapping(mapping);
			
			ClearForNextTransformation();
		}
		else {

			throw std::invalid_argument("DataTransformMapping was not transformable.");
		}
	}

	void Transformer::ClearForNextTransformation() {

		m_fieldGroupsMinAndMax.clear();
		m_sourceDataCache.Close();
		m_defaults.Clear();

		if (m_overrideRootXYBoundingBox.IsValid()) {

			m_overrideRootXYBoundingBox = GeographicBoundingBox();
		}
	}

	void Transformer::SetSourceDataCacheLocation(const QString& sourceDataCacheLocation) {

		m_sourceDataCacheLocation = sourceDataCacheLocation;
	}

	GlyphGraph::ConstSharedVector Transformer::CreateGlyphTreesFromMinMaxTrees(const DataTransformMapping& mapping) {

		GlyphGraph::ConstSharedVector allTrees;

		for (auto minMaxTree : mapping.GetGlyphGraphs()) {

			if (minMaxTree.second->IsTransformable()) {

				m_label2LabelMap.clear();
				GlyphGraph::ConstSharedVector newTrees = CreateGlyphTreesFromMinMaxTree(minMaxTree.second);
				allTrees.insert(allTrees.end(), newTrees.begin(), newTrees.end());
			}
		}

		return allTrees;
	}

	void Transformer::RunSelectSqlQuery(const InputField& inputfield, QVariantList& data) const {

		SharedSQLQuery query = m_sourceDataCache.CreateSelectFieldQueryAscending(inputfield);
		query->exec();
		while (query->next()) {
			
			data.push_back(query->value(0));
		}
		query->finish();
	}

	GlyphGraph::ConstSharedVector Transformer::CreateGlyphTreesFromMinMaxTree(DataMappingGlyphGraph::ConstSharedPtr minMaxTree) {

		GlyphGraph::ConstSharedVector trees;

		DataMappingGlyphGraph::ConstGlyphIterator minMaxGlyph = minMaxTree->GetRoot().constify();

		InputFieldDataMap queryResultData;
		const DataMappingGlyphGraph::InputFieldMap& inputFields = minMaxTree->GetInputFields();
		for (DataMappingGlyphGraph::InputFieldMap::const_iterator iterator = inputFields.begin(); iterator != inputFields.end(); ++iterator) {

			QVariantList data;
			InputFieldData::SharedPtr fieldData;

			if (iterator->second.IsNumeric()) {

				SharedSQLQuery minAndMaxQuery = m_sourceDataCache.CreateMinMaxQuery(iterator->second);
				minAndMaxQuery->exec();
				minAndMaxQuery->first();
				
				RunSelectSqlQuery(iterator->second, data);
				fieldData.reset(new InputFieldData(data, minAndMaxQuery->value(0).toDouble(), minAndMaxQuery->value(1).toDouble()));
				minAndMaxQuery->finish();
			}
			else {

				RunSelectSqlQuery(iterator->second, data);
				fieldData.reset(new InputFieldData(data));
			}

			queryResultData[iterator->first] = fieldData;
		}

		size_t numGlyphs = queryResultData.begin()->second->GetData().length();

		for (unsigned int i = 0; i < numGlyphs; ++i) {

			GlyphGraph::SharedPtr glyphTree = std::make_shared<GlyphGraph>();

			GlyphGraph::GlyphIterator newRoot = glyphTree->SetRootGlyph(ProcessMinMaxGlyph(minMaxGlyph, minMaxTree, queryResultData, i));
			m_label2LabelMap[minMaxGlyph->first] = newRoot->first;

			AddChildrenToGlyphTree(glyphTree, glyphTree->GetRoot(), minMaxTree, minMaxGlyph, queryResultData, i);

			for (auto& link : minMaxTree->GetLinks()) {

				glyphTree->AddLink(m_label2LabelMap[link.first.first], m_label2LabelMap[link.first.second], link.second.GetMaxGlyph());
			}

			trees.push_back(glyphTree);
		}

		return trees;
	}

	Glyph Transformer::ProcessMinMaxGlyph(const DataMappingGlyphGraph::ConstGlyphIterator& minMaxGlyph, DataMappingGlyphGraph::ConstSharedPtr minMaxTree, const InputFieldDataMap& queryResultData, unsigned int index) const {

		Glyph glyph;

		//Copy all non mappable fields first
		glyph.GetStructure() = minMaxGlyph->second.GetStructure().ExportGlyphGeometry();
		const GeometryShapeMappingProperty& shapeMappingProperty = minMaxGlyph->second.GetStructure().GetGeometryShape();
		glyph.GetStructure().SetGeometryShape(TransformProperty(shapeMappingProperty.GetBinding(), shapeMappingProperty, queryResultData, index));

		//Copy all non mappable fields first
		glyph.GetVirtualTopology() = minMaxGlyph->second.GetVirtualTopology().ExportVirtualTopology();
		const VirtualTopologyMappingProperty& topologyTypeMappingProperty = minMaxGlyph->second.GetVirtualTopology().GetType();
		glyph.GetVirtualTopology().SetType(TransformProperty(topologyTypeMappingProperty.GetBinding(), topologyTypeMappingProperty, queryResultData, index));

		Vector3 mappedVector3;

		if ((minMaxTree->GetRoot() == minMaxGlyph) && (m_overrideRootXYBoundingBox.IsValid())) {

			InputBinding xBinding(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::PositionX).GetInputFieldID());
			xBinding.SetMinMaxOverride(m_overrideRootXYBoundingBox.GetSWCorner().get<0>(), m_overrideRootXYBoundingBox.GetNECorner().get<0>());
			glyph.GetPosition()[0] = TransformProperty(xBinding, minMaxGlyph->second.GetPosition()[0], queryResultData, index);

			InputBinding yBinding(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::PositionY).GetInputFieldID());
			yBinding.SetMinMaxOverride(m_overrideRootXYBoundingBox.GetSWCorner().get<1>(), m_overrideRootXYBoundingBox.GetNECorner().get<1>());
			glyph.GetPosition()[1] = TransformProperty(yBinding, minMaxGlyph->second.GetPosition()[1], queryResultData, index);
		}
		else {

			glyph.GetPosition()[0] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::PositionX), minMaxGlyph->second.GetPosition()[0], queryResultData, index);
			glyph.GetPosition()[1] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::PositionY), minMaxGlyph->second.GetPosition()[1], queryResultData, index);
		}

		glyph.GetPosition()[2] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::PositionZ), minMaxGlyph->second.GetPosition()[2], queryResultData, index);

		glyph.GetRotation()[0] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::RotationX), minMaxGlyph->second.GetRotation()[0], queryResultData, index);
		glyph.GetRotation()[1] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::RotationY), minMaxGlyph->second.GetRotation()[1], queryResultData, index);
		glyph.GetRotation()[2] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::RotationZ), minMaxGlyph->second.GetRotation()[2], queryResultData, index);

		glyph.GetScale()[0] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::ScaleX), minMaxGlyph->second.GetScale()[0], queryResultData, index);
		glyph.GetScale()[1] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::ScaleY), minMaxGlyph->second.GetScale()[1], queryResultData, index);
		glyph.GetScale()[2] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::ScaleZ), minMaxGlyph->second.GetScale()[2], queryResultData, index);

		glyph.GetColor() = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::Color), minMaxGlyph->second.GetColor(), queryResultData, index);
		glyph.GetTransparency() = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::Transparency), minMaxGlyph->second.GetTransparency(), queryResultData, index);

		glyph.GetTag() = GenerateTag(minMaxGlyph, minMaxTree, queryResultData, index);

		glyph.GetRotationRate()[0] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::RotationRateX), minMaxGlyph->second.GetRotationRate()[0], queryResultData, index);
		glyph.GetRotationRate()[1] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::RotationRateY), minMaxGlyph->second.GetRotationRate()[1], queryResultData, index);
		glyph.GetRotationRate()[2] = TransformProperty(minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::RotationRateZ), minMaxGlyph->second.GetRotationRate()[2], queryResultData, index);

		glyph.GetURL() = GenerateURL(minMaxGlyph, queryResultData, index);

		return glyph;
	}

	void Transformer::AddChildrenToGlyphTree(GlyphGraph::SharedPtr tree, GlyphGraph::GlyphIterator newNode, DataMappingGlyphGraph::ConstSharedPtr minMaxTree, DataMappingGlyphGraph::ConstGlyphIterator node, const InputFieldDataMap& queryResultData, unsigned int index) {

		for (int i = 0; i < minMaxTree->ChildCount(node); ++i) {

			DataMappingGlyphGraph::ConstGlyphIterator child = minMaxTree->GetChild(node, i);
			GlyphGraph::GlyphIterator newChild = tree->AddChildGlyph(newNode, ProcessMinMaxGlyph(child, minMaxTree, queryResultData, index));
			m_label2LabelMap[child->first] = newChild->first;
			AddChildrenToGlyphTree(tree, newChild, minMaxTree, child, queryResultData, index);
		}
	}

	DoubleMinDiff Transformer::GetDataMinAndDifference(const InputBinding& binding, InterpolationMappingData::ConstSharedPtr mappingData, const InputFieldData& fieldData) const {

		if (binding.IsMinMaxOverrideInUse()) {

			return DoubleMinDiff(binding.GetMinOverride(), binding.GetMaxMinOverrideDifference());
		}
		else if (mappingData->GetInputMinMaxType() == InterpolationMappingData::InputMinMaxType::UserSpecified) {

			return DoubleMinDiff(mappingData->GetUserSpecifiedInputMinMax());
		}
		else if (mappingData->GetInputMinMaxType() == InterpolationMappingData::InputMinMaxType::InputFieldGroup) {

			return m_fieldGroupsMinAndMax.at(mappingData->GetInputMinMaxFieldGroup());
		}
		else {

			return DoubleMinDiff(fieldData.GetMin(), fieldData.GetMaxMinDifference());
		}
	}

	GlyphColor Transformer::TransformProperty(const InputBinding& binding, const ColorMappingProperty& mappingProperty, const InputFieldDataMap& queryResultData, unsigned int index) const {

		InputField::HashID id = binding.GetInputFieldID();
		if (id != 0) {

			InputFieldDataMap::const_iterator fieldData = queryResultData.find(id);
			if (fieldData != queryResultData.end()) {

				const QVariant& input = fieldData->second->GetData()[index];
				MappingFunctionData::Function function = mappingProperty.GetMappingFunctionData()->GetFunction();
				if ((function == MappingFunctionData::Function::LinearInterpolation) || (function == MappingFunctionData::Function::LogarithmicInterpolation)) {

					InterpolationMappingData::ConstSharedPtr interpolationData = std::dynamic_pointer_cast<const InterpolationMappingData>(mappingProperty.GetMappingFunctionData());
					DoubleMinDiff minDiff = GetDataMinAndDifference(binding, interpolationData, *(fieldData->second));
					return interpolationData->Interpolate(mappingProperty.GetValue(), minDiff.GetMin(), minDiff.GetMax(), ValidateInputForMinAndMax(input.toDouble(), minDiff));
				}
				else if (function == MappingFunctionData::Function::Numeric2Value) {

					Numeric2ColorMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Numeric2ColorMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(input.toDouble());
				}
				else if (function == MappingFunctionData::Function::Text2Value) {

					Text2ColorMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Text2ColorMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(input.toString().toStdWString());
				}
				else if (function == MappingFunctionData::Function::Range2Value) {

					Range2ColorMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Range2ColorMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(input.toDouble());
				}
			}
		}
		
		//Return the max value as the default if no transform takes place
		return mappingProperty.GetValue().GetMax();
	}

	double Transformer::TransformProperty(const InputBinding& binding, const NumericMappingProperty& mappingProperty, const InputFieldDataMap& queryResultData, unsigned int index) const {

		InputField::HashID id = binding.GetInputFieldID();
		if (id != 0) {

			InputFieldDataMap::const_iterator fieldData = queryResultData.find(id);
			if (fieldData != queryResultData.end()) {

				const QVariant& input = fieldData->second->GetData()[index];
				MappingFunctionData::Function function = mappingProperty.GetMappingFunctionData()->GetFunction();
				if ((function == MappingFunctionData::Function::LinearInterpolation) || (function == MappingFunctionData::Function::LogarithmicInterpolation)) {

					InterpolationMappingData::ConstSharedPtr interpolationData = std::dynamic_pointer_cast<const InterpolationMappingData>(mappingProperty.GetMappingFunctionData());
					DoubleMinDiff minDiff = GetDataMinAndDifference(binding, interpolationData, *(fieldData->second));
					return interpolationData->Interpolate(mappingProperty.GetValue(), minDiff.GetMin(), minDiff.GetMax(), ValidateInputForMinAndMax(input.toDouble(), minDiff));
				}
				else if (function == MappingFunctionData::Function::Numeric2Value) {

					Numeric2NumericMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Numeric2NumericMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(input.toDouble());
				}
				else if (function == MappingFunctionData::Function::Text2Value) {

					Text2NumericMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Text2NumericMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(input.toString().toStdWString());
				}
				else if (function == MappingFunctionData::Function::Range2Value) {

					Range2NumericMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Range2NumericMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(input.toDouble());
				}
			}
		}
		
		//Return the max value as the default if no transform takes place
		return mappingProperty.GetValue().GetMax();
	}

	GlyphGeometryInfo::Shape Transformer::TransformProperty(const InputBinding& binding, const GeometryShapeMappingProperty& mappingProperty, const InputFieldDataMap& queryResultData, unsigned int index) const {

		InputField::HashID id = binding.GetInputFieldID();
		if (id != 0) {

			InputFieldDataMap::const_iterator fieldData = queryResultData.find(id);
			if (fieldData != queryResultData.end()) {

				MappingFunctionData::Function function = mappingProperty.GetMappingFunctionData()->GetFunction();
				if (function == MappingFunctionData::Function::Numeric2Value) {

					Numeric2ShapeMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Numeric2ShapeMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(fieldData->second->GetData()[index].toDouble());
				}
				else if (function == MappingFunctionData::Function::Text2Value) {

					Text2ShapeMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Text2ShapeMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(fieldData->second->GetData()[index].toString().toStdWString());
				}
				else if (function == MappingFunctionData::Function::Range2Value) {

					Range2ShapeMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Range2ShapeMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(fieldData->second->GetData()[index].toDouble());
				}
			}
		}

		//Return the max value as the default if no transform takes place
		return mappingProperty.GetValue();
	}

	VirtualTopologyInfo::Type Transformer::TransformProperty(const InputBinding& binding, const VirtualTopologyMappingProperty& mappingProperty, const InputFieldDataMap& queryResultData, unsigned int index) const {

		InputField::HashID id = binding.GetInputFieldID();
		if (id != 0) {

			InputFieldDataMap::const_iterator fieldData = queryResultData.find(id);
			if (fieldData != queryResultData.end()) {

				MappingFunctionData::Function function = mappingProperty.GetMappingFunctionData()->GetFunction();
				if (function == MappingFunctionData::Function::Numeric2Value) {

					Numeric2VirtualTopologyMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Numeric2VirtualTopologyMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(fieldData->second->GetData()[index].toDouble());
				}
				else if (function == MappingFunctionData::Function::Text2Value) {

					Text2VirtualTopologyMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Text2VirtualTopologyMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(fieldData->second->GetData()[index].toString().toStdWString());
				}
				else if (function == MappingFunctionData::Function::Range2Value) {

					Range2VirtualTopologyMappingData::ConstSharedPtr valueMappingData = std::dynamic_pointer_cast<const Range2VirtualTopologyMappingData>(mappingProperty.GetMappingFunctionData());
					return valueMappingData->GetOutputValueFromInput(fieldData->second->GetData()[index].toDouble());
				}
			}
		}

		//Return the max value as the default if no transform takes place
		return mappingProperty.GetValue();
	}

	bool Transformer::HaveDatasourcesBeenUpdated(const DataTransformMapping& mapping, std::time_t lastUpdateTime) const {

		DatasourceMaps datasourcesInUse = mapping.GetDatasourcesInUse();
		for (auto datasource : datasourcesInUse.GetFileDatasources()) {

			if (HasFileBeenUpdated(datasource.second.GetDBName(), lastUpdateTime)) {

				return true;
			}
		}

		return false;
	}

	bool Transformer::HaveUserImageFilesBeenUpdated(const DataTransformMapping& mapping, std::time_t lastUpdateTime) const {

		for (const auto& baseObject : mapping.GetBaseObjects()) {

			SynGlyphX::UserDefinedBaseImageProperties::ConstSharedPtr properties = std::dynamic_pointer_cast<const SynGlyphX::UserDefinedBaseImageProperties>(baseObject.GetProperties());
			if (properties) {

				if (HasFileBeenUpdated(properties->GetFilename(), lastUpdateTime)) {

					return true;
				}
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

			DataMappingGlyphGraph::GlyphIterator& rootGlyph = minMaxTree.second->GetRoot();

			QVariantList queryResultDataX;
			QVariantList queryResultDataY;
			const DataMappingGlyphGraph::InputFieldMap& inputFields = minMaxTree.second->GetInputFields();

			RunSelectSqlQuery(inputFields.at(rootGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::PositionX).GetInputFieldID()), queryResultDataX);
			RunSelectSqlQuery(inputFields.at(rootGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::PositionY).GetInputFieldID()), queryResultDataY);

			size_t numGlyphs = queryResultDataX.length();
			for (int i = 0; i < numGlyphs; ++i) {
				points.push_back(GeographicPoint(queryResultDataX[i].toDouble(), queryResultDataY[i].toDouble()));
			}
		}
	}

	std::wstring Transformer::GenerateTag(const DataMappingGlyphGraph::ConstGlyphIterator& minMaxGlyph, DataMappingGlyphGraph::ConstSharedPtr minMaxTree, const InputFieldDataMap& queryResultData, unsigned int index) const {

		InputField::HashID id = minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::Tag).GetInputFieldID();

		if (id == 0) {

			if (m_defaults.IsDefaultTagFieldSet()) {

				id = minMaxGlyph->second.GetInputBinding(m_defaults.GetTagField()).GetInputFieldID();
			}
		}

		if (id != 0) {

			InputFieldDataMap::const_iterator dataList = queryResultData.find(id);
			if (dataList != queryResultData.end()) {

				std::wstring tag = minMaxTree->GetInputFields().find(id)->second.GetField() + L": " + dataList->second->GetData()[index].toString().toStdWString();
				return tag;
			}
		}

		return m_defaults.GetDefaultTagValue();
	}

	std::wstring Transformer::GenerateURL(const DataMappingGlyphGraph::ConstGlyphIterator& minMaxGlyph, const InputFieldDataMap& queryResultData, unsigned int index) const {

		InputField::HashID id = minMaxGlyph->second.GetInputBinding(DataMappingGlyph::MappableField::URL).GetInputFieldID();

		if (id != 0) {

			InputFieldDataMap::const_iterator dataList = queryResultData.find(id);
			if (dataList != queryResultData.end()) {

				std::wstring tag = dataList->second->GetData()[index].toString().toStdWString();
				return tag;
			}
		}

		return L"";
	}

	const QString& Transformer::GetSourceDataCacheLocation() const {

		return m_sourceDataCacheLocation;
	}

	bool Transformer::DownloadBaseImage(const SynGlyphX::DataTransformMapping& mapping, const SynGlyphX::BaseImage& baseImage, const QString& baseImageFilename) {

		if (QFile::exists(baseImageFilename)) {

			if (!QFile::remove(baseImageFilename)) {

				throw std::exception("Failed to remove old base image");
			}
		}

		std::vector<GeographicPoint> points;
		GetPositionXYForAllGlyphTrees(mapping, points);
		
		NetworkDownloader& downloader = NetworkDownloader::Instance();
		try {

			m_overrideRootXYBoundingBox = downloader.DownloadMap(points, baseImageFilename.toStdString(), std::dynamic_pointer_cast<const DownloadedMapProperties>(baseImage.GetProperties()));
			return true;
		}
		catch (const DownloadException& e) {

			m_error = QObject::tr("Base image failed to download so the world map was used instead.\n\nError: ") + e.what();
			m_overrideRootXYBoundingBox = GeographicBoundingBox(GeographicPoint(0.0, 0.0), 90.0, 180.0);
			return false;
		}
	}

	void Transformer::CopyImage(const QString& sourceFilename, const QString& baseImageFilename) {

		if (!QFile::copy(sourceFilename, baseImageFilename)) {

			throw std::exception("Failed to copy base image");
		}
	}

	QString Transformer::GetUserImageFilename(const SynGlyphX::BaseImage& baseImage) const {

		auto properties = std::dynamic_pointer_cast<const SynGlyphX::UserDefinedBaseImageProperties>(baseImage.GetProperties());
		return QString::fromStdWString(properties->GetFilename());
	}

	const QString& Transformer::GetError() const {

		return m_error;
	}

	void Transformer::SetDefaultImagesDirectory(const QString& defaultImagesDirectory) {

		s_defaultImagesDirectory = defaultImagesDirectory;
	}

	void Transformer::GetMinMaxForFieldGroups(const DataTransformMapping& mapping) {

		for (const auto& fieldGroup : mapping.GetFieldGroupMap()) {

			m_fieldGroupsMinAndMax[fieldGroup.first] = GetMinMaxForGroup(fieldGroup.second);
		}
	}

	DoubleMinDiff Transformer::GetMinMaxForGroup(const FieldGroup& fieldGroup) const {

		double min = std::numeric_limits<double>::max();
		double max = std::numeric_limits<double>::min();
		for (const auto& inputField : fieldGroup) {

			SharedSQLQuery minAndMaxQuery = m_sourceDataCache.CreateMinMaxQuery(inputField);
			minAndMaxQuery->exec();
			minAndMaxQuery->first();

			min = std::min(min, minAndMaxQuery->value(0).toDouble());
			max = std::max(max, minAndMaxQuery->value(1).toDouble());

			minAndMaxQuery->finish();
		}

		return DoubleMinDiff(min, max - min);
	}

	double Transformer::ValidateInputForMinAndMax(double input, const DoubleMinDiff& minDiff) const {

		return std::min(std::max(input, minDiff.GetMin()), minDiff.GetMax());
	}

} //namespace SynGlyphX