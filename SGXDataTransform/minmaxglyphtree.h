#ifndef SYNGLYPHX_MINMAXGLYPHTREE_H
#define SYNGLYPHX_MINMAXGLYPHTREE_H

#include "sgxdatatransform_global.h"
#include <containers/ntree.hpp>
#include "minmaxglyph.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include "inputfield.h"
#include <unordered_map>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT MinMaxGlyphTree : public stlplus::ntree <MinMaxGlyph>
	{
	public:
		typedef boost::shared_ptr<MinMaxGlyphTree> SharedPtr;
		typedef boost::shared_ptr<const MinMaxGlyphTree> ConstSharedPtr;
		typedef boost::property_tree::wptree PropertyTree;
		typedef stlplus::ntree_node<MinMaxGlyph> Node;
		typedef std::unordered_map<InputField::HashID, InputField> InputFieldMap;

		MinMaxGlyphTree();
		MinMaxGlyphTree(const boost::property_tree::wptree& propertyTree);
		MinMaxGlyphTree(const GlyphTree& glyphTree);
		~MinMaxGlyphTree();

		PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& propertyTreeParent) const;
		void WriteToFile(const std::string& filename) const;
		void ReadFromFile(const std::string& filename);

		void SetInputField(MinMaxGlyphTree::const_iterator& node, unsigned int index, const InputField& inputfield, double min = 0.0, double max = 0.0);
		void ClearInputBinding(MinMaxGlyphTree::const_iterator& node, unsigned int index);

		const InputFieldMap& GetInputFields() const;

		bool DoesRootGlyphPositionXYHaveBindings() const;

		GlyphTree::SharedPtr GetMinGlyphTree() const;

	private:
		void ExportToPropertyTree(const MinMaxGlyphTree::const_iterator& parent, boost::property_tree::wptree& propertyTreeParent) const;
		void ProcessPropertyTreeChildren(const MinMaxGlyphTree::iterator& iT, const boost::property_tree::wptree& propertyTree);
		void AddGlyphSubtree(MinMaxGlyphTree::iterator& parentNode, const GlyphTree& glyphTree, const GlyphTree::const_iterator& iT);
		void CreateMinGlyphSubtree(const MinMaxGlyphTree::const_iterator& parentNode, GlyphTree::iterator& newParent, GlyphTree::SharedPtr newGlyphTree) const;

		InputFieldMap m_inputFields;
		std::unordered_map<InputField::HashID, unsigned int> m_inputFieldReferenceCounts;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_MINMAXGLYPHTREE_H