#version 460 core

in vec4 vColor;
in vec2 texCoords;

out vec4 fragColor;
uniform sampler2D tex0; // Diffuse Texture

void main()
{
    vec4 texColor = texture(tex0, texCoords);
    fragColor = vColor * texColor;
}