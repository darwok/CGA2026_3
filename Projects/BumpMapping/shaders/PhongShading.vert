#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoords;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

out vec3 FragPos;
out vec2 texCoords;
out mat3 TBN;

void main ()
{
    texCoords = vTexCoords;
    FragPos = vec3(modelTrans * vPosition);
    
    // TBN Matrix construction for flat plane
    mat3 normalMatrix = transpose(inverse(mat3(modelTrans)));
    vec3 T = normalize(normalMatrix * vec3(1.0, 0.0, 0.0));
    vec3 B = normalize(normalMatrix * vec3(0.0, 0.0, 1.0));
    vec3 N_base = normalize(normalMatrix * vec3(0.0, 1.0, 0.0));
    TBN = mat3(T, B, N_base);
    
    gl_Position = projection * camera * modelTrans * vPosition;
}