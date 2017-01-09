package synglyphx.link;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import synglyphx.data.Query;
import synglyphx.data.Cursor;
import synglyphx.data.SourceDataInfo;
import synglyphx.glyph.XMLGlyphTemplate;
import synglyphx.io.Logger;

public class LinkCreator {
	
	private Map<Integer, LinkTemplate> link_temps = null;
	private Map<Integer, XMLGlyphTemplate> temps = null;
	private ArrayList<SourceDataInfo> dataPaths = null;
	private ArrayList<Integer> rootIds = null;

	public LinkCreator(Map<Integer, LinkTemplate> link_temps,
		Map<Integer, XMLGlyphTemplate> temps,
		ArrayList<SourceDataInfo> dataPaths,
		ArrayList<Integer> rootIds){
		this.link_temps = link_temps;
		this.dataPaths = dataPaths;
		this.rootIds = rootIds;
		this.temps = temps;
		//System.out.println("Number of data sources: "+String.valueOf(dataPaths.size()));
	}

	public void begin(){

		Thread t1, t2;

		for(Map.Entry<Integer, LinkTemplate> entry : link_temps.entrySet()) {
			LinkFunction funct = entry.getValue().getLinkFunction();
			HashMap<String, ArrayList<Integer>> key_nodes = new HashMap<String, ArrayList<Integer>>();

			ArrayList<Integer> segment_sizes = new ArrayList<Integer>();
			ArrayList<Integer> nodes_per_seg = new ArrayList<Integer>();
			int[] seg_info = defineSegments(entry.getValue(), segment_sizes, nodes_per_seg);

			if(funct.type().name().equals("MATCHVALUE")){
				HashMap<String, ArrayList<Integer>> key_rows = new HashMap<String, ArrayList<Integer>>();
				HashMap<String, ArrayList<Integer>> value_rows = new HashMap<String, ArrayList<Integer>>();
				t1 = mapRowIndexes(dataPaths.get(entry.getValue().getDatasourceID(0)), key_rows, entry.getValue().getFieldName(0));
				t2 = mapRowIndexes(dataPaths.get(entry.getValue().getDatasourceID(1)), value_rows, entry.getValue().getFieldName(1));
				wait(t1,t2);
				setFinalMatchValueEndpoints(entry.getValue(), key_rows, value_rows, segment_sizes, nodes_per_seg, seg_info);
			}
			else if(funct.type().name().equals("KEYVALUE")){
				HashMap<String, String> key_value = funct.getKeyValuePairs();
				ArrayList<String> kv_keys = new ArrayList<String>();
				ArrayList<String> kv_values = new ArrayList<String>();
				HashMap<String, ArrayList<Integer>> value_nodes = new HashMap<String, ArrayList<Integer>>();
				for(Map.Entry<String, String> kv : key_value.entrySet()){
					String lc_key = kv.getKey().toLowerCase();
					String lc_value = kv.getValue().toLowerCase();
					kv_keys.add(lc_key);
					kv_values.add(lc_value);
					if(!key_nodes.containsKey(lc_key)){
						key_nodes.put(lc_key, new ArrayList<Integer>());
					}	
					if(!value_nodes.containsKey(lc_value)){
						value_nodes.put(lc_value, new ArrayList<Integer>());
					}	
				}
				t1 = retrieveNodesMatchingKey(key_nodes, dataPaths.get(entry.getValue().getDatasourceID(0)), kv_keys, entry.getValue().getFieldName(0)); 
				t2 = retrieveNodesMatchingValue(value_nodes, dataPaths.get(entry.getValue().getDatasourceID(1)), kv_values, entry.getValue().getFieldName(1));
				wait(t1,t2);
				setFinalKeyValueEndpoints(entry.getValue(), key_nodes, value_nodes, segment_sizes, nodes_per_seg, seg_info);
				System.out.println("Exiting Key Value linking code...");
			}
			else if(funct.type().name().equals("KEYRANGE")){
				HashMap<String, double[]> range_value = funct.getKeyRangePairs();
				ArrayList<String> rv_keys = new ArrayList<String>();
				ArrayList<double[]> rv_values = new ArrayList<double[]>();
				HashMap<double[], ArrayList<Integer>> range_nodes = new HashMap<double[], ArrayList<Integer>>();
				for(Map.Entry<String, double[]> rv : range_value.entrySet()){
					rv_keys.add(rv.getKey());
					rv_values.add(rv.getValue());
					if(!key_nodes.containsKey(rv.getKey())){
						key_nodes.put(rv.getKey(), new ArrayList<Integer>());
					}	
					if(!range_nodes.containsKey(rv.getValue())){
						range_nodes.put(rv.getValue(), new ArrayList<Integer>());
					}	
				}
				t1 = retrieveNodesMatchingKey(key_nodes, dataPaths.get(entry.getValue().getDatasourceID(0)), rv_keys, entry.getValue().getFieldName(0));
				t2 = retrieveNodesMatchingRange(range_nodes, dataPaths.get(entry.getValue().getDatasourceID(1)), rv_values, entry.getValue().getFieldName(1));
				wait(t1,t2);
				setFinalRangeValueEndpoints(entry.getValue(), key_nodes, range_nodes, segment_sizes, nodes_per_seg, seg_info);
			}
		}
	}

	public int[] defineSegments(LinkTemplate link_temp,
		ArrayList<Integer> segment_sizes, ArrayList<Integer> nodes_per_seg){
		//NEED TO CHECK FOR CSV OR SQLITE3
		int b_temp_id = link_temp.getGlyphTempID(0);
		int e_temp_id = link_temp.getGlyphTempID(1);
		System.out.println(b_temp_id);
		System.out.println(e_temp_id);
		int b_temp_seg = -1;
		int e_temp_seg = -1;

		System.out.print("Temps size: ");
		System.out.println(temps.size());

		for(int i = 0; i < rootIds.size(); i++){
			System.out.print("root id: ");
			System.out.println(rootIds.get(i));
			if(i+1 != rootIds.size()){
				if(b_temp_id >= rootIds.get(i) && b_temp_id < rootIds.get(i+1)){
					b_temp_seg = i;
				}
				if(e_temp_id >= rootIds.get(i) && e_temp_id < rootIds.get(i+1)){
					e_temp_seg = i;
				}
				segment_sizes.add(rootIds.get(i+1)-rootIds.get(i));
			}else{
				if(b_temp_seg == -1){b_temp_seg = i;}
				if(e_temp_seg == -1){e_temp_seg = i;}
				if(rootIds.size() == rootIds.get(i)){
					if(rootIds.get(i) == temps.get(rootIds.get(i)).getLastChildID()){
						segment_sizes.add(1);
					}else{
						int prev_sizes = 0;
						for(int x = 0; x < segment_sizes.size(); x++){
							prev_sizes += segment_sizes.get(x);
						}
						segment_sizes.add(temps.get(rootIds.get(i)).getLastChildID()-prev_sizes);
					}
				}else{
					segment_sizes.add(temps.size()-(rootIds.get(i)-1));
				}
			}
			nodes_per_seg.add(segment_sizes.get(i)*dataPaths.get(temps.get(rootIds.get(i)).getDataSource()).getRowCount());
			System.out.print("Segment size: ");
			System.out.println(segment_sizes.get(i));
			System.out.print("Nodes per segment: ");
			System.out.println(nodes_per_seg.get(i));
		}
		int[] temps = new int[4];
		temps[0] = b_temp_id; temps[1] = b_temp_seg;
		temps[2] = e_temp_id; temps[3] = e_temp_seg;
		System.out.println(b_temp_seg);
		System.out.println(e_temp_seg);
		return temps;
	}

	public int[] getPlaceIDs(int[] seg_info,
		ArrayList<Integer> segment_sizes,
		ArrayList<Integer> nodes_per_seg){

		int[] ids = new int[2];
		ids[0] = getNodeStartPos(seg_info[0],seg_info[1],segment_sizes,nodes_per_seg);
		ids[1] = getNodeStartPos(seg_info[2],seg_info[3],segment_sizes,nodes_per_seg);
		return ids;
	}

	private int getNodeStartPos(int glyph_temp_pos, int segment_id,
		ArrayList<Integer> segment_sizes,
		ArrayList<Integer> nodes_per_seg){

		if(segment_id == 0){
			return 0;
		}else{
			int total_displacement = 0;
			for(int i = segment_id-1; i >= 0; i--){
				total_displacement += nodes_per_seg.get(i);
			}
			return total_displacement;
		}
	}

	private int placeInSegment(int glyph_temp_pos, int segment_id, 
		ArrayList<Integer> segment_sizes){

		if(segment_id == 0){
			return glyph_temp_pos;
		}else{
			int total_displacement = 0;
			for(int i = segment_id-1; i >= 0; i--){
				total_displacement += segment_sizes.get(i);
			}
			return glyph_temp_pos-total_displacement;
		}
	}

	public void setFinalMatchValueEndpoints(LinkTemplate link_temp,
		HashMap<String, ArrayList<Integer>> key_rows, 
		HashMap<String, ArrayList<Integer>> value_rows,
		ArrayList<Integer> segment_sizes,
		ArrayList<Integer> nodes_per_seg,
		int[] seg_info){

		int[] b_e_ids = getPlaceIDs(seg_info, segment_sizes, nodes_per_seg);

		for(Map.Entry<String,ArrayList<Integer>> entry : key_rows.entrySet()){
			if(value_rows.containsKey(entry.getKey())){
				ArrayList<Integer> base_set = entry.getValue();
				ArrayList<Integer> end_set = value_rows.get(entry.getKey());
				for(int i = 0; i < base_set.size(); i++){
					int b_id = ((base_set.get(i)) * segment_sizes.get(seg_info[1])) + b_e_ids[0];
					b_id += placeInSegment(seg_info[0],seg_info[1],segment_sizes);
					for(int j = 0; j < end_set.size(); j++){
						int e_id = ((end_set.get(j)) * segment_sizes.get(seg_info[3])) + b_e_ids[1];
						e_id += placeInSegment(seg_info[2],seg_info[3],segment_sizes);
						if(b_id != e_id){ //Needs to be removed for intra-glyph links 
							link_temp.addEndpointIDs(b_id, e_id);
						}
					}
				}
			}
		}

	}

	public void setFinalKeyValueEndpoints(LinkTemplate link_temp,
		HashMap<String, ArrayList<Integer>> key_rows, 
		HashMap<String, ArrayList<Integer>> value_rows,
		ArrayList<Integer> segment_sizes,
		ArrayList<Integer> nodes_per_seg,
		int[] seg_info){

		int[] b_e_ids = getPlaceIDs(seg_info, segment_sizes, nodes_per_seg);

		HashMap<String, String> key_value = link_temp.getLinkFunction().getKeyValuePairs();

		for(Map.Entry<String,String> entry : key_value.entrySet()){
			ArrayList<Integer> base_set = key_rows.get(entry.getKey().toLowerCase());
			ArrayList<Integer> end_set = value_rows.get(entry.getValue().toLowerCase());
			if(base_set != null && end_set != null){
				for(int i = 0; i < base_set.size(); i++){
					int b_id = ((base_set.get(i)) * segment_sizes.get(seg_info[1])) + b_e_ids[0];
					b_id += placeInSegment(seg_info[0],seg_info[1],segment_sizes);
					for(int j = 0; j < end_set.size(); j++){
						int e_id = ((end_set.get(j)) * segment_sizes.get(seg_info[3])) + b_e_ids[1];
						e_id += placeInSegment(seg_info[2],seg_info[3],segment_sizes);
						if(b_id != e_id){ //Needs to be removed for intra-glyph links
							if(j == 0){/*
								System.out.println(entry.getKey());
								System.out.print("placeInSegment 0 1: ");
								System.out.println(placeInSegment(seg_info[0],seg_info[1],segment_sizes));
								System.out.print("placeInSegment 2 3: ");
								System.out.println(placeInSegment(seg_info[2],seg_info[3],segment_sizes));
								System.out.print(seg_info[0]);System.out.print(",");System.out.println(seg_info[1]);
								System.out.print(seg_info[2]);System.out.print(",");System.out.println(seg_info[3]);
								System.out.print(b_id+7);System.out.print(",");System.out.println(e_id+7);
								System.out.print("Base set: ");
								System.out.println(base_set.get(i));
								System.out.print("End set: ");
								System.out.println(end_set.get(j));
								System.out.print("b_e_ids 0: ");
								System.out.println(b_e_ids[0]);
								System.out.print("b_e_ids 1: ");
								System.out.println(b_e_ids[1]);
								System.out.println("");*/
							}
							link_temp.addEndpointIDs(b_id, e_id);
						}
					}
				}
			}
		}
	}

	public void setFinalRangeValueEndpoints(LinkTemplate link_temp,
		HashMap<String, ArrayList<Integer>> key_rows, 
		HashMap<double[], ArrayList<Integer>> value_rows,
		ArrayList<Integer> segment_sizes,
		ArrayList<Integer> nodes_per_seg,
		int[] seg_info){

		int[] b_e_ids = getPlaceIDs(seg_info, segment_sizes, nodes_per_seg);

		HashMap<String, double[]> key_range = link_temp.getLinkFunction().getKeyRangePairs();

		for(Map.Entry<String,double[]> entry : key_range.entrySet()){
			ArrayList<Integer> base_set = key_rows.get(entry.getKey().toLowerCase());
			ArrayList<Integer> end_set = value_rows.get(entry.getValue());
			if(base_set != null && end_set != null){
				for(int i = 0; i < base_set.size(); i++){
					int b_id = ((base_set.get(i)) * segment_sizes.get(seg_info[1])) + b_e_ids[0];
					b_id += placeInSegment(seg_info[0],seg_info[1],segment_sizes);
					for(int j = 0; j < end_set.size(); j++){
						int e_id = ((end_set.get(j)) * segment_sizes.get(seg_info[3])) + b_e_ids[1];
						e_id += placeInSegment(seg_info[2],seg_info[3],segment_sizes);
						if(b_id != e_id){ //Needs to be removed for intra-glyph links
							link_temp.addEndpointIDs(b_id, e_id);
						}
					}
				}
			}
		}
	}

	public Thread mapRowIndexes(final SourceDataInfo ds, final HashMap<String, ArrayList<Integer>> rows, final String field){

		Thread thread = new Thread(){
			public void run(){
				if(ds.getType().equals("csv") || ds.getType().equals("sqlite3")){
					Query query = new Query(ds.getDataFrame()).all(); 
					Cursor cursor = ds.getDataFrame().query(query);

					int row_count = 0;
					while(cursor.next()){
						String fn = cursor.get(field).toLowerCase();
						if(!rows.containsKey(fn)){
							rows.put(fn, new ArrayList<Integer>());
						}
						rows.get(fn).add(row_count);
						row_count++;
					}
				}
			}
		};
		thread.start();
		return thread;
	}

	public Thread retrieveNodesMatchingKey(final HashMap<String, ArrayList<Integer>> key_nodes, 
		final SourceDataInfo ds, final ArrayList<String> keys, final String field_name){

		Thread thread = new Thread(){
			public void run(){
				if(ds.getType().equals("csv") || ds.getType().equals("sqlite3")){
					Query query = new Query(ds.getDataFrame()).all(); 
					Cursor cursor = ds.getDataFrame().query(query);
					Logger.getInstance().addT("Retrieving nodes matching keys..."+String.valueOf(key_nodes.size()));
					int row_count = 0;
					while(cursor.next()){
						String fn = cursor.get(field_name).toLowerCase();
						if(key_nodes.containsKey(fn)){
							Logger.getInstance().addT("Key: "+fn);
							key_nodes.get(fn).add(row_count);
						}
						row_count++;
					}
				}
			}
		};
		thread.start();
		return thread;
	}

	public Thread retrieveNodesMatchingValue(final HashMap<String, ArrayList<Integer>> value_nodes,
		final SourceDataInfo ds, final ArrayList<String> values, final String field_name){

		Thread thread = new Thread(){
			public void run(){
				if(ds.getType().equals("csv") || ds.getType().equals("sqlite3")){
					Query query = new Query(ds.getDataFrame()).all(); 
					Cursor cursor = ds.getDataFrame().query(query);

					int row_count = 0;
					while(cursor.next()){
						String fn = cursor.get(field_name).toLowerCase();
						if(value_nodes.containsKey(fn)){
							Logger.getInstance().addT("Value: "+fn);
							value_nodes.get(fn).add(row_count);
						}
						row_count++;
					}
				}
			}
		};
		thread.start();
		return thread;
	}

	public Thread retrieveNodesMatchingRange(final HashMap<double[], ArrayList<Integer>> range_nodes,
		final SourceDataInfo ds, final ArrayList<double[]> ranges, final String field_name){

		Thread thread = new Thread(){
			public void run(){
				if(ds.getType().equals("csv") || ds.getType().equals("sqlite3")){
					Query query = new Query(ds.getDataFrame()).all(); 
					Cursor cursor = ds.getDataFrame().query(query);

					int row_count = 0;
					while(cursor.next()){
						double temp = Double.parseDouble(cursor.get(field_name));
						for(int i = 0; i < ranges.size(); i++){
							if(temp >= ranges.get(i)[0] && temp <= ranges.get(i)[1]){
								range_nodes.get(ranges.get(i)).add(row_count);
							}
						}
						row_count++;
					}
				}
			}
		};
		thread.start();
		return thread;
	}

	public void wait(Thread t1, Thread t2){
		try{
			t1.join();
			t2.join();
		}catch(InterruptedException ie){
	        ie.printStackTrace();
		}
	}

}