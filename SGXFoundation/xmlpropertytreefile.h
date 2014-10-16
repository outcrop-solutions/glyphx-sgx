#ifndef XMLPROPERTYTREEFILE_H
#define XMLPROPERTYTREEFILE_H

#include "sgxfoundation.h"
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXFOUNDATION_API XMLPropertyTreeFile
	{
	public:
		XMLPropertyTreeFile();
		virtual ~XMLPropertyTreeFile();

		void ReadFromFile(const std::string& filename);
		void WriteToFile(const std::string& filename) const;

	protected:
		virtual void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) = 0;
		virtual void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const = 0;
	};

} //namespace SynGlyphX

#endif //XMLPROPERTYTREEFILE_H