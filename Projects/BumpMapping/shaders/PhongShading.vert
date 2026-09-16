#version 460 core

layout (location = 0) in vec4 vPosition;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;
uniform float time;

out vec3 FragPos;
out vec3 Normal;

// Function to compute radial wave height and analytical normal
void computeWave(vec3 pos, out vec4 animatedPos, out vec3 normal)
{
    // Radial wave parameters
    float amp = 0.15;
    float k = 15.0;
    float speed = 5.0;
    float alpha = 0.8;

    float r = length(pos.xz);
    r = max(r, 0.0001);

    animatedPos = vec4(pos, 1.0);
    float phase = k * r - speed * time;
    float decay = exp(-alpha * r);
    animatedPos.y = amp * decay * cos(phase);

    // Partial derivatives for the radial analytical normal calculation
    float dy_dr = -amp * decay * (alpha * cos(phase) + k * sin(phase));
    float dy_dx = dy_dr * (pos.x / r);
    float dy_dz = dy_dr * (pos.z / r);
    normal = normalize(vec3(-dy_dx, 1.0, -dy_dz));
}

void main ()
{
    vec4 animatedPos;
    vec3 localNormal;
    
    computeWave(vPosition.xyz, animatedPos, localNormal);

    FragPos = vec3(modelTrans * animatedPos);
    mat3 normalMatrix = transpose(inverse(mat3(modelTrans)));
    Normal = normalMatrix * localNormal;
    
    gl_Position = projection * camera * modelTrans * animatedPos;
}