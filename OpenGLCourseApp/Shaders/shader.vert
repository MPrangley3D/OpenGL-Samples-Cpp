#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

out vec4 VertexColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragmentPosition;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * View * Model * vec4(pos,1.0);
    VertexColor = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);

    TexCoord = tex;

    Normal = mat3(transpose(inverse(Model))) * norm;

    FragmentPosition = (Model * vec4(pos, 1.0)).xyz;
}