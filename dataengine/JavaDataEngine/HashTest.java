
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class HashTest {
	
	public static long hash_value(String v){
		ByteBuffer buf = ByteBuffer.wrap(v.getBytes());
		buf.order(ByteOrder.LITTLE_ENDIAN);
		return buf.hashCode();
	}

	public static long hash_combine(long seed, String v){
		System.out.println(hash_value(v));
		return hash_value(v) + 0x9e377969 + (seed<<6) + (seed>>>2);
	}

	public static void main(String[] args){

		String id = "9e13c7df-2b8c-43be-b113-0db1bf7f5fec";
		String table = "Merged";
		String field = "City.`Population`";

		System.out.println("Expected output: 2238718627");

		long seed = 0;
		//seed ^= HashTest.hash_combine(seed, id);
		System.out.println(HashTest.hash_combine(seed, id));
		//seed ^= HashTest.hash_combine(seed, table);
		System.out.println(HashTest.hash_combine(seed, table));
		//seed ^= HashTest.hash_combine(seed, field);
		System.out.println(HashTest.hash_combine(seed, field));
	}
}