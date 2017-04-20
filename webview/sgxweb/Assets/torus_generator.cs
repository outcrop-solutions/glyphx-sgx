using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class torus_generator
{
    public static GameObject create(float ratio)
    {
        Mesh mesh = null;

        if (torus_meshes_by_ratio.ContainsKey(ratio))
        {
            mesh = torus_meshes_by_ratio[ratio];
        }
        else
        {
            mesh = generate_torus(1.5f, ratio * 1.5f, 8, 16);
            torus_meshes_by_ratio.Add(ratio, mesh);
        }

        var obj = new GameObject("torus " + ratio.ToString());
        obj.AddComponent<MeshFilter>();
        obj.AddComponent<MeshRenderer>();
        MeshFilter mFilter = obj.GetComponent<MeshFilter>();

        mFilter.mesh = mesh;


        return obj;
    }

    static Mesh generate_torus(float seg_radius, float tube_radius, int tubes, int segments)
    {
        int verts = segments * tubes;
        int prims = verts * 2;
        int indices = prims * 3;

        Mesh mesh = new Mesh();

        Vector3[] vertices = new Vector3[verts];
        int[] triangleIndices = new int[indices];

        // size of a segment and a tube
        float seg_size = 2 * Mathf.PI / (float)segments;
        float tube_size = 2 * Mathf.PI / (float)tubes;

        float x, y, z;

        for (int i = 0; i < segments; i++)
        {
            int n = (i + 1) % segments;

            // find current and next segments
            int tube_offset = i * tubes;
            int next_tube_offset = n * tubes;

            for (int j = 0; j < tubes; j++)
            {
                // find next vertex offset
                int m = (j + 1) % tubes;

                int iv1 = tube_offset + j;
                int iv2 = tube_offset + m;
                int iv3 = next_tube_offset + m;
                int iv4 = next_tube_offset + j;

                x = (seg_radius + tube_radius * Mathf.Cos(j * tube_size)) * Mathf.Cos(i * seg_size);
                z = (seg_radius + tube_radius * Mathf.Cos(j * tube_size)) * Mathf.Sin(i * seg_size);
                y = tube_radius * Mathf.Sin(j * tube_size);

                // @todo - port normal vector computation code from C++; we're calling Mesh.RecalculateNormals below
                // but analytical normals would be higher quality

                vertices[iv1] = new Vector3(x, y, z);

                triangleIndices[iv1 * 6] = iv1;
                triangleIndices[iv1 * 6 + 1] = iv2;
                triangleIndices[iv1 * 6 + 2] = iv3;

                triangleIndices[iv1 * 6 + 3] = iv3;
                triangleIndices[iv1 * 6 + 4] = iv4;
                triangleIndices[iv1 * 6 + 5] = iv1;
            }
        }
        mesh.vertices = vertices;
        mesh.triangles = triangleIndices;

        mesh.RecalculateBounds();
        mesh.RecalculateNormals();

        return mesh;
    }

    static Dictionary<float, Mesh> torus_meshes_by_ratio = new Dictionary<float, Mesh>();
}
