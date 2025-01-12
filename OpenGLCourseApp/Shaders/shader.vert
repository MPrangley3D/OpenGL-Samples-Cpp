#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;

out vec4 VertexColor;
out vec2 TexCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * View * Model * vec4(pos,1.0);
    VertexColor = vec4(clamp(pos, 0.0f, 1.0f),1.0f);

    TexCoord = tex;
}