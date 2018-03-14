package synglyphx.io.web;

import java.util.Map;
import synglyphx.glyph.Node;

public class WebOutputSelector {

	public static WebOutput getWebOutput(Map<Integer,Node> allNodes){

		if(isApplicantGlyph(allNodes)){
			return new ApplicantOutput(allNodes);
		}
		else if(isHighSchoolGlyph(allNodes)){
			return new HighSchoolOutput(allNodes);
		}
		return new StandardOutput(allNodes);
	}

	private static boolean isApplicantGlyph(Map<Integer,Node> allNodes){

		if(allNodes.size() >= 27){
			if(matchNodeScale(allNodes.get(2), 0.3, 0.3, 0.3) && matchNodeScale(allNodes.get(5), 0.3, 0.3, 0.35) &&
				matchNodeScale(allNodes.get(11), 0.3, 0.3, 0.25) && matchNodeScale(allNodes.get(14), 0.15, 0.15, 0.225) &&
				matchNodeScale(allNodes.get(17), 0.3, 0.3, 0.35) && matchNodeScale(allNodes.get(25), 1.0, 1.0, 0.3)){
				return true;
			}
		}
		return false;
	}

	private static boolean isHighSchoolGlyph(Map<Integer,Node> allNodes){

		if(allNodes.size() >= 52){
			if(matchNodeScale(allNodes.get(1), 7.5, 7.5, 2.0) && matchNodeScale(allNodes.get(2), 1.0, 1.0, 0.3) &&
				matchNodeScale(allNodes.get(10), 1.0, 1.0, 0.2) && matchNodeScale(allNodes.get(14), 1.0, 1.0, 0.3) &&
				matchNodeScale(allNodes.get(19), 0.2, 0.04, 1.0) && matchNodeScale(allNodes.get(45), 1.0, 1.0, 0.275)){
				return true;
			}
		}
		return false;
	}

	private static boolean matchNodeScale(Node x, double sx, double sy, double sz){
		System.out.println(String.valueOf(x.getSX()) +": "+ String.valueOf(sx) +", "+String.valueOf(x.getSY()) +": "+ String.valueOf(sy) +", "+String.valueOf(x.getSZ()) +": "+ String.valueOf(sz));
		return x.getSX() == sx && x.getSY() == sy && x.getSZ() == sz;
	}
}