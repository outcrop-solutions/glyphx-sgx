package synglyphx.data;

public class DataStats {

 	private String type;
 	private String min;
 	private String max;
 	private String avg;
 	private String count;
 	private String distinct;

 	public DataStats(Boolean type,String min,String max,Double avg,int count,int distinct){
 		if(type){
 			this.type = "real";
 		}else{
 			this.type = "text";
 		}
 		this.min = min;
 		this.max = max;
 		if(this.type.equals("text")){
 			this.avg = "N/A";
 		}else{
 			this.avg = String.valueOf(avg);
 		}
 		this.count = String.valueOf(count);
 		this.distinct = String.valueOf(distinct);
 	}

 	public String getType(){
 		return type;
 	}

 	public String getMin(){
 		return min;
 	}

 	public String getMax(){
 		return max;
 	}

 	public String getAverage(){
 		return avg;
 	}

 	public String getCount(){
 		return count;
 	}

 	public String getDistinct(){
 		return distinct;
 	}

}