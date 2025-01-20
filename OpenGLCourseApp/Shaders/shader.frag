#version 330

in vec4 VertexColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragmentPosition;
in vec4 DirectionalLightSpacePosition;

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

struct OmniShadowMap
{
    samplerCube ShadowMapCube;
    float FarPlane;
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
uniform sampler2D DirectionalShadowMap;
uniform Material MyMaterial;
uniform vec3 EyePosition;

uniform OmniShadowMap OmniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

vec3 SampleDisk[20] = vec3[]
(
   vec3(1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3(1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3(1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3(1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3(0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);


float CalculateDirectionalShadowFactor(DirectionalLight Light)
{
    vec3 ProjectedCoords = DirectionalLightSpacePosition.xyz / DirectionalLightSpacePosition.w;
    // Map "-1 to +1" to "0 to +1"
    ProjectedCoords = (ProjectedCoords * 0.5) + 0.5;

    float CurrentDepth = ProjectedCoords.z;

    vec3 MyNormal = normalize(Normal);
    vec3 LightDirection = normalize(Light.Direction);

    float Bias = max(0.005 * (1 - dot(MyNormal, LightDirection)), 0.005);

    float Shadow = 0.0;

    vec2 TexelSize = 1.0 / textureSize(DirectionalShadowMap, 0);
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            float PCF_Depth = texture(DirectionalShadowMap, ProjectedCoords.xy + vec2(x, y) * TexelSize).r;
            Shadow += CurrentDepth - Bias > PCF_Depth ? 1.0 : 0.0;
        }
    }

    Shadow /= 9.0f;

    if(ProjectedCoords.z > 1.0)
    {
        Shadow = 0.0;
    }

    return Shadow;
}

vec4 CalculateLightByDirection(Light TheLight, vec3 TheDirection, float ShadowFactor)
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

    return (AmbientColor + (1.0 - ShadowFactor) * (DiffuseColor + SpecularColor));
}

float CalculateOmniShadowFactor(PointLight InLight, int ShadowIndex)
{
    vec3 FragmentToLight = FragmentPosition - InLight.Position;
    float CurrentDepth = length(FragmentToLight);

    float Shadow = 0.0;
    float Bias = 0.05;
    float Samples = 20;

    float ViewDistance = length(EyePosition - FragmentPosition);
    float DiskRadius = (1.0 + (ViewDistance / OmniShadowMaps[ShadowIndex].FarPlane)) / 25.0;

    for(int i = 0; i < Samples; i++)
    {
        float ClosestDepth = texture(OmniShadowMaps[ShadowIndex].ShadowMapCube, FragmentToLight + SampleDisk[i] * DiskRadius).r;
        ClosestDepth *= OmniShadowMaps[ShadowIndex].FarPlane;
        if(CurrentDepth - Bias > ClosestDepth)
        {
            Shadow += 1.0;
        }
    }

    Shadow /= float(Samples);
    return Shadow;
}


vec4 CalculateDirectionalLight()
{
    float ShadowFactor = CalculateDirectionalShadowFactor(MyDirectionalLight);
    return CalculateLightByDirection(MyDirectionalLight.Base, MyDirectionalLight.Direction, ShadowFactor);
}

vec4 CalculatePointLight(PointLight InLight, int ShadowIndex)
{
        vec3 Direction = FragmentPosition - InLight.Position;
        float Distance = length(Direction);
        Direction = normalize(Direction);

        float ShadowFactor = CalculateOmniShadowFactor(InLight, ShadowIndex);

        vec4 PointColor = CalculateLightByDirection(InLight.Base, Direction, ShadowFactor);

        // ax^2 + bx + c  (Where Distance == x)
        float Attenuation = InLight.Exponent * Distance * Distance + 
                            InLight.Linear * Distance + 
                            InLight.Constant;
        Attenuation = max(Attenuation, 0.0001); // Avoid division by zero
        return (PointColor / Attenuation);
}

vec4 CalculateSpotLight(SpotLight InSpot, int ShadowIndex)
{
    vec3 RayDirection = normalize(FragmentPosition - InSpot.Base.Position);
    float SpotFactor = dot(RayDirection, InSpot.Direction);

    if(SpotFactor > InSpot.Edge)
    {
        vec4 SpotColor = CalculatePointLight(InSpot.Base, ShadowIndex);

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
        TotalColor += CalculatePointLight(MyPointLights[i], i);
    }

    return TotalColor;
}

vec4 CalculateSpotLights()
{
    vec4 TotalColor = vec4(0, 0, 0, 0);
    for(int i = 0; i < SpotLightCount; i++)
    {
        TotalColor += CalculateSpotLight(MySpotLights[i], i + PointLightCount);
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