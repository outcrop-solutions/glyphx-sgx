using System.IO;
using System.Collections;
using System.Collections.Generic;

// @todo - this should check endianness of the host platform (BitConverter.IsLittleEndian)
public class reader
{
    static void swap<T>(ref T lhs, ref T rhs)
    {
        T temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    public reader(byte[] in_data)
    {
        data = in_data;
    }

    public byte read_byte()
    {
        var result = data[offset];
        offset += 1;
        return result;
    }

    public int read_int16()
    {
        swap(ref data[offset], ref data[offset + 1]);
        var result = System.BitConverter.ToInt16(data, offset);
        offset += 2;
        return result;
    }

    public int read_int32()
    {
        swap(ref data[offset + 0], ref data[offset + 3]);
        swap(ref data[offset + 1], ref data[offset + 2]);
        var result = System.BitConverter.ToInt32(data, offset);
        offset += 4;
        return result;
    }

    public uint read_uint32()
    {
        swap(ref data[offset + 0], ref data[offset + 3]);
        swap(ref data[offset + 1], ref data[offset + 2]);
        var result = System.BitConverter.ToUInt32(data, offset);
        offset += 4;
        return result;
    }

    public float read_float()
    {
        swap(ref data[offset + 0], ref data[offset + 3]);
        swap(ref data[offset + 1], ref data[offset + 2]);
        var result = System.BitConverter.ToSingle(data, offset);
        offset += 4;
        return result;
    }

    public UnityEngine.Vector3 read_vec3()
    {
        var v0 = read_float();
        var v1 = read_float();
        var v2 = read_float();
        return new UnityEngine.Vector3(v0, v1, v2);
    }

    public UnityEngine.Vector4 read_vec4()
    {
        var v0 = read_float();
        var v1 = read_float();
        var v2 = read_float();
        var v3 = read_float();
        return new UnityEngine.Vector4(v0, v1, v2, v3);
    }

    public UnityEngine.Color32 read_packed_solid_color()
    {
        // @todo - verify this is the correct order
        var r = read_byte();
        var g = read_byte();
        var b = read_byte();
        var a = (byte)255;
        return new UnityEngine.Color32(r, g, b, a);
    }

    public UnityEngine.Color32 read_packed_color()
    {
        // @todo - verify this is the correct order
        var r = read_byte();
        var g = read_byte();
        var b = read_byte();
        var a = read_byte();
        return new UnityEngine.Color32(r, g, b, a); 
    }

    public string read_string()
    {
        var length = read_int16();
        var str = System.Text.Encoding.UTF8.GetString(data, offset, length);
        offset += length;
        return str;
    }

    int offset = 0;
    byte[] data;
}
