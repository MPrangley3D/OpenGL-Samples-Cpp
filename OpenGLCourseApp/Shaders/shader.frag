#version 330

in vec4 VertexColor;
in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

struct DirectionalLight 
{
    vec3 Color;
    float AmbientIntensity;
    vec3 Direction;
    float DiffuseIntensity;
};

uniform sampler2D MyTexture;
uniform DirectionalLight MyDirectionalLight;

void main()
{
    vec4 AmbientColor = vec4(MyDirectionalLight.Color, 1.0f) * MyDirectionalLight.AmbientIntensity;

    float DiffuseFactor = max(dot(normalize(Normal), normalize(MyDirectionalLight.Direction)), 0.0f);
    vec4 DiffuseColor = vec4(MyDirectionalLight.Color, 1.0f) * MyDirectionalLight.DiffuseIntensity * DiffuseFactor;

    color = texture(MyTexture, TexCoord) * (AmbientColor + DiffuseColor);
}