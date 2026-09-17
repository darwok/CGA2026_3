#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoords;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

<<<<<<< Updated upstream
=======
// Generic spritesheet config
>>>>>>> Stashed changes
uniform float col;
uniform float row;
uniform float totalCols;
uniform float totalRows;
<<<<<<< Updated upstream
=======
uniform bool flipX;
>>>>>>> Stashed changes

out vec2 texCoords;

void main ()
{
<<<<<<< Updated upstream
    // Mapeo básico para avance (WIP)
    texCoords = vec2((vTexCoords.x + col) / totalCols, (vTexCoords.y + row) / totalRows);

=======
    float x = vTexCoords.x;
    
    // Invert horizontal coordinates if facing left
    if (flipX) 
    {
        x = 1.0 - x;
    }

    // Direct mapping because Plane.cpp already outputs (0,0) at the top-left
    texCoords = vec2((x + col) / totalCols, (vTexCoords.y + row) / totalRows);

>>>>>>> Stashed changes
    gl_Position = projection * camera * modelTrans * vPosition;
}