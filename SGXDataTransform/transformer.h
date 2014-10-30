#ifndef SYNGLYPHX_TRANSFORMER_H
#define SYNGLYPHX_TRANSFORMER_H

#include "sgxdatatransform_global.h"
#include "glyph.h"
#include "minmaxglyphtree.h"
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

		GlyphTree::ConstSharedVector CreateGlyphTreesFromMinMaxTrees(const DataTransformMapping& mapping) const;
		GlyphTree::ConstSharedVector CreateGlyphTreesFromMinMaxTree(MinMaxGlyphTree::ConstSharedPtr minMaxTree) const;
		GlyphProperties ProcessMinMaxGlyph(const MinMaxGlyphTree::const_iterator& minMaxGlyph, const InputFieldDataMap& queryResultData, unsigned int index) const;
		void AddChildrenToGlyphTree(GlyphTree::SharedPtr tree, GlyphTree::iterator newNode, MinMaxGlyphTree::ConstSharedPtr minMaxTree, MinMaxGlyphTree::const_iterator node, const InputFieldDataMap& queryResultData, unsigned int index) const;
		double LinearInterpolate(const InputBinding& binding, double min, double difference, const InputFieldDataMap& queryResultData, unsigned int index) const;
		double LinearInterpolate(double min, double difference, double dataMin, double dataDifference, double currentData) const;
		Color ColorRGBInterpolate(const InputBinding& binding, const Color& min, const Color& difference, const InputFieldDataMap& queryResultData, unsigned int index) const;
		void GetPositionXYForAllGlyphTrees(const SynGlyphX::DataTransformMapping& mapping, std::vector<GeographicPoint>& points) const;
		void GetDataMinAndDifference(const InputBinding& binding, const InputFieldData& fieldData, double& dataMin, double& dataDifference) const;
		std::wstring GenerateTag(const MinMaxGlyphTree::const_iterator& minMaxGlyph, const InputFieldDataMap& queryResultData, unsigned int index) const;

		SourceDataManager m_sourceDataManager;
		GeographicBoundingBox m_overrideRootXYBoundingBox;
		DataMappingDefaults m_defaults;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_TRANSFORMER_H