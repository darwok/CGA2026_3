#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoords;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

// Configuración genérica para cualquier spritesheet enviada desde C++
uniform float col;
uniform float row;
uniform float totalCols;
uniform float totalRows;
uniform bool flipX;

out vec2 texCoords;

void main ()
{
    float x = vTexCoords.x;
    
    // Invertir coordenadas horizontales si va hacia la izquierda
    if (flipX) 
    {
        x = 1.0 - x;
    }

    // OpenGL mapea el Y=0 en la parte inferior, si tu imagen de spritesheet 
    // se lee convencionalmente (Fila 0 = hasta arriba), necesitamos invertir la fila.
    float invertedRow = (totalRows - 1.0) - row;
    
    // Convertir las coordenadas crudas de (0 a 1) en las coordenadas exactas de recorte del cuadro
    texCoords = vec2((x + col) / totalCols, (vTexCoords.y + invertedRow) / totalRows);

    gl_Position = projection * camera * modelTrans * vPosition;
}