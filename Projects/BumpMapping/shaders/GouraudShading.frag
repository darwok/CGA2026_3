#version 460 core

in vec4 vColor;
in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D tex0; // Diffuse Texture
uniform int useLighting;

void main()
{
    if (useLighting != 0) 
    {
        // Multiply ADS vertex color by diffuse texture
        fragColor = vColor * texture(tex0, texCoords);
    }
    else 
    {
        fragColor = vColor;
    }
}