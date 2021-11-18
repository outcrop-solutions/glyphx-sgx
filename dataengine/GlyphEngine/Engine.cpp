#include "Engine.h"
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <QFile>
#include <QTextStream>

namespace GlyphEngine
{
	Engine::Engine()
	{

	}

	void Engine::initiate() 
	{
		
		std::string sdtname = "C:/ProgramData/SynGlyphX/Content/mcgee_sku_model/mcgee_sku_model/dark-mode-model - Copy.sdt";

		reader = new SDTReader();
		reader->ReadSDTFile(sdtname); //Are we done here? Test

		DataSource::SharedPtr dataSource = reader->GetDataSource();
		std::vector<BaseImage> baseImages = reader->GetBaseImages();
		DataMappingGlyph* glyph = reader->GetGlyphTemplate();

		std::wstring filename = dataSource->GetFormattedName();
		size_t count = baseImages.size();
		int label = glyph->GetLabel();

		QString fn = "data.txt";
		QFile file(fn);
		if (file.open(QIODevice::ReadWrite)) {
			QTextStream stream(&file);
			stream << QString::fromStdWString(filename) << endl;
			stream << QString::number(count) << endl;
			stream << QString::number(label) << endl;
		}


	}

	void Engine::generateGlyphs()
	{

	}

}
