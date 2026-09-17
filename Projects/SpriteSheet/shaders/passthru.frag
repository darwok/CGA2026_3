#version 460 core

out vec4 fragColor;
in vec2 texCoords;

uniform sampler2D tex0;

void main()
{
    vec4 color = texture(tex0, texCoords);
    
    // Ignorar (recortar) la transparencia alfa
    if(color.a < 0.1) 
    {
        discard;
    }

    fragColor = color;
}