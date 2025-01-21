#version 330

layout (location = 0) in vec3 pos;

out vec3 TextureCoordinates;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
	TextureCoordinates = pos;
	gl_Position = Projection * View * vec4(pos, 1.0);
}