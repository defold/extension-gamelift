varying mediump vec4 var_position;
varying mediump vec3 var_normal;
varying mediump vec2 var_texcoord0;
varying mediump vec4 var_light;

uniform lowp sampler2D tex0;
uniform lowp vec4 tint;
uniform lowp vec4 light;

void main()
{
	// Pick a coordinate to visualize in a grid
	vec2 coord = gl_FragCoord.xy*0.05; //var_position.xy*50.0;

	// Compute anti-aliased world-space grid lines
	vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
	float line = min(grid.x, grid.y);

	// Just visualize the grid lines directly
	gl_FragColor = vec4(vec3(1.0 - min(line, 1.0))*vec3(0.4), 1.0);
}

