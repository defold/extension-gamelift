varying mediump vec2 var_texcoord0;
varying mediump vec2 var_texcoord1;
varying mediump float var_rotation;

uniform lowp sampler2D normal_tex;
uniform lowp sampler2D diffuse_tex;
uniform lowp vec4 tint;

vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}

void main()
{
    // Pre-multiply alpha since all runtime textures already are
	vec4 tint_pm = vec4(tint.xyz * tint.w, tint.w);

	if (tint_pm.w < 0.0)
	{
		discard;
	}

	vec4 normal = texture2D(normal_tex, var_texcoord0.xy);
	normal = normal * 2.0 - 1.0;
	vec2 normal_rotated = rotate(normal.xy, var_rotation - 3.14*0.5) * 0.01;

    vec4 color = texture2D(diffuse_tex, var_texcoord1.xy + normal_rotated);
	gl_FragColor = vec4(color.rgb, 1.0);
}
