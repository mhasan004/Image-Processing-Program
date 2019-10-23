#version 330

in	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader

uniform float     u_Brightness;	// threshold value
uniform float     u_Contrast;	// threshold value
uniform	sampler2D u_Sampler;	// uniform variable for the texture image

out	vec4	  fragColor;

void main() 
{
	vec3 clr = texture(u_Sampler, v_TexCoord).rgb;
	float shift = 0.5 + u_Brightness;
	float r = clamp((clr.r - 0.5)*u_Contrast + shift, 0.0, 1.0);
	float g = clamp((clr.g - 0.5)*u_Contrast + shift, 0.0, 1.0);
	float b = clamp((clr.b - 0.5)*u_Contrast + shift, 0.0, 1.0);
	fragColor = vec4(r, g, b, 1.0);
}
