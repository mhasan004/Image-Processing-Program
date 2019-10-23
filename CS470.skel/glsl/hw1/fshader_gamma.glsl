#version 330

in	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader
uniform	sampler2D u_Sampler;	// uniform variable for the texture image
uniform float     u_gamma;	// threshold value
out	vec4	  fragColor;

void main() 
{
	vec3 clr = texture(u_Sampler, v_TexCoord).rgb;
	float r = pow(clr.r, u_gamma);
	float g = pow(clr.g, u_gamma);
	float b = pow(clr.b, u_gamma);
	fragColor = vec4(r, g, b, 1.0);
}
