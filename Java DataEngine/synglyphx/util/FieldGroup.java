package synglyphx.util;

import java.util.ArrayList;
import synglyphx.data.SourceDataInfo;

public class FieldGroup {

	private String name;
	private SourceDataInfo csv = null;
	private ArrayList<String> colNames = null;
	private Double min;
	private Double max;
	
	public FieldGroup(SourceDataInfo csv) {
		this.csv = csv;
		colNames = new ArrayList<String>();
	}

	public void setName(String name){
		this.name = name;
	}

	public void setColNames(ArrayList<ArrayList<String>> cn){
		
		for(int i=0; i<cn.size();i++){
			colNames.add(cn.get(i).get(2));
		}

		setMinMax();
	}

	private void setMinMax(){

		boolean first = true;

		for(int i = 0; i < colNames.size(); i++){
			Double ln = Double.parseDouble(csv.getDataFrame().getMinMaxTable().get(colNames.get(i)).get(0));
			Double lx = Double.parseDouble(csv.getDataFrame().getMinMaxTable().get(colNames.get(i)).get(1));
			if(first){
				this.min = ln;
				this.max = lx;
				first = false;
			}else{
				if(ln < this.min){this.min = ln;}
				if(lx > this.max){this.max = lx;}
			}
		}
	}

	public String getName(){
		return name;
	}

	public Double getMin(){
		return min;
	}

	public Double getMax(){
		return max;
	}
}
