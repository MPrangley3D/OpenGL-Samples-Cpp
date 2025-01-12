#version 330

in vec4 VertexColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D MyTexture;

void main()
{
    color = texture(MyTexture, TexCoord) * VertexColor;
}