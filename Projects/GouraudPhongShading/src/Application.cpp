#include "Application.h"
#include <GLFW/glfw3.h>
#include <vector>
#include "ShaderFuncs.h"
#include <chrono>
#include <iostream>
#include "glm/gtc/type_ptr.hpp"

void Application::setupShaders()
{
	std::string vertexShader = loadTextFile("shaders/passthru.vert");
	std::string fragmentShader = loadTextFile("shaders/passthru.frag");
	programs["passthru"] = InitializeProgram(vertexShader, fragmentShader);

	// Obtemenos la localidad de la variable en los shaders y
	// la guardamos en c++ en nuestro mapa
	uniforms["time"] = glGetUniformLocation(programs["passthru"], "time");
	uniforms["camera"] = glGetUniformLocation(programs["passthru"], "camera");
	uniforms["modelTrans"] = glGetUniformLocation(programs["passthru"], "modelTrans");
	uniforms["projection"] = glGetUniformLocation(programs["passthru"], "projection");

	// Wave parameters uniforms
	uniforms["x"] = glGetUniformLocation(programs["passthru"], "x");
	uniforms["y"] = glGetUniformLocation(programs["passthru"], "y");
	uniforms["amp"] = glGetUniformLocation(programs["passthru"], "amp");
	uniforms["fase"] = glGetUniformLocation(programs["passthru"], "fase");
	uniforms["fr"] = glGetUniformLocation(programs["passthru"], "fr");

	// ADS Uniforms
	uniforms["myLight.ambient"] = glGetUniformLocation(programs["passthru"], "myLight.ambient");
	uniforms["myLight.diffuse"] = glGetUniformLocation(programs["passthru"], "myLight.diffuse");
	uniforms["myLight.specular"] = glGetUniformLocation(programs["passthru"], "myLight.specular");
	uniforms["myLight.position"] = glGetUniformLocation(programs["passthru"], "myLight.position");

	uniforms["myMaterial.ambient"] = glGetUniformLocation(programs["passthru"], "myMaterial.ambient");
	uniforms["myMaterial.diffuse"] = glGetUniformLocation(programs["passthru"], "myMaterial.diffuse");
	uniforms["myMaterial.specular"] = glGetUniformLocation(programs["passthru"], "myMaterial.specular");
	uniforms["myMaterial.shininess"] = glGetUniformLocation(programs["passthru"], "myMaterial.shininess");

	uniforms["eyePos"] = glGetUniformLocation(programs["passthru"], "eyePos");
}

void Application::setup()
{
	// Crear Plano
	plane.createPlane(10);
	plane.cleanMemory();
	geometry["plane"] = plane.vao;

	// Cargar shaders, compilarlos y ligarlos
	setupShaders();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Default wave parameters
	amp = 0.05;
	fr = 25.0;
	fase = 0.0;
	x = 0.0;
	y = 0.0;

	// Setup initial light properties
	myLight.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	myLight.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	myLight.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	myLight.position = glm::vec3(0.0f, 5.0f, 0.0f);

	// Setup initial material properties
	myMaterial.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	myMaterial.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	myMaterial.shininess = 32.0f;
}

void Application::update(GLFWwindow* window)
{
	double currentTime = glfwGetTime();
	if (lastTime == 0.0)
	{
		lastTime = currentTime;
	}
	float deltaTime = static_cast<float>(currentTime - lastTime);
	lastTime = currentTime;
	time = currentTime * 1000.0; // time in ms for shaders

	if (deltaTime > 0.1f) deltaTime = 0.1f;

	// Obtener la posición del cursor y el tamaño de la ventana
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Calcular offsets normalizados respecto al centro [-1.0, 1.0]
	float ndx = 0.0f;
	float ndy = 0.0f;
	if (width > 0 && height > 0)
	{
		ndx = static_cast<float>((xpos - (width / 2.0)) / (width / 2.0));
		ndy = static_cast<float>((ypos - (height / 2.0)) / (height / 2.0));
	}
	ndx = glm::clamp(ndx, -1.0f, 1.0f);
	ndy = glm::clamp(ndy, -1.0f, 1.0f);

	// Mapear a ángulos de rotación de vuelo:
	// - Pitch (cabeceo alrededor de X): cursor abajo -> pitch arriba (rotación positiva)
	// - Roll (alabeo alrededor de Z): cursor derecha -> roll derecha (rotación negativa)
	// - Yaw (guiñada alreadedor de Y): cursor derecha -> guiñada derecha (rotación negativa)
	float targetRoll = -ndx * glm::radians(45.0f);
	float targetPitch = ndy * glm::radians(30.0f);
	float targetYaw = -ndx * glm::radians(35.0f);

	// Suavizado (inercia)
	float lerpFactor = 5.0f * deltaTime;
	if (lerpFactor > 1.0f) lerpFactor = 1.0f;

	currentRoll = glm::mix(currentRoll, targetRoll, lerpFactor);
	currentPitch = glm::mix(currentPitch, targetPitch, lerpFactor);
	currentYaw = glm::mix(currentYaw, targetYaw, lerpFactor);

	// Construir matrices
	modelTrans = glm::mat4(1.0f);
	modelTrans = glm::rotate(modelTrans, currentYaw, glm::vec3(0.0f, 1.0f, 0.0f));
	modelTrans = glm::rotate(modelTrans, currentPitch, glm::vec3(1.0f, 0.0f, 0.0f));
	modelTrans = glm::rotate(modelTrans, currentRoll, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 eye = glm::vec3(0.0f, 2.5f, 2.5f);
	glm::vec3 center = glm::vec3(0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	camera = glm::lookAt(eye, center, up);

	// Store eye position to send it to the shaders for ADS Calculation
	eyePos = eye;

	float aspect = height > 0 ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
	projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void Application::draw()
{
	// Seleccionar programa (shaders)
	glUseProgram(programs["passthru"]);

	// Doy valores a las uniform
	glUniform1f(uniforms["time"], static_cast<float>(time));
	glUniformMatrix4fv(uniforms["camera"], 1, GL_FALSE, glm::value_ptr(camera));
	glUniformMatrix4fv(uniforms["modelTrans"], 1, GL_FALSE, glm::value_ptr(modelTrans));
	glUniformMatrix4fv(uniforms["projection"], 1, GL_FALSE, glm::value_ptr(projection));

	// Send wave parameters
	glUniform1f(uniforms["x"], static_cast<float>(x));
	glUniform1f(uniforms["y"], static_cast<float>(y));
	glUniform1f(uniforms["amp"], static_cast<float>(amp));
	glUniform1f(uniforms["fase"], static_cast<float>(fase));
	glUniform1f(uniforms["fr"], static_cast<float>(fr));

	// Send Light data
	glUniform4fv(uniforms["myLight.ambient"], 1, glm::value_ptr(myLight.ambient));
	glUniform4fv(uniforms["myLight.diffuse"], 1, glm::value_ptr(myLight.diffuse));
	glUniform4fv(uniforms["myLight.specular"], 1, glm::value_ptr(myLight.specular));
	glUniform3fv(uniforms["myLight.position"], 1, glm::value_ptr(myLight.position));

	// Send static Material data and Eye Position
	glUniform4fv(uniforms["myMaterial.ambient"], 1, glm::value_ptr(myMaterial.ambient));
	glUniform4fv(uniforms["myMaterial.specular"], 1, glm::value_ptr(myMaterial.specular));
	glUniform1f(uniforms["myMaterial.shininess"], myMaterial.shininess);
	glUniform3fv(uniforms["eyePos"], 1, glm::value_ptr(eyePos));

	// Seleccionar la geometria (el triangulo/plano)
	glBindVertexArray(geometry["plane"]);

	// 1. Dibujar el plano relleno con color oscuro (azul oscuro)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	myMaterial.diffuse = glm::vec4(0.8f, 0.2f, 1.0f, 1.0f);
	glUniform4fv(uniforms["myMaterial.diffuse"], 1, glm::value_ptr(myMaterial.diffuse));
	glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());

	//// 2. Dibujar la cuadrícula de alambre en color cian brillante con desplazamiento de polígono
	//glEnable(GL_POLYGON_OFFSET_LINE);
	//glPolygonOffset(-1.0f, -1.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_LINE_SMOOTH);
	//glLineWidth(1.5f);
	//myMaterial.diffuse = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
	//glUniform4fv(uniforms["myMaterial.diffuse"], 1, glm::value_ptr(myMaterial.diffuse));
	//glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());
	//glDisable(GL_POLYGON_OFFSET_LINE);
}

Application::~Application()
{
}