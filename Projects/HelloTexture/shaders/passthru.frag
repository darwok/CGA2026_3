#version 460 core

out vec4 fragColor;
in vec2 texCoords;

uniform vec4 color;
uniform sampler2D texture0;

void main()
{
    fragColor = color;
}