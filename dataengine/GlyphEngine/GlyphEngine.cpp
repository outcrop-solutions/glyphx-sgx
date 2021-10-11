#include "GlyphEngine.h"
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace GlyphEngine
{
	Engine::Engine()
	{

	}

	void Engine::initiate() 
	{
		
		std::string filename = "C:/ProgramData/SynGlyphX/Content/mcgee_sku_model/mcgee_sku_model/dark-mode-model - Copy.sdt";

		reader = new SDTReader();
		reader->ReadSDTFile(filename);

	}

	void Engine::generateGlyphs()
	{

	}

}
