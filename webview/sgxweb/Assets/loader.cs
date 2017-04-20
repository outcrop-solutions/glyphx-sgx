using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(scene))]
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
        data.topo = (topology)r.read_byte();
        data.ratio = r.read_float();
        data.rotation_rates = r.read_vec3();

        bool parent_found = data.parent_id != 0 && GetComponent<scene>().glyphs.ContainsKey(data.parent_id);
        data.is_root = !(data.parent_id != 0 && parent_found);
        Debug.Assert(data.parent_id == 0 || parent_found, "read_glyph: glyph has parent id that is not in scene");

        // strings
        data.tag = r.read_string();
        data.url = r.read_string();
        data.desc = r.read_string();

        return data;
    }

    void read_link(reader r)
    {
        // @todo
    }

    GameObject create_shape(ref creation_data data)
    {
        GameObject obj = null;
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
            case geom_type.TORUS:
                obj = torus_generator.create(data.ratio); break;
            default:
                obj = GameObject.Instantiate(MissingMeshPrefab); break;
        }
        return obj;
    }

    void place_root(ref creation_data data, Transform obj)
    {
        Matrix4x4 local = Matrix4x4.identity;

        local = matrix.translate(local, data.pos);
        local = matrix.scale(local, data.scale);
        local = apply_antz_rotation(local, data.rot);

        if (data.topo == topology.kNPtopoRod)
        {
            //visual_scale = glm::vec3(data.ratio * 2.0f, data.ratio * 2.0f, data.scale.z * 5.f);
            local = matrix.translate(local, new Vector3(0.0f, 0.0f, 1.0f));
        }
        else
        {
            // @todo - is this right?
            local = matrix.scale(local, data.scale);
        }

        if ((data.geom == geom_type.CYLINDER || data.geom == geom_type.WIRE_CYLINDER)
            || data.geom == geom_type.CONE || data.geom == geom_type.WIRE_CONE
            || (data.topo != topology.kNPtopoPin
                && (data.geom == geom_type.PIN
                    || data.geom == geom_type.WIRE_PIN)))
        {

            local = matrix.translate(local, new Vector3(0.0f, 0.0f, -1.0f));
        }

        matrix.set_from_matrix(obj.transform, local);
        //glyphnode->setCachedTransform(transform);
        //glyphnode->setVisualScale(visual_scale);
        //++root_count;
    }

    Matrix4x4 apply_antz_rotation(Matrix4x4 m, Vector3 antz_rot)
    {
        m = matrix.rotate(m, Mathf.Deg2Rad * antz_rot.y, new Vector3(0.0f, 0.0f, -1.0f));
        m = matrix.rotate(m, Mathf.Deg2Rad * antz_rot.x, new Vector3(-1.0f, 0.0f, 0.0f));
        m = matrix.rotate(m, Mathf.Deg2Rad * antz_rot.z, new Vector3(0.0f, 0.0f, -1.0f));
        return m;
    }

    // @todo - put these somewhere more sensible
    float s_offsetUnit = 1.0f;
    float s_offsetPin = 5.0f;
    float s_offsetRod = 10.0f;
    float s_offsetCube = 0.7071068f;
    float s_offsetTorus = 0.75f;

    void apply_non_pin_rod_offset(ref creation_data data, Transform t, glyph parent)
    {

        if (data.topo == topology.kNPtopoCube)
            t.Translate(new Vector3(0.0f, 0.0f, s_offsetCube));
        else if (data.topo == topology.kNPtopoTorus)
        {
            if (parent.data.topo == topology.kNPtopoTorus)
                t.Translate(new Vector3(0.0f, 0.0f, s_offsetTorus));
        }
        else if (parent.data.topo == topology.kNPtopoPin)
            t.Translate(new Vector3(0.0f, 0.0f, s_offsetUnit));
    }

    void apply_final_offset(ref creation_data data, Transform t)
    {
        if ((data.geom == geom_type.CYLINDER || data.geom == geom_type.CONE)
            || ((data.topo == topology.kNPtopoRod) && (data.geom == geom_type.PIN)))
        {

            t.Translate(new Vector3(0.0f, 0.0f, -s_offsetUnit));
        }
    }

    void apply_final_child_offset(ref creation_data data, Transform t, out Vector3 visual_scale)
    {
        visual_scale = Vector3.one;

        if (data.topo != topology.kNPtopoRod && (data.geom != geom_type.PIN))
        {
            t.Translate(new Vector3(0.0f, 0.0f, s_offsetPin));
        }
        else if (data.topo == topology.kNPtopoRod)
        {                   //width uses ratio, length uses scale 2 * 5 = 10
            visual_scale = new Vector3(data.ratio * 2.0f, data.ratio * 2.0f, data.scale.z * s_offsetPin);
            t.Translate(new Vector3(0.0f, 0.0f, s_offsetUnit));
        }

        apply_final_offset(ref data, t);
    }

    void place_child(ref creation_data data, GameObject obj, glyph parent)
    {
        switch (parent.data.topo)
        {
            case topology.kNPtopoTorus:
                {
                    var scale = data.scale;
                    var pos = data.pos;
                    var antz_rot = data.rot;
                    Matrix4x4 local = Matrix4x4.identity;// matrix.from_transform(obj.transform.parent);

                    if ((parent.data.geom == geom_type.CONE) || (parent.data.geom == geom_type.CYLINDER) || (parent.data.geom == geom_type.PIN))
                    {
                        local = matrix.translate(local, new Vector3(0.0f, 0.0f, 1.0f));
                    }

                    //scale up 1.5X to match torus geometry size inner radius (?)
                    float s_torusRadius = 1.5f;
                    local = matrix.scale(local, new Vector3(s_torusRadius, s_torusRadius, s_torusRadius));

                    //position at torus outer radius, inside center of tube

                    local = matrix.rotate(local, pos.x + 90.0f, new Vector3(0.0f, 0.0f, 1.0f));
                    local = matrix.translate(local, new Vector3(1.0f, 0.0f, 0.0f));

                    if (data.topo == topology.kNPtopoTorus)//dynamic_cast <const ANTzTorusPlacementPolicy* const> (childGlyph->getPlacementPolicy()) != nullptr)
                    {
                        local = matrix.rotate(local, 90.0f, new Vector3(1.0f, 0.0f, 0.0f));
                        local = matrix.rotate(local, -90.0f, new Vector3(0.0f, 0.0f, 1.0f));
                        local = matrix.rotate(local, pos.y, new Vector3(0.0f, 0.0f, 1.0f));
                                                                                                     //translate.z based on scale.x and is converted from deg 57.29...
                        local = matrix.translate(local, new Vector3(0.0f, 0.1591f * scale.x * scale.z / 57.29578f, 0.0f));
                    }
                    else
                    {
                        local = matrix.rotate(local, 90.0f, new Vector3(0.0f, 1.0f, 0.0f));
                        local = matrix.rotate(local, pos.y, new Vector3(0.0f, -1.0f, 0.0f)); //latitude

                        local = matrix.rotate(local, 90.0f, new Vector3(0.0f, 0.0f, 1.0f));               //orient North
                                                                                                                   //translate 1 unit to surface then convert node z local units
                                                                                                                   //uses parent->ratio for torus inner radius and 1.5f for scaling factor
                        local = matrix.translate(local, new Vector3(0.0f, 0.0f, parent.data.ratio + (pos.z / 360.0f)));

                        local = matrix.scale(local, new Vector3(0.5f, 0.5f, 0.5f)); //results in 1/4 parent
                    }
                    //orientation
                    local = apply_antz_rotation(local, antz_rot);

                    //scale 1/2 size of parent, smaller is good for torus .5 / 1.5
                    if (data.topo == topology.kNPtopoRod)        //rod is scaled at draw time and does not pass along scale to children
                        local = matrix.scale(local, new Vector3(0.33333333f, 0.33333333f, 0.33333333f));
                    else
                        local = matrix.scale(local, new Vector3(scale.x * 0.33333333f, scale.y * 0.33333333f, scale.z * 0.33333333f));   //node scale

                    apply_non_pin_rod_offset(ref data, obj.transform, parent);

                    Vector3 visual_scale;
                    apply_final_child_offset(ref data, obj.transform, out visual_scale);
                    // childGlyph->setVisualScale(visual_scale);

                    matrix.set_from_matrix(obj.transform, local);
                }
                break;
        }
    }

    // corresponds to SceneReader::read_glyph_element
    void create_glyph(creation_data data)
    {
        // first instantiate the right prefab and add the glyph component to it
        GameObject obj = create_shape(ref data);
        var gc = obj.AddComponent<glyph>();
        gc.data = data;

        if (data.is_root)   // root elements have their own placement logic
        {
            place_root(ref data, obj.transform);
        }
        else
        {
            var parent = GetComponent<scene>().glyphs[data.parent_id];
            obj.transform.SetParent(parent.transform, false);
            place_child(ref data, obj, parent.GetComponent<glyph>());
        }

        GetComponent<scene>().glyphs.Add(data.id, obj);
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

        var torus = torus_generator.create(0.1f);
        torus.name = "test torus";
    }

    // Update is called once per frame
    void Update()
    {

    }
}
