varying mediump vec4 var_position;
varying mediump vec3 var_normal;
varying mediump vec2 var_texcoord0;
varying mediump vec4 var_light;

uniform lowp sampler2D diffuse;
uniform lowp sampler2D normals;
uniform lowp vec4 tint;
uniform lowp vec4 light;

vec2 unpack(vec4 v)
{
	vec2 o = v.xy - v.zw;
	//o = o * 2.0 - 1.0;
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

void main()
{
    vec4 normal = texture2D(normals, var_texcoord0.xy);
	vec2 norm_vec = unpack(normal);

	vec2 color_norm = norm_vec * 0.5 + 0.5;
	vec4 color_mod = vec4(norm_vec.xy, norm_vec.x + norm_vec.y, 0.0);

	//normal.xy = (normal.xy * 2.0) - 1.0;

	vec4 color = texture2D(diffuse, var_texcoord0.xy - norm_vec.xy*0.1);
	

    gl_FragColor = vec4(color.rgb + color_mod.rgb*1.0, 1.0);
	//gl_FragColor = vec4((norm_vec.xy + 1.0)*0.5, 0.0, 1.0);
}

