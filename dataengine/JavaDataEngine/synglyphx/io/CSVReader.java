package synglyphx.io;

import java.io.*;
import java.util.Arrays;
import java.util.ArrayList;
import synglyphx.data.DataFrame;
import synglyphx.util.ErrorHandler;

public class CSVReader {

	private DataFrame data = null;
	private int headers = 0;

	public CSVReader(){}

	public void createDataFrame(String path) throws Exception{
			this.data = new DataFrame();
			this.parseCSV(path);
			data.createMinMaxTable();
			data.createSQLiteTable4CSV(path);
	}

	private void parseCSV(String path) throws Exception{

		InputStreamReader fr = new InputStreamReader(new FileInputStream(path), "UTF-8");
		//System.out.println(fr.getEncoding());
		com.opencsv.CSVReader reader = new com.opencsv.CSVReader(fr);
	    String [] nextLine;

     	while ((nextLine = reader.readNext()) != null) {
     		if(rowNotAllEmpty(nextLine)){
     			this.data.addRow(CSVtoArrayList(nextLine));
     		}
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

	private boolean rowNotAllEmpty(String[] row){

		boolean empty = false;
		for(int i = 0; i < row.length; i++){
			if(!row[i].equals(" ") && !row[i].equals("") && row[i] != null){
				empty = true;
			}
		}
		return empty;
	}

}
