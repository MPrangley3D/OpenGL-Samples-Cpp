#version 330

in vec4 VertexColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragmentPosition;

out vec4 color;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight 
{
    Light Base;
    vec3 Direction;
};

struct PointLight
{
    Light Base;
    vec3 Position;
    float Constant;
    float Linear;
    float Exponent;
};

struct SpotLight
{
    PointLight Base;
    vec3 Direction;
    float Edge;
};

struct Material
{
    float SpecularIntensity;
    float Shininess;
};

uniform int PointLightCount;
uniform int SpotLightCount;

uniform DirectionalLight MyDirectionalLight;
uniform PointLight MyPointLights[MAX_POINT_LIGHTS];
uniform SpotLight MySpotLights[MAX_SPOT_LIGHTS];

uniform sampler2D MyTexture;
uniform Material MyMaterial;
uniform vec3 EyePosition;


vec4 CalculateLightByDirection(Light TheLight, vec3 TheDirection)
{
    vec4 AmbientColor = vec4(TheLight.Color, 1.0f) * TheLight.AmbientIntensity;

    float DiffuseFactor = max(dot(normalize(Normal), normalize(TheDirection)), 0.0f);
    vec4 DiffuseColor = vec4(TheLight.Color, 1.0f) * TheLight.DiffuseIntensity * DiffuseFactor;

    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if(DiffuseFactor > 0.0f)
    {
        vec3 FragToEye = normalize(EyePosition - FragmentPosition);
        vec3 ReflectedVertex = normalize(reflect(TheDirection, normalize(Normal)));

        float SpecularFactor = max(dot(FragToEye, ReflectedVertex), 0.0f);

        if(SpecularFactor > 0.0f)
        {
            SpecularFactor = pow(SpecularFactor, MyMaterial.Shininess);
            SpecularColor = vec4(TheLight.Color * MyMaterial.SpecularIntensity * SpecularFactor, 1.0f);
        }
    }

    return (AmbientColor + DiffuseColor + SpecularColor);
}

vec4 CalculateDirectionalLight()
{
    return CalculateLightByDirection(MyDirectionalLight.Base, MyDirectionalLight.Direction);
}

vec4 CalculatePointLight(PointLight InLight)
{
        vec3 Direction = FragmentPosition - InLight.Position;
        float Distance = length(Direction);
        Direction = normalize(Direction);


        vec4 PointColor = CalculateLightByDirection(InLight.Base, Direction);

        // ax^2 + bx + c  (Where Distance == x)
        float Attenuation = InLight.Exponent * Distance * Distance + 
                            InLight.Linear * Distance + 
                            InLight.Constant;
        Attenuation = max(Attenuation, 0.0001); // Avoid division by zero
        return (PointColor / Attenuation);
}

vec4 CalculateSpotLight(SpotLight InSpot)
{
    vec3 RayDirection = normalize(FragmentPosition - InSpot.Base.Position);
    float SpotFactor = dot(RayDirection, InSpot.Direction);

    if(SpotFactor > InSpot.Edge)
    {
        vec4 SpotColor = CalculatePointLight(InSpot.Base);

        return SpotColor * (1.0f - (1.0f - SpotFactor)*(1.0f / (1.0f - InSpot.Edge)));
    }
    else
    {
        return vec4(0, 0, 0, 0);
    }
}

vec4 CalculatePointLights()
{
    vec4 TotalColor = vec4(0,0,0,0);
    for(int i = 0; i < PointLightCount; i++)
    {
        TotalColor += CalculatePointLight(MyPointLights[i]);
    }

    return TotalColor;
}

vec4 CalculateSpotLights()
{
    vec4 TotalColor = vec4(0, 0, 0, 0);
    for(int i = 0; i < SpotLightCount; i++)
    {
        TotalColor += CalculateSpotLight(MySpotLights[i]);
    }

    return TotalColor;
}

void main()
{
    vec4 FinalColor = CalculateDirectionalLight();
    FinalColor += CalculatePointLights();
    FinalColor += CalculateSpotLights();
    color = texture(MyTexture, TexCoord) * FinalColor;
}