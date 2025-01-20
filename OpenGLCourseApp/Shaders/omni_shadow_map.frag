#version 330

in vec4 FragmentPosition;

uniform vec3 LightPosition;
uniform float FarPlane;

void main()
{
	float Distance = length(FragmentPosition.xyz - LightPosition);
	Distance = Distance / FarPlane;
	gl_FragDepth = Distance;
}