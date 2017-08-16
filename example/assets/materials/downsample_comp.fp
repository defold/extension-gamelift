varying highp vec2 var_texcoord0;

uniform highp sampler2D tex0;
uniform highp vec4 pdx;

void main()
{
	/*
	vec4 p00 = texture2D(tex0, var_texcoord0.xy + vec2(-pdx.x, -pdx.y));
	vec4 p10 = texture2D(tex0, var_texcoord0.xy + vec2(   0.0, -pdx.y));
	vec4 p20 = texture2D(tex0, var_texcoord0.xy + vec2( pdx.x, -pdx.y));
	vec4 p01 = texture2D(tex0, var_texcoord0.xy + vec2(-pdx.x,    0.0));
	vec4 p11 = texture2D(tex0, var_texcoord0.xy + vec2(   0.0,    0.0));
	vec4 p21 = texture2D(tex0, var_texcoord0.xy + vec2( pdx.x,    0.0));
	vec4 p02 = texture2D(tex0, var_texcoord0.xy + vec2(-pdx.x,  pdx.y));
	vec4 p12 = texture2D(tex0, var_texcoord0.xy + vec2(   0.0,  pdx.y));
	vec4 p22 = texture2D(tex0, var_texcoord0.xy + vec2( pdx.x,  pdx.y));
	vec4 o = (p00 + p10 + p20 + p01 + p11 + p21 + p02 + p12 + p22) / 9.0;
    gl_FragColor = vec4(o.rgb, 1.0);
    */
    vec4 p = texture2D(tex0, var_texcoord0.xy);
	float d = (p.r + p.g + p.b) / 3.0;
	d *= d*10.0;
	//vec3 c = vec3(d);
	vec3 c = vec3(p.rgb*d);
    gl_FragColor = vec4(c, 0.05);
}

