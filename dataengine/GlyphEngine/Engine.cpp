#include "Engine.h"
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QDebug>

namespace GlyphEngine
{
	Engine::Engine()
	{

	}

	void Engine::initiate() 
	{
		
		std::string sdtname = "C:/ProgramData/SynGlyphX/Content/mcgee_sku_model/mcgee_sku_model/dark-mode-model - Copy.sdt";

		reader = new SDTReader();
		reader->ReadSDTFile(sdtname);

		Data *dataSource = reader->GetDataSource();
		std::wstring filename = dataSource->GetHost();
		int field_count = dataSource->GetInputFieldCount();

		std::vector<BaseObject> baseImages = reader->GetBaseImages();
		size_t count = baseImages.size();

		GlyphObject* glyph = reader->GetGlyphTemplate();
		int label = glyph->GetLabel();
		/*
		QString fn = "data.txt";
		QFile file(fn);
		if (file.open(QIODevice::ReadWrite)) {
			QTextStream stream(&file);
			stream << QString::fromStdWString(filename) << endl;
			stream << QString::number(count) << endl;
			stream << QString::number(label) << endl;
			stream << QString::number(field_count) << endl;
		}
		*/

		qDebug() << filename;
		qDebug() << field_count;
		qDebug() << count;
		qDebug() << label;
	}

	void Engine::generateGlyphs()
	{

	}

}
