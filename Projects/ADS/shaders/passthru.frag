#version 460 core

// Receive the calculated ADS color from the vertex shader
in vec4 vColor;
out vec4 fragColor;

void main()
{
    fragColor = vColor;
}