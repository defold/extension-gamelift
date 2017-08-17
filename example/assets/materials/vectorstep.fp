varying mediump vec2 var_texcoord0;
varying mediump float var_rotation;

uniform highp sampler2D vectors_tex;
uniform highp sampler2D vectors_sub_tex;
uniform highp vec4 pdx;

vec2 unpack(vec4 v)
{
    //v = v * 2.0 - 1.0;
    vec2 o = v.rg - v.ba;
    //o = o * 2.0 - 1.0;
    return o;
}

vec4 pack(vec2 v)
{
    //vec2 v = clamp(vi, -1.0, 1.0);
    vec4 o = vec4(v.xy, 0.0, 0.0);
    o.zw = abs(clamp(v.xy, -1.0, 0.0));
    return o;
}

void main()
{
    vec4 vect_c = texture2D(vectors_sub_tex, var_texcoord0.xy);
	vec2 a = unpack(vect_c);
	vec4 vect_c2 = texture2D(vectors_tex, var_texcoord0.xy - a*0.02);
    //vec4 vect_sub_c = texture2D(vectors_sub_tex, var_texcoord0.xy);


    //vec4 vect_out = (vect_c + vect_sub_c) / 2.0;
	vec4 vect_out = vect_c2;
	vect_out = vect_out * 0.995;

    //vect_out = vect_out * 0.9;



    //vec2 vect_sub = unpack(texture2D(vectors_sub_tex, var_texcoord0.xy));

    //vec4 vect_out = pack(unpack(vect_c) * 0.98);

    //vec4 vect_out = vect_c*0.98;// + vect_sub*0.0;//vect_sub * 0.4 + vect * 0.4;
    //vec4 vect_out = pack(vect); //floor((vect*0.1)*255.0) / 255.0;

    // gl_FragColor = pack(vect_out);
    gl_FragColor = vect_out;
}
