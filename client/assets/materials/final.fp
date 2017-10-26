varying mediump vec4 var_position;
varying mediump vec3 var_normal;
varying mediump vec2 var_texcoord0;
varying mediump vec4 var_light;

uniform lowp sampler2D comp;
uniform lowp sampler2D bloom;
uniform lowp vec4 tint;
uniform lowp vec4 light;

void main()
{
	vec4 comp_color = texture2D(comp, var_texcoord0.xy);
	vec4 bloom_color = texture2D(bloom, var_texcoord0.xy);
    gl_FragColor = vec4(comp_color.rgb*0.8 + bloom_color.rgb*0.8, 1.0);
	//gl_FragColor = vec4(bloom_color.rgb, 1.0);
}

