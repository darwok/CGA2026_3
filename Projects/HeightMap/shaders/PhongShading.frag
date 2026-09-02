#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 texCoords;

uniform vec3 eyePos;
uniform int useLighting;
uniform vec4 solidColor;

uniform sampler2D tex0; // Diffuse Texture

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};
uniform Material myMaterial;

struct Light {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
};
uniform Light myLight;

out vec4 fragColor;

vec4 Ambient(Light light, Material material) 
{
    return light.ambient * material.ambient;
}

vec4 Diffuse(Light light, Material material, vec3 L, vec3 N) 
{
    float nDotL = max(dot(N, L), 0.0);
    if (nDotL > 0.0) 
    {
        return light.diffuse * material.diffuse * nDotL;
    }
    return vec4(0.0);
}

vec4 Specular(Light light, Material material, vec3 R, vec3 V) 
{
    float rDotV = max(dot(R, V), 0.0);
    if (rDotV > 0.0) 
    {
        float spec = pow(rDotV, material.shininess);
        return light.specular * material.specular * spec;
    }
    return vec4(0.0);
}

void main()
{
    if (useLighting != 0) 
    {
        vec3 N = normalize(Normal);
        vec3 L = normalize(myLight.position - FragPos);
        vec3 V = normalize(eyePos - FragPos);
        vec3 R = reflect(-L, N);

        vec4 ambientComponent = Ambient(myLight, myMaterial);
        vec4 diffuseComponent = Diffuse(myLight, myMaterial, L, N);
        vec4 specularComponent = vec4(0.0);
        
        if (dot(N, L) > 0.0) 
        {
            specularComponent = Specular(myLight, myMaterial, R, V);
        }

        vec4 texColor = texture(tex0, texCoords);
        vec4 result = ambientComponent + diffuseComponent + specularComponent;
        
        // El color final se combina multiplicándolo con el de la textura
        result = result * texColor;
        result.a = myMaterial.diffuse.a;
        
        fragColor = clamp(result, 0.0, 1.0);
    } 
    else 
    {
        fragColor = solidColor;
    }
}