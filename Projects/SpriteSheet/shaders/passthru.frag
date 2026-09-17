#version 460 core

out vec4 fragColor;
in vec2 texCoords;

uniform sampler2D tex0;

void main()
{
    vec4 color = texture(tex0, texCoords);
    
<<<<<<< Updated upstream
    // Ignorar (recortar) la transparencia alfa
    if(color.a < 0.1) 
    {
        discard;
    }
=======
    // Ignorar (recortar) todos los pixeles que no tienen color en el png (Alpha Transparency)
//    if(color.a < 0.1) 
//    {
//        discard;
//    }
>>>>>>> Stashed changes

    fragColor = color;
}