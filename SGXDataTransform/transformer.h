#ifndef SYNGLYPHX_TRANSFORMER_H
#define SYNGLYPHX_TRANSFORMER_H

#include "sgxdatatransform_global.h"
#include "glyph.h"
#include "minmaxglyphtree.h"

namespace SynGlyphX {

	class DataTransformMapping;

	class SGXDATATRANSFORM_EXPORT Transformer
	{
	public:
		Transformer();
		~Transformer();

		void Transform(DataTransformMapping& mapping);

	protected:
		virtual void CreateGlyphsFromMapping(DataTransformMapping& mapping) = 0;
		bool HaveDatasourcesBeenUpdated(const DataTransformMapping& mapping, std::time_t lastUpdateTime) const;

		GlyphTree::ConstSharedVector CreateGlyphTreesFromMinMaxTrees(const DataTransformMapping& mapping) const;
		GlyphTree::ConstSharedVector CreateGlyphTreesFromMinMaxTree(MinMaxGlyphTree::ConstSharedPtr minMaxTree) const;
		GlyphProperties ProcessMinMaxGlyph(const MinMaxGlyphTree::const_iterator& minMaxGlyph, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const;
		void AddChildrenToGlyphTree(GlyphTree::SharedPtr tree, GlyphTree::iterator newNode, MinMaxGlyphTree::ConstSharedPtr minMaxTree, MinMaxGlyphTree::const_iterator node, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const;
		double LinearInterpolate(const InputBinding& binding, double min, double difference, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const;
		Color ColorRGBInterpolate(const InputBinding& binding, const Color& min, const Color& difference, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_TRANSFORMER_H