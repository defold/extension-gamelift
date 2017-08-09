varying mediump vec4 var_position;
varying mediump vec3 var_normal;
varying mediump vec2 var_texcoord0;
varying mediump vec4 var_light;

uniform lowp sampler2D diffuse;
uniform lowp sampler2D normals;
uniform lowp vec4 tint;
uniform lowp vec4 light;

void main()
{
    // Pre-multiply alpha since all runtime textures already are
    vec4 tint_pm = vec4(tint.xyz * tint.w, tint.w);

    vec4 normal = (texture2D(normals, var_texcoord0.xy) - 0.5) * 2.0 * 0.01;
    vec4 color = texture2D(diffuse, var_texcoord0.xy + normal.xy) * tint_pm;

    gl_FragColor = vec4(color.rgb,1.0);
}

