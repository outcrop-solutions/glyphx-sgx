
layout(std140) uniform camera_data
{
	mat4 view, proj;
	vec3 camera_pos;
	vec3 upvec;
	vec2 viewport;
};

layout(std140) uniform global_data
{
	float elapsed_seconds;
	float selection_anim_max_scale;
	float selection_anim_state;
	vec4 tint_color;
};

layout(std140) uniform instance_data
{
	mat4 world[max_elements];
};

layout(std140) uniform bounds
{
	vec4 bound[max_elements];
};

layout(std140) uniform alternate_positions
{
	vec4 alternate_position[max_elements];
};

struct rotation_t
{
	vec4 axis_rate;		// xyz = axis, w = rate
	vec4 center;	// xyz = rotate around
};

layout(std140) uniform animation
{
	rotation_t rotation[max_elements];
};

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
out vec3 nml;
out vec3 eye;
out vec4 frag_color;

vec2 world_pt_to_window_pt( vec3 world_pt )
{
	vec4 pt = vec4( world_pt, 1 );
	vec4 result = ( proj * view ) * pt;
	result.x /= result.w;
	result.y /= result.w;

	result.x += 1;
	result.x *= 0.5;
	result.x *= viewport.x;

	result.y += 1;
	result.y *= 0.5;
	result.y *= viewport.y;

	return result.xy;
}

float compute_max_scale( vec3 eyevec )
{
	// Figure out a maximum scale to animate to that's about desired_screen_size in screen-space.
	// We do this by transforming a line from the center of the object to the edge of its bounding
	// sphere, parallel to the near plane, into screen space, and using that to estimate  how much
	// we'd need to scale the object to make that line desired_screen_size pixels long.
	// (This computation is done on the GPU because each instance will have a different position and
	// different bounds, so we want each one to compute its own max_scale.)
	vec4 b = bound[gl_InstanceID];
	float desired_screen_size = selection_anim_max_scale;
	float max_scale = 1.24;
	vec2 screen_pt0 = world_pt_to_window_pt( b.xyz );
	vec2 screen_pt1 = world_pt_to_window_pt( b.xyz + normalize( cross( eyevec, upvec ) ) * b.w );
	float screen_size = length( screen_pt1 - screen_pt0 ) * 2;
	float ratio = desired_screen_size / screen_size;
	return max( max_scale, ratio );
}

void main()
{
	mat4x4 w = world[gl_InstanceID];

	vec3 rotation_center = rotation[gl_InstanceID].center.xyz;
	vec3 rotation_axis = rotation[gl_InstanceID].axis_rate.xyz;
	float rotation_rate = rotation[gl_InstanceID].axis_rate.w;
	mat3 rotation_mat = rotation_matrix( rotation_axis, elapsed_seconds * rotation_rate );
	vec3 world_pos_noscale = vec3( w * vec4( rotation_mat * position.xyz, 1 ) );
	eye = normalize( world_pos_noscale.xyz - camera_pos );

	float max_scale = compute_max_scale( eye );

	// Animate selection scale and alpha.
	float scale = mix( 1.05, max_scale, selection_anim_state );

	// Fade the alpha up to alpha_max until alpha_crossover, then fade it back down to zero.
	const float alpha_max = 0.8;
	const float alpha_crossover = 0.4;
	float alpha_start = mix( 0, alpha_max, selection_anim_state / alpha_crossover );
	float alpha = selection_anim_state <= alpha_crossover ? alpha_start : mix( alpha_max, 0, ( selection_anim_state - alpha_crossover ) / ( 1.f - alpha_crossover ) );

	nml = normalize( rotation_mat * vec3( inverse( transpose( world[gl_InstanceID] ) ) * vec4( normal, 0 ) ) );

	frag_color = tint_color;
	frag_color.a = alpha;

	vec3 world_pos_scaled = vec3( w * vec4( scale * position.xyz, 1 ) );
	world_pos_scaled = offset_rotation( world_pos_scaled.xyz, rotation_mat, rotation_center );

    gl_Position = proj * ( view * vec4( world_pos_scaled, 1 ) );
}
