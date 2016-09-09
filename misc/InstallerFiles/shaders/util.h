
vec3 offset_rotation( vec3 pos, mat3 rot, vec3 offset )
{
    return ( rot * ( pos - offset ) ) + offset;
}

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

float rim(vec3 surface_normal, vec3 eye, float rim_width, float intensity, float focus)
{
    float rimlight = rim_width - dot( -eye, surface_normal );
    rimlight = max( rimlight, 0.0 );
    return smoothstep( 0.1, 1, pow( rimlight, focus ) * intensity );
}

vec3 dir_light(vec3 surface_normal, vec3 light_dir, vec3 light_color)
{
    vec3 ret = vec3(0);
    ret += clamp( light_color * dot( surface_normal, light_dir ), 0, 1 );
    return ret;
}

vec3 get_lighting(vec3 surface_normal, vec3 eye, float intensity0, float intensity1, float rim_scale)
{
	const vec3 ambient = vec3( 0.2 );
	vec3 color1 = vec3( intensity0 );
	vec3 color2 = vec3( intensity1 );
	vec3 light_dir = normalize( vec3( 1, 1, -1 ) );
    vec3 light1 = dir_light( surface_normal, -light_dir, color1 );
    vec3 light2 = dir_light( surface_normal, light_dir, color2 );
    vec3 light = clamp( ambient + ( light1 + light2 ), 0, 1 );

    return vec3( rim_scale * rim( surface_normal, eye, 0.8, 1.2, 1.2 ) ) + light;
}

