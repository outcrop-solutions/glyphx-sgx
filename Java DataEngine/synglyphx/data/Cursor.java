package synglyphx.data;

import java.io.*;
import java.util.Map;
import java.util.HashMap;
import java.util.TreeMap;
import java.util.ArrayList;
import java.util.List;
import java.util.Collections;

public class Cursor {

	private HashMap<String, ArrayList<String>> data = null;
	private ArrayList<Integer> indices = null;
	private int increment = -1;
	private int currPlace = 0;
	private String randHeader = "";
	private Query query = null;

	public Cursor(HashMap<String, ArrayList<String>> qd, ArrayList<Integer> indices, Query q){
		this.data = qd;
		this.indices = indices;
		//System.out.println(indices.size());
		this.query = q;
		for (Map.Entry<String, ArrayList<String>> entry : this.data.entrySet()) {
			this.randHeader = entry.getKey();
			break;
		}
		Collections.sort(indices);

		if(!q.getGrouping().equals("")){
			this.handleGrouping();
		}

		if(q.getOrdering() != null && q.getGrouping().equals("")){
			this.handleOrdering();
		}
	}

	public Integer size(){
		return indices.size();

	}

	public String get(String header){
		return data.get(header).get(currPlace);
	}

	public boolean next(){
			increment++;

		try{
			this.currPlace = indices.get(increment);
			data.get(randHeader).get(increment);
		}catch(IndexOutOfBoundsException npe){
			return false;
		}
		return true;
	}

	public int getCurrentIndex(){
		return increment;
	}

	private void handleGrouping(){
		HashMap<String, ArrayList<Integer>> grouped = new HashMap<String, ArrayList<Integer>>();
		String gHeader = query.getGrouping();

		for(int i=0; i<indices.size();i++){
			String curr = data.get(gHeader).get(indices.get(i));
			if(grouped.get(curr) == null){
				grouped.put(curr, new ArrayList<Integer>());
			}
			grouped.get(curr).add(indices.get(i));
		}

		if(query.getOrdering() != null){this.handleOrdering(grouped);return;}

		ArrayList<Integer> temp = new ArrayList<Integer>();
		for (Map.Entry<String, ArrayList<Integer>> entry : grouped.entrySet()) {
			for(int i=0; i<entry.getValue().size();i++){
				temp.add(entry.getValue().get(i));
			}
		}

		this.indices = temp;
	}

	private void handleOrdering(){
		String[] oHeader = query.getOrdering();
		Map<String, Integer> map = new TreeMap<String, Integer>();

		for(int i=0; i<indices.size();i++){
			String curr = data.get(oHeader[0]).get(indices.get(i));
			map.put(curr, indices.get(i));

		}

		ArrayList<Integer> temp = new ArrayList<Integer>();
		for (Map.Entry<String, Integer> entry : map.entrySet()){
            temp.add(entry.getValue());
        }

        this.indices = temp;
	}

	private void handleOrdering(HashMap<String, ArrayList<Integer>> grouped){
		String[] oHeader = query.getOrdering();
		ArrayList<Integer> temp = new ArrayList<Integer>();
		Map<String, Integer> map = null;

		for (Map.Entry<String, ArrayList<Integer>> entry : grouped.entrySet()) {
			map = new TreeMap<String, Integer>();
			ArrayList<Integer> gTemp = entry.getValue();
			for(int i=0; i<gTemp.size();i++){
				String curr = data.get(oHeader[0]).get(gTemp.get(i));
				map.put(curr, gTemp.get(i));

			}
			for (Map.Entry<String, Integer> entryy : map.entrySet()){
            	temp.add(entryy.getValue());
        	}
		}

		this.indices = temp;
	}

	public double getMin(String header) throws Exception{
		ArrayList<String> temp = this.data.get(header);

		if(isNumeric(temp.get(0))){
			double minVal = Double.parseDouble(temp.get(0));
			for(int i=1; i<temp.size();i++){
				double d = Double.parseDouble(temp.get(i));
				if(d < minVal){
					minVal = d;
				}
			}
			return minVal;
		}else{
			throw new Exception("Min value can't be obtained from non-numeric field, " + header);
		}

	}

	public double getMax(String header) throws Exception{
		ArrayList<String> temp = this.data.get(header);

		if(isNumeric(temp.get(0))){
			double maxVal = Double.parseDouble(temp.get(0));
			for(int i=1; i<temp.size();i++){
				double d = Double.parseDouble(temp.get(i));
				if(d > maxVal){
					maxVal = d;
				}
			}
			return maxVal;
		}else{
			throw new Exception("Max value can't be obtained from non-numeric field, " + header);
		}

	}

	public double getAvg(String header) throws Exception{
		ArrayList<String> temp = this.data.get(header);

		if(isNumeric(temp.get(0))){
			double avgVal = Double.parseDouble(temp.get(0));
			for(int i=1; i<temp.size();i++){
				avgVal += Double.parseDouble(temp.get(i));
			}
			return avgVal/temp.size();
		}else{
			throw new Exception("Average value can't be obtained from non-numeric field, " + header);
		}

	}

	public static boolean isNumeric(String str){
  		return str.matches("-?\\d+(\\.\\d+)?");  //match a number with optional '-' and decimal.
	}

}