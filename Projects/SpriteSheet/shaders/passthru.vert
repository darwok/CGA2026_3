#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoords;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

uniform float col;
uniform float row;
uniform float totalCols;
uniform float totalRows;

out vec2 texCoords;

void main ()
{
    // Mapeo básico para avance (WIP)
    texCoords = vec2((vTexCoords.x + col) / totalCols, (vTexCoords.y + row) / totalRows);

    gl_Position = projection * camera * modelTrans * vPosition;
}