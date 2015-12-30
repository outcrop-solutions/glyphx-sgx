package synglyphx.glyph;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import synglyphx.glyph.XMLGlyphTemplate;
import synglyphx.glyph.GlyphCreator;
import synglyphx.data.SourceDataInfo;
import synglyphx.io.Logger;
import synglyphx.util.BaseObject;

public class Mapper {

	private Map<Integer, XMLGlyphTemplate> temps = null;
	private int mappingCount;
	private boolean download;

	public Mapper(){
		mappingCount = 0;
		temps = new HashMap<Integer, XMLGlyphTemplate>();
	}

	public void addNodeTemplates(HashMap<Integer, XMLGlyphTemplate> temps, int count){
		this.temps = temps;
		this.mappingCount = count;
	
	}

	public void checkRangeXY(boolean dl){
		download = dl;
	}

	public void generateGlyphTrees(ArrayList<SourceDataInfo> csvData, ArrayList<Integer> rootIds, String outDir, String[] colorStr, String app, ArrayList<BaseObject> base_objects){
		GlyphCreator creator = new GlyphCreator(csvData, temps, mappingCount, rootIds);
		creator.setOutDir(outDir, app);
		creator.checkRangeXY(download);
		Logger.getInstance().add("Beginning to generate glyphs...");
		creator.begin();
		creator.printGlyphRepo(colorStr, base_objects);
	}

}