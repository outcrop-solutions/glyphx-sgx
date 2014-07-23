#ifndef SYNGLYPHX_MINMAXGLYPHTREE_H
#define SYNGLYPHX_MINMAXGLYPHTREE_H

#include "sgxdatatransform_global.h"
#include <containers/ntree.hpp>
#include "minmaxglyph.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT MinMaxGlyphTree : public stlplus::ntree < MinMaxGlyph >
	{
	public:
		typedef boost::shared_ptr<MinMaxGlyphTree> SharedPtr;
		typedef boost::shared_ptr<const MinMaxGlyphTree> ConstSharedPtr;
		typedef boost::property_tree::wptree PropertyTree;
		typedef stlplus::ntree_node<MinMaxGlyph> Node;

		MinMaxGlyphTree();
		MinMaxGlyphTree(const boost::property_tree::wptree& propertyTree);
		MinMaxGlyphTree(const GlyphTree& glyphTree);
		~MinMaxGlyphTree();

		PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& propertyTreeParent) const;
		void WriteToFile(const std::string& filename) const;
		void ReadFromFile(const std::string& filename);

	private:
		void ProcessPropertyTreeChildren(const MinMaxGlyphTree::iterator& iT, const boost::property_tree::wptree& propertyTree);
		void AddGlyphSubtree(MinMaxGlyphTree::iterator& parentNode, const GlyphTree& glyphTree, const GlyphTree::const_iterator& iT);
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_MINMAXGLYPHTREE_H