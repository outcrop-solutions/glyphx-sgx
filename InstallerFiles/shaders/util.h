
mat3 rotation_matrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c );
}

vec3 get_lighting(vec3 surface_normal)
{
	const vec3 ambient = vec3( 0.2 );
	const float light1_str = 1.0;
	const float light2_str = 0.25;
	const vec3 color1 = vec3( light1_str );
	const vec3 color2 = vec3( light2_str );
	vec3 light_dir = normalize( vec3( 1, 1, -1 ) );
    vec3 light1 = clamp( color1 * dot( surface_normal, -light_dir ), 0, 1 );
    vec3 light2 = clamp( color2 * dot( surface_normal, light_dir ), 0, 1 );
    vec3 light = clamp( ambient + ( light1 + light2 ), 0, 1 );

    return light;
}