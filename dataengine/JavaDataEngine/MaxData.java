
public class MaxData {
	
	public static void main(String[] args){

		int templates = 16;
		int columns = 25;
		int rows1 = 30265;
		int rows2 = 15132;
		int rows3 = 7566;
		double mem1 = 1750000.0 - 150000;
		double mem2 = 950000.0 - 150000;
		double mem3 = 550000.0 - 150000;
		double kb = 3.3042;
		//1600000

		//16
		//25
		//15132
		//950000

		int total_nodes1 = templates*rows1;
		int total_nodes2 = templates*rows2;
		int total_nodes3 = templates*rows3;

		System.out.println(total_nodes1);
		System.out.println(total_nodes2);
		System.out.println(total_nodes3);
		System.out.println(mem1/total_nodes1);
		System.out.println(mem2/total_nodes2);
		System.out.println(mem3/total_nodes3);

		System.out.println(rows1*templates*kb);
		System.out.println(rows2*templates*kb);
		System.out.println(rows3*templates*kb);

		int temps = 128;
		int cols = 146;
		int rows = 334;
		int mem = 282000;

		System.out.println(rows*temps*kb+150000);

		//Equation:
		//Total Memory in Kb = ((row_count*template_count)*3.3042)+150000

	}

}