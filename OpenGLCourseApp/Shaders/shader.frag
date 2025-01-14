#version 330

in vec4 VertexColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragmentPosition;

out vec4 color;

struct DirectionalLight 
{
    vec3 Color;
    float AmbientIntensity;
    vec3 Direction;
    float DiffuseIntensity;
};

struct Material
{
    float SpecularIntensity;
    float Shininess;
};

uniform sampler2D MyTexture;
uniform DirectionalLight MyDirectionalLight;
uniform Material MyMaterial;
uniform vec3 EyePosition;

void main()
{
    vec4 AmbientColor = vec4(MyDirectionalLight.Color, 1.0f) * MyDirectionalLight.AmbientIntensity;

    float DiffuseFactor = max(dot(normalize(Normal), normalize(MyDirectionalLight.Direction)), 0.0f);
    vec4 DiffuseColor = vec4(MyDirectionalLight.Color, 1.0f) * MyDirectionalLight.DiffuseIntensity * DiffuseFactor;

    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if(DiffuseFactor > 0.0f)
    {
        vec3 FragToEye = normalize(EyePosition - FragmentPosition);
        vec3 ReflectedVertex = normalize(reflect(MyDirectionalLight.Direction, normalize(Normal)));

        float SpecularFactor = max(dot(FragToEye, ReflectedVertex), 0.0f);

        if(SpecularFactor > 0.0f)
        {
            SpecularFactor = pow(SpecularFactor, MyMaterial.Shininess);
            SpecularColor = vec4(MyDirectionalLight.Color * MyMaterial.SpecularIntensity * SpecularFactor, 1.0f);
        }
    }

    color = texture(MyTexture, TexCoord) * (AmbientColor + DiffuseColor + SpecularColor);
}