package synglyphx.io.web;

import java.util.Map;
import synglyphx.glyph.Node;

public class WebOutputSelector {

	public static WebOutput getWebOutput(Map<Integer,Node> allNodes){

		if(isFirstSourceGlyph(allNodes)){
			return new FirstSourceOutput(allNodes);
		}
		else if(isAdversityGlyph(allNodes)){
			return new AdversityOutput(allNodes);
		}
		else if(isApplicantGlyph(allNodes)){
			return new ApplicantOutput(allNodes);
		}
		else if(isProspectHighSchoolGlyph(allNodes)){
			return new ProspectHighSchoolOutput(allNodes);
		}
		else if(isHighSchoolGlyph(allNodes)){
			return new HighSchoolOutput(allNodes);
		}
		else if(isReviewCommitteeGlyph(allNodes)){
			return new ReviewCommitteeOutput(allNodes);
		}
		return new StandardOutput(allNodes);
	}

	private static boolean isAdversityGlyph(Map<Integer,Node> allNodes){

		if(allNodes.size() >= 21){
			if(matchNodeScale(allNodes.get(3), 0.4, 0.4, 0.4) && matchNodeScale(allNodes.get(7), 0.25, 0.25, 1.25) &&
				matchNodeScale(allNodes.get(10), 0.1, 0.1, 0.3) && matchNodeScale(allNodes.get(19), 0.25, 0.25, 1.25) &&
				matchNodeScale(allNodes.get(21), 1.0, 1.0, 1.0) && matchNodeScale(allNodes.get(1), 0.0, 0.0, 0.0)){
				return true;
			}
		}
		return false;
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

	private static boolean isProspectHighSchoolGlyph(Map<Integer,Node> allNodes){

		if(allNodes.size() >= 52 && allNodes.size() % 52 != 0){
			if(matchNodeScale(allNodes.get(1), 5.0, 5.0, 1.0) && matchNodeScale(allNodes.get(2), 1.0, 1.0, 0.3) &&
				matchNodeScale(allNodes.get(10), 1.0, 1.0, 0.2) && matchNodeScale(allNodes.get(14), 1.0, 1.0, 0.3) &&
				matchNodeScale(allNodes.get(19), 0.2, 0.04, 1.0) && matchNodeScale(allNodes.get(45), 1.0, 1.0, 0.275)){
				return true;
			}
		}
		return false;
	}

	private static boolean isReviewCommitteeGlyph(Map<Integer,Node> allNodes){

		if(allNodes.size() >= 30){
			if(matchNodeScale(allNodes.get(1), 0.1, 0.1, 0.1) && matchNodeScale(allNodes.get(11), 0.3, 0.3, 0.7) &&
				matchNodeScale(allNodes.get(3), 1.0, 1.0, 1.0) && matchNodeScale(allNodes.get(4), 1.0, 1.0, 1.0) &&
				matchNodeScale(allNodes.get(5), 0.3, 0.3, 0.8) && matchNodeScale(allNodes.get(6), 0.7, 0.7, 2.0)){
				return true;
			}
		}
		return false;
	}

	private static boolean isFirstSourceGlyph(Map<Integer,Node> allNodes){

		if(allNodes.size() >= 50){
			if(matchNodeScale(allNodes.get(1), 1.0, 1.0, 0.4) && matchNodeScale(allNodes.get(2), 1.0, 1.0, 2.5) &&
				matchNodeScale(allNodes.get(17), 1.0, 1.0, 1.0) && matchNodeScale(allNodes.get(22), 1.0, 1.0, 0.8) &&
				matchNodeScale(allNodes.get(31), 1.0, 1.0, 0.8) && matchNodeScale(allNodes.get(35), 1.0, 1.0, 0.8)){
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