#ifndef XMLPROPERTYTREEFILE_H
#define XMLPROPERTYTREEFILE_H

#include "sgxfoundation.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace SynGlyphX {

	class SGXFOUNDATION_API XMLPropertyTreeFile
	{
	public:
		XMLPropertyTreeFile();
		virtual ~XMLPropertyTreeFile();

		void ReadFromFile(const std::string& filename);
		void WriteToFile(const std::string& filename) const;

		static int GetReadFlags();
		static const boost::property_tree::xml_writer_settings<wchar_t>& GetWriteSettings();

	protected:
		virtual void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) = 0;
		virtual void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const = 0;

	private:
		static boost::property_tree::xml_writer_settings<wchar_t> s_writeSettings;
	};

} //namespace SynGlyphX

#endif //XMLPROPERTYTREEFILE_H