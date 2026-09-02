#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoords;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;
uniform vec3 eyePos;
uniform int useLighting;
uniform vec4 solidColor;

uniform sampler2D tex1; // HeightMap

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

out vec4 vColor;
out vec2 texCoords;

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

void main ()
{
    texCoords = vTexCoords;
    
    // Read the HeightMap to calculate luminosity
    vec4 heightTexel = texture(tex1, texCoords);
    float L = 0.2126 * heightTexel.r + 0.7152 * heightTexel.g + 0.0722 * heightTexel.b;
    
    vec4 displacedPos = vPosition;
    displacedPos.y = L * 0.2; // Escalamos la altura para hacerla visible, ajustando según se necesite porque si se usan valores muy altos causa que se generen picos muy largos
    
    vec3 localNormal = vec3(0.0, 1.0, 0.0);

    if (useLighting != 0) 
    {
        vec4 worldPos = modelTrans * displacedPos;
        mat3 normalMatrix = transpose(inverse(mat3(modelTrans)));
        vec3 N = normalize(normalMatrix * localNormal);
        
        vec3 L_dir = normalize(myLight.position - vec3(worldPos));
        vec3 V_dir = normalize(eyePos - vec3(worldPos));
        vec3 R_dir = reflect(-L_dir, N);

        vec4 ambientComponent = Ambient(myLight, myMaterial);
        vec4 diffuseComponent = Diffuse(myLight, myMaterial, L_dir, N);
        vec4 specularComponent = vec4(0.0);
        
        if (dot(N, L_dir) > 0.0) 
        {
            specularComponent = Specular(myLight, myMaterial, R_dir, V_dir);
        }

        vec4 result = ambientComponent + diffuseComponent + specularComponent;
        result.a = myMaterial.diffuse.a;
        vColor = clamp(result, 0.0, 1.0);
    } 
    else 
    {
        vColor = solidColor;
    }

    gl_Position = projection * camera * modelTrans * displacedPos;
}