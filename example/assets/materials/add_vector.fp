varying mediump vec2 var_texcoord0;
varying mediump float var_rotation;

uniform lowp sampler2D normal_tex;
uniform lowp sampler2D diffuse_tex;
uniform lowp vec4 tint;
uniform highp vec4 rotation;

vec2 unpack(vec4 v)
{
	//v = v * 2.0 - 1.0;
	vec2 o = v.xy - v.zw;
	o = o * 2.0 - 1.0;
	return o;
}

vec4 pack(vec2 vi)
{
	vec2 v = clamp(vi, -1.0, 1.0);
	vec4 o = vec4(0.0);
	o.xy = clamp(v.xy, 0.0, 1.0);
	o.zw = abs(clamp(v.xy, -1.0, 0.0));
	return o;
}

vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}

void main()
{
	float movement_delta = clamp(length(rotation.yz) / 10.0, 0.0, 1.0);
	//vec4 normal = texture2D(normal_tex, var_texcoord0.xy);
	vec2 normal = vec2(var_texcoord0.xy) * 2.0 - 1.0;
	vec2 normal_n = normalize(normal);
	vec2 normal_rotated = rotate(normal_n, -rotation.x);
	//normal_rotated = (normal_rotated+1.0)*0.5;

    //vec4 color = texture2D(diffuse_tex, var_texcoord1.xy + normal_rotated);
	float dist = clamp(1.0 - length(normal), 0.0, 1.0) * 0.1;
	vec2 result = normal_rotated.xy*dist*movement_delta;
	//vec4 c_out = vec4(0.0);
	//c_out.xy = clamp(result.xy, 0.0, 1.0);
	//c_out.zw = abs(clamp(result.xy, -1.0, 0.0));
	vec4 c_out = pack(result.xy);
	gl_FragColor = vec4(c_out);
}
