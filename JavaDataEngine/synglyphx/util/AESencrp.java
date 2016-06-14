package synglyphx.util;

import java.security.*;
import javax.crypto.*;
import javax.crypto.spec.*;
import java.io.*;
import synglyphx.io.Logger;
 
public class AESencrp {

    final protected static char[] hexArray = "0123456789ABCDEF".toCharArray();
    public static String code = "PBEWithMD5AndDES";

    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return new String(hexChars);
    }

    public static byte[] hexToBytes(String s) {
        byte[] b = new byte[s.length() / 2];
        for (int i = 0; i < b.length; i++) {
          int index = i * 2;
          int v = Integer.parseInt(s.substring(index, index + 2), 16);
          b[i] = (byte) v;
        }
        return b;
    }

    public static String encrypt(String value) {

        try{
            SecretKeySpec skeySpec = new SecretKeySpec(code.getBytes("UTF-8"), "AES");
            Cipher cipher = Cipher.getInstance("AES");
            cipher.init(Cipher.ENCRYPT_MODE, skeySpec);
            byte[] encrypted = cipher.doFinal(value.getBytes("UTF-8"));
            return bytesToHex(encrypted);
        }
        catch(Exception e){
            e.printStackTrace();
            try{
                e.printStackTrace(Logger.getInstance().addError());
            }catch(Exception ex){}
        }
        return null;
    }

    public static String decrypt(String value) {

        try{
            SecretKeySpec skeySpec = new SecretKeySpec(code.getBytes("UTF-8"), "AES");
            Cipher cipher = Cipher.getInstance("AES");
            cipher.init(Cipher.DECRYPT_MODE, skeySpec);
            byte[] encrypted = hexToBytes(value);
            byte[] original = cipher.doFinal(encrypted);
            String originalString = new String(original, "UTF-8");
            return originalString;
        }
        catch(Exception e){
            e.printStackTrace();
            try{
                e.printStackTrace(Logger.getInstance().addError());
            }catch(Exception ex){}
        }
        return null;
    }

    public static void main(String[] args) {

        String encrypted = encrypt("Synglyphx_user@9102");
        String decrypted = decrypt(encrypted);
        System.out.println(encrypted);
        System.out.println(decrypted);

    }

}


