using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class loader : MonoBehaviour
{
    public GameObject ConePrefab;
    public GameObject SpherePrefab;
    public GameObject CylinderPrefab;
    public GameObject CubePrefab;
    public GameObject MissingMeshPrefab;

    void read_base_image(reader r)
    {
        var tex_id = r.read_byte();
        var pos = r.read_vec3();
        var rot = r.read_vec3();    // @todo - to radians?
        var color = r.read_packed_solid_color();
        var grid_w = r.read_float();
        var grid_h = r.read_float();
        var grid_x = r.read_int32();
        var grid_y = r.read_int32();
    }

    creation_data read_glyph(reader r)
    {
        creation_data data;

        data.id = r.read_int32();
        data.label = r.read_int32();
        data.glyph_index = r.read_int32();
        Debug.Assert(data.glyph_index <= 8, "invalid glyph_index, up to 8 glyph types supported");
        data.parent_id = r.read_int32();
        data.pos = r.read_vec3();
        data.rot = r.read_vec3();
        data.scale = r.read_vec3();
        data.color = r.read_packed_color();
        data.geom = (geom_type)r.read_byte();
        data.topo = r.read_byte();
        data.ratio = r.read_float();
        data.rotation_rates = r.read_vec3();
        data.is_root = (data.parent_id == 0) /*|| !parent */;

        // strings
        var tag = r.read_string();
        var url = r.read_string();
        var desc = r.read_string();

        return data;
    }

    void read_link(reader r)
    {
        // @todo
    }

    void create_glyph(creation_data data)
    {
        if (data.is_root)
        {
            GameObject obj;
            switch (data.geom)
            {
                case geom_type.CONE:
                    obj = GameObject.Instantiate(ConePrefab); break;
                case geom_type.SPHERE:
                    obj = GameObject.Instantiate(SpherePrefab); break;
                case geom_type.CYLINDER:
                    obj = GameObject.Instantiate(CylinderPrefab); break;
                case geom_type.CUBE:
                    obj = GameObject.Instantiate(CubePrefab); break;
                default:
                    obj = GameObject.Instantiate(MissingMeshPrefab); break;
            }
            obj.transform.localScale = data.scale;
            obj.transform.position = data.pos;
        }
    }

    // Use this for initialization
    void Start()
    {
        uint FORMAT_VERSION = 2;
        uint SCENE_FILE_MAGIC_NUMBER = 0xa042bc3f;
        uint COUNT_FILE_MAGIC_NUMBER = 0x294ee1ac;

        System.Diagnostics.Debug.WriteLine("starting up");

        TextAsset sgnsrc = Resources.Load("glyphs_sgn") as TextAsset;
        reader sgn = new reader(sgnsrc.bytes);

        var sgn_magic = sgn.read_uint32();
        var sgn_version = sgn.read_uint32();
        var base_image_count = sgn.read_uint32();
        var glyph_count = sgn.read_uint32();
        var link_count = sgn.read_uint32();

        Debug.Assert(sgn_magic == COUNT_FILE_MAGIC_NUMBER, "invalid count file magic number");
        Debug.Assert(sgn_version == FORMAT_VERSION, "invalid count file version number");

        TextAsset sgcsrc = Resources.Load("glyphs_sgc") as TextAsset;
        reader sgc = new reader(sgcsrc.bytes);
        var sgc_magic = sgc.read_uint32();
        var sgc_version = sgc.read_uint32();

        Debug.Assert(sgc_magic == SCENE_FILE_MAGIC_NUMBER, "invalid scene file magic number");
        Debug.Assert(sgc_version == FORMAT_VERSION, "invalid scene file version number");

        for (uint i = 0; i < base_image_count; ++i)
            read_base_image(sgc);
        for (uint i = 0; i < glyph_count; ++i)
        {
            var data = read_glyph(sgc);
            create_glyph(data);
        }
        for (uint i = 0; i < link_count; ++i)
            read_link(sgc);
    }

    // Update is called once per frame
    void Update()
    {

    }
}
