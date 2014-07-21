#ifndef SYNGLYPHX_MINMAXGLYPHTREE_H
#define SYNGLYPHX_MINMAXGLYPHTREE_H

#include <containers/ntree.hpp>
#include "minmaxglyph.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

namespace SynGlyphX {

	class MinMaxGlyphTree : public stlplus::ntree < MinMaxGlyph >
	{
	public:
		typedef boost::shared_ptr<MinMaxGlyphTree> SharedPtr;
		typedef boost::shared_ptr<const MinMaxGlyphTree> ConstSharedPtr;
		typedef boost::property_tree::wptree PropertyTree;

		MinMaxGlyphTree();
		MinMaxGlyphTree(const boost::property_tree::wptree& propertyTree);
		~MinMaxGlyphTree();

		PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& propertyTreeParent) const;
		void WriteToFile(const std::string& filename) const;
		void ReadFromFile(const std::string& filename);

	private:
		void ProcessPropertyTree(const boost::property_tree::wptree& propertyTree);
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_MINMAXGLYPHTREE_H