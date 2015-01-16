#ifndef SYNGLYPHX_TRANSFORMER_H
#define SYNGLYPHX_TRANSFORMER_H

#include "sgxdatatransform_global.h"
#include "glyphgraph.h"
#include "datamappingglyphgraph.h"
#include "sourcedatamanager.h"
#include "geographicboundingbox.h"
#include "inputfielddata.h"
#include "datamappingdefaults.h"

namespace SynGlyphX {

	class DataTransformMapping;

	class SGXDATATRANSFORM_EXPORT Transformer
	{
	public:
		typedef std::unordered_map<SynGlyphX::InputField::HashID, InputFieldData::SharedPtr> InputFieldDataMap;

		Transformer();
		~Transformer();

		void Transform(const DataTransformMapping& mapping);

	protected:
		virtual void CreateGlyphsFromMapping(const DataTransformMapping& mapping) = 0;
		bool HaveDatasourcesBeenUpdated(const DataTransformMapping& mapping, std::time_t lastUpdateTime) const;
		bool HasFileBeenUpdated(const std::wstring& filename, std::time_t lastUpdateTime) const;

		GlyphGraph::ConstSharedVector CreateGlyphTreesFromMinMaxTrees(const DataTransformMapping& mapping) const;
		GlyphGraph::ConstSharedVector CreateGlyphTreesFromMinMaxTree(DataMappingGlyphGraph::ConstSharedPtr minMaxTree) const;
		Glyph ProcessMinMaxGlyph(const DataMappingGlyphGraph::const_iterator& minMaxGlyph, const InputFieldDataMap& queryResultData, unsigned int index) const;
		void AddChildrenToGlyphTree(GlyphGraph::SharedPtr tree, GlyphGraph::iterator newNode, DataMappingGlyphGraph::ConstSharedPtr minMaxTree, DataMappingGlyphGraph::const_iterator node, const InputFieldDataMap& queryResultData, unsigned int index) const;
		double LinearInterpolate(const InputBinding& binding, double min, double difference, const InputFieldDataMap& queryResultData, unsigned int index) const;
		double LinearInterpolate(double min, double difference, double dataMin, double dataDifference, double currentData) const;
		GlyphColor ColorRGBInterpolate(const InputBinding& binding, const GlyphColor& min, const GlyphColor& difference, const InputFieldDataMap& queryResultData, unsigned int index) const;
		void GetPositionXYForAllGlyphTrees(const SynGlyphX::DataTransformMapping& mapping, std::vector<GeographicPoint>& points) const;
		void GetDataMinAndDifference(const InputBinding& binding, const InputFieldData& fieldData, double& dataMin, double& dataDifference) const;
		std::wstring GenerateTag(const DataMappingGlyphGraph::const_iterator& minMaxGlyph, const InputFieldDataMap& queryResultData, unsigned int index) const;

		SourceDataManager m_sourceDataManager;
		GeographicBoundingBox m_overrideRootXYBoundingBox;
		DataMappingDefaults m_defaults;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_TRANSFORMER_H