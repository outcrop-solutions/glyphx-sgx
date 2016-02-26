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

		BufferedReader reader = null;
		try{
			reader = new BufferedReader(new FileReader(path));

			String line;
			while((line = reader.readLine()) != null && line.length() > 0){
				this.data.addRow(CSVtoArrayList(line));
			}	
		} catch (IOException re) {
			re.printStackTrace();
		} finally {
			try {
				if (reader != null) reader.close();
			} catch (IOException rc) {
				rc.printStackTrace();
			}
		}

	}

	public DataFrame getDataFrame(){
		return data;
	}

	private ArrayList<String> CSVtoArrayList(String csv) {
		ArrayList<String> result = new ArrayList<String>();
		
		if (csv != null) {
			String[] splitData = csv.split(",(?=([^\"]*\"[^\"]*\")*[^\"]*$)", -1);
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

