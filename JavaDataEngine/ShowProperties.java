
class ShowProperties {
    public static void main(String[] args) {
        
        String os = System.getProperty("os.name");
        String win = "windows";
        if(os.toLowerCase().contains(win.toLowerCase())){
        	System.out.println("windows");
        }else{
        	System.out.println("not windows");
        }
    }
}