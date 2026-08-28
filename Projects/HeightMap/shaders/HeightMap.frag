#version 460 core

out vec4 fragColor;
in vec2 texCoords;

uniform vec4 color;
uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{
    //fragColor = color;
    fragColor = texture(tex1, texCoords);
}