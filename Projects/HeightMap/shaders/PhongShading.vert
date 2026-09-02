#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoords;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

uniform sampler2D tex1; // HeightMap

out vec3 FragPos;
out vec3 Normal;
out vec2 texCoords;

void main ()
{
    texCoords = vTexCoords;
    
    vec4 heightTexel = texture(tex1, texCoords);
    float L = 0.2126 * heightTexel.r + 0.7152 * heightTexel.g + 0.0722 * heightTexel.b;
    
    vec4 displacedPos = vPosition;
    displacedPos.y = L * 0.2; 

    vec3 localNormal = vec3(0.0, 1.0, 0.0);

    FragPos = vec3(modelTrans * displacedPos);
    mat3 normalMatrix = transpose(inverse(mat3(modelTrans)));
    Normal = normalMatrix * localNormal;
    
    gl_Position = projection * camera * modelTrans * displacedPos;
}