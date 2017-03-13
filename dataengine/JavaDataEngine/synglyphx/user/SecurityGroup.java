package synglyphx.user;

public class SecurityGroup {

	private int groupId;
	private String groupName;
	private String intstitutionName;
	private String s3Directory;

	public SecurityGroup(int id, String group_name, String inst_name, String s3_dir){
		groupId = id;
		groupName = group_name;
		intstitutionName = inst_name;
		s3Directory = s3_dir;
	}

	public int getId(){
		return groupId;
	}

	public String getGroupName(){
		return groupName;
	}

	public String getInstitutionName(){
		return intstitutionName;
	}

	public String getS3Directory(){
		return s3Directory;
	}

}