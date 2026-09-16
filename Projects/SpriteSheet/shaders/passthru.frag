#version 460 core

out vec4 fragColor;
in vec2 texCoords;

uniform vec4 color;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform float blendFactor;

void main()
{
    vec4 color0 = texture(tex0, texCoords);
    vec4 color1 = texture(tex1, texCoords);
    fragColor = mix(color0, color1, blendFactor);
}