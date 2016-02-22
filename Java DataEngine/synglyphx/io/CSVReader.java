package synglyphx.io;

import java.io.*;
import java.util.Arrays;
import java.util.ArrayList;
import synglyphx.data.DataFrame;

public class CSVReader {

	private DataFrame data = null;
	private int headers = 0;

	public CSVReader(){

	}

	public void createDataFrame(String path){
		this.data = new DataFrame();
		this.parseCSV(path);
		data.createMinMaxTable();
	}

	private void parseCSV(String path){

		try{
			com.opencsv.CSVReader reader = new com.opencsv.CSVReader(new FileReader(path));
	     	String [] nextLine;
	     	while ((nextLine = reader.readNext()) != null) {
	     		this.data.addRow(CSVtoArrayList(nextLine));
	     	}
     	} catch (IOException re) {
			re.printStackTrace();
		}

	}

	public DataFrame getDataFrame(){
		return data;
	}

	private ArrayList<String> CSVtoArrayList(String[] splitData) {
		ArrayList<String> result = new ArrayList<String>();
		
		if (splitData.length != 0) {
			if (headers != 0 && splitData.length != headers){
				String[] temp = Arrays.copyOf(splitData, splitData.length + headers-splitData.length);
				for (int i = 0; i < headers-splitData.length; i++) {
					temp[splitData.length+i] = " ";
				}
				splitData = temp;
			}else if(headers == 0){headers = splitData.length;}

			for (int i = 0; i < splitData.length; i++) {

				if (!(splitData[i] == null) || !(splitData[i].length() == 0)) {
					result.add(splitData[i].trim().replace("\"",""));
				}
			}
		}
		
		return result;
	}

}
