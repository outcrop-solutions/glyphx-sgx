
using System.Collections;
using System.Collections.Generic;

// shape defs from gui/sgxsceneviewer/sceneviewer/scenereader.cpp (and originally from antz)
public enum geom_type
{
    WIRE_CUBE = 0,
    CUBE = 1,
    WIRE_SPHERE = 2,
    SPHERE = 3,
    WIRE_CONE = 4,
    CONE = 5,
    WIRE_TORUS = 6,
    TORUS = 7,
    WIRE_DODECAHEDRON = 8,
    DODECAHEDRON = 9,
    WIRE_OCTAHEDRON = 10,
    OCTAHEDRON = 11,
    WIRE_TETRAHEDRON = 12,
    TETRAHEDRON = 13,
    WIRE_ICOSAHDRON = 14,
    ICOSAHEDRON = 15,
    PIN = 16,       // really?
    WIRE_PIN = 17,  // I mean...really?
    WIRE_CYLINDER = 18,
    CYLINDER = 19,
}

public struct creation_data
{
    public bool is_root;
    public geom_type geom;
    public int topo;
    public int id, parent_id, label, glyph_index;
    public UnityEngine.Color32 color;
    public UnityEngine.Vector3 pos, rot, scale;
    public float ratio;
    public UnityEngine.Vector3 rotation_rates;
}
