#version 460 core

layout (location = 0) in vec4 vPosition;

uniform float time;
uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;
uniform float x, y, amp, fase, fr;

// Struct for Material
struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
uniform Material myMaterial;

// Struct for Light
struct Light {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};
uniform Light myLight;

uniform vec3 eyePos;

// Pass final color to the fragment shader
out vec4 vColor;

// ADS Independent Functions
vec4 Ambient(Light light, Material material) 
{
	return light.ambient * material.ambient;
}

vec4 Diffuse(Light light, Material material, vec3 L, vec3 N) 
{
	float nDotL = dot(N, L);
	
	// Only calculate if the light hits the front of the surface
	if (nDotL > 0.0) 
	{
		return light.diffuse * material.diffuse * nDotL;
	}
	
	return vec4(0.0);
}

vec4 Specular(Light light, Material material, vec3 R, vec3 V) 
{
	float rDotV = dot(R, V);
	
	// Only calculate if the reflection is directed towards the camera
	if (rDotV > 0.0) 
	{
		float spec = pow(rDotV, material.shininess);
		return light.specular * material.specular * spec;
	}
	
	return vec4(0.0);
}

// Function to compute wave height and analytical normal
void computeWave(vec3 pos, out float height, out vec3 normal)
{
	// Calculate total phase incorporating time for dynamic wave movement
	float currentPhase = fase + time * 0.003;
	
	float dx = pos.x - x;
	float dz = pos.z - y;
	float r2 = dx * dx + dz * dz;

	float u = fr * r2 + currentPhase;

	// Height displacement
	height = amp * cos(u);

	// Partial derivatives for analytical normal calculation
	float dydx = -amp * sin(u) * fr * 2.0 * dx;
	float dydz = -amp * sin(u) * fr * 2.0 * dz;

	// Local normal vector
	normal = normalize(vec3(-dydx, 1.0, -dydz));
}

void main ()
{
	vec4 newPosition = vPosition;
	vec3 localNormal;

	// Calculate height and local normal
	computeWave(newPosition.xyz, newPosition.y, localNormal);

	// Calculate world position for accurate lighting
	vec4 worldPos = modelTrans * newPosition;

	// Transform local normal to world space using normal matrix
	mat3 normalMatrix = transpose(inverse(mat3(modelTrans)));
	vec3 N = normalize(normalMatrix * localNormal);
	
	// Calculate Vectors L, V and R
	vec3 L = normalize(myLight.position - vec3(worldPos));
	vec3 V = normalize(eyePos - vec3(worldPos));
	vec3 R = reflect(-L, N);

	// Compute Color
	vec4 ambientComponent = Ambient(myLight, myMaterial);
	vec4 diffuseComponent = Diffuse(myLight, myMaterial, L, N);
	vec4 specularComponent = vec4(0.0);
	
	// Only calculate specular if the surface is lit by the diffuse light
	if (dot(N, L) > 0.0) 
	{
		specularComponent = Specular(myLight, myMaterial, R, V);
	}

	vColor = ambientComponent + diffuseComponent + specularComponent;
	
	// Clamp to keep it within correct color bounds and apply alpha
	vColor = clamp(vColor, 0.0, 1.0);
	vColor.a = myMaterial.diffuse.a;

	// Output final position
	gl_Position = projection * camera * worldPos;
}