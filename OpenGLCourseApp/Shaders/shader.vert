#version 330

layout (location = 0) in vec3 pos;
out vec4 VertexColor;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * Model * vec4(pos,1.0);
    VertexColor = vec4(clamp(pos, 0.0f, 1.0f),1.0f);
}