#version 330

in	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader

uniform int       u_Hsize;	// blur height value
uniform float	  u_Step;
uniform	sampler2D u_Sampler;	// uniform variable for the texture image

out	vec4	  fragColor;

void main() 
{
	vec3 avg = vec3(0.0);
	vec2 tc  = v_TexCoord;
	int  h2  = u_Hsize / 2;
	for(int i=-h2; i<=h2; ++i)
		avg += texture(u_Sampler, vec2(tc.x, tc.y + i*u_Step)).rgb;
	avg = avg / (u_Hsize);
	fragColor = vec4(avg, 1.0);
}
