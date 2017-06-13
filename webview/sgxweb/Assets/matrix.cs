
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class matrix
{
    public static Matrix4x4 rotate(Matrix4x4 input, float angle, Vector3 axis)
    {
        axis.Normalize();
        float rad = Mathf.Deg2Rad * angle;
        float s = Mathf.Sin(rad);
        float c = Mathf.Cos(rad);
        float oc = 1.0f - c;

        Matrix4x4 m = new Matrix4x4();
        m.SetRow(0, new Vector4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0f));
        m.SetRow(1, new Vector4(oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0f));
        m.SetRow(2, new Vector4(oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c));
        m.SetRow(3, new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
        return input * m;
    }

    public static Matrix4x4 translate(Matrix4x4 input, Vector3 distance)
    {
        return input * Matrix4x4.Translate(distance);
    }

    public static Matrix4x4 scale(Matrix4x4 input, Vector3 amount)
    {
        return input * Matrix4x4.Scale(amount);
    }

    public static Matrix4x4 from_transform(Transform t)
    {
        return Matrix4x4.TRS(t.localPosition, t.localRotation, t.localScale);
    }

    public static void set_from_matrix(Transform t, Matrix4x4 m)
    {
        t.localPosition = m.GetColumn(3);
        t.localRotation = Quaternion.LookRotation(
            m.GetColumn(2),
            m.GetColumn(1)
        );
        t.localScale = new Vector3(
            m.GetColumn(0).magnitude,
            m.GetColumn(1).magnitude,
            m.GetColumn(2).magnitude
        );
    }
}
