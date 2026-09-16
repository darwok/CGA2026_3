#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoords;

uniform float time;
uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

// 
uniform sampler2D tex1; 

out vec2 texCoords;
out float fragLuminosity; 

void main ()
{
    texCoords = vTexCoords;


    vec4 normalColor = texture(tex1, vTexCoords);

 
    float luminosity = 0.2126 * normalColor.r + 0.7152 * normalColor.g + 0.0722 * normalColor.b;


    vec4 displacedPosition = vPosition;

    displacedPosition.y += luminosity * 0.9f; 

 
    fragLuminosity = luminosity;

    gl_Position = projection * camera * modelTrans * displacedPosition;
}