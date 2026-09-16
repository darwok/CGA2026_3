#version 460 core

layout (location = 0) in vec4 vPosition;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;
uniform float time;
uniform vec3 eyePos;
uniform int useLighting;
uniform vec4 solidColor;

// Structs for C++ mapping
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

// Pass final color to the fragment shader
out vec4 vColor;

// ADS Independent Functions
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

// Function to compute radial wave height and analytical normal
void computeWave(vec3 pos, out vec4 animatedPos, out vec3 normal)
{
    // Radial wave parameters
    float amp = 0.15;
    float k = 15.0;
    float speed = 5.0;
    float alpha = 0.8;

    float r = length(pos.xz);
    r = max(r, 0.0001);

    animatedPos = vec4(pos, 1.0);
    float phase = k * r - speed * time;
    float decay = exp(-alpha * r);
    animatedPos.y = amp * decay * cos(phase);

    // Partial derivatives for the radial analytical normal calculation
    float dy_dr = -amp * decay * (alpha * cos(phase) + k * sin(phase));
    float dy_dx = dy_dr * (pos.x / r);
    float dy_dz = dy_dr * (pos.z / r);
    normal = normalize(vec3(-dy_dx, 1.0, -dy_dz));
}

void main ()
{
    vec4 animatedPos;
    vec3 localNormal;

    computeWave(vPosition.xyz, animatedPos, localNormal);
    gl_Position = projection * camera * modelTrans * animatedPos;

    if (useLighting != 0) 
    {
        vec4 worldPos = modelTrans * animatedPos;
        mat3 normalMatrix = transpose(inverse(mat3(modelTrans)));
        vec3 N = normalize(normalMatrix * localNormal);
        
        vec3 L = normalize(myLight.position - vec3(worldPos));
        vec3 V = normalize(eyePos - vec3(worldPos));
        vec3 R = reflect(-L, N);

        vec4 ambientComponent = Ambient(myLight, myMaterial);
        vec4 diffuseComponent = Diffuse(myLight, myMaterial, L, N);
        vec4 specularComponent = vec4(0.0);
        
        // Calculate specular only if surface is lit
        if (dot(N, L) > 0.0) 
        {
            specularComponent = Specular(myLight, myMaterial, R, V);
        }

        vec4 result = ambientComponent + diffuseComponent + specularComponent;
        result.a = myMaterial.diffuse.a;
        vColor = clamp(result, 0.0, 1.0);
    } 
    else 
    {
        vColor = solidColor;
    }
}