#if defined (_APPLE_)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Window.h"
#include "Shader.hpp"
#include "SkyBox.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream>

const unsigned int SHADOW_WIDTH = 4*2048;
const unsigned int SHADOW_HEIGHT = 4*2048;
// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 lightRotation;

// light parameters
glm::vec3 lightDir ;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint fogDensityLoc;

GLfloat fogDensity;
GLfloat fogSensitivity = 0.0015;
GLfloat cameraSensitivity = 0.1;
GLfloat lightAngle;
GLfloat dragonRotationAngle = 0.0f;
GLfloat deltaPresentation1, deltaPresentation2 = 0.0f;

// camera
gps::Camera myCamera(
	glm::vec3(-10.0f, 1.0f, 0.0f),
	glm::vec3(10.0f, 0.1f, 11.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));

float cameraSpeed = 0.1f;

bool pressedKeys[1024];
float angleY = 0.0f;


gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D ground;
gps::Model3D scene, dragon, thing, naruto1, naruto2,bridge;

gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader mySkyboxShader;

GLuint shadowMapFBO;

float lastX = 400, lastY = 300;
float  yaw = -90.0f;
float pitch = 0.0f;
float naruto1loc = 0.0;
float naruto2loc = 0.0;
int narutoNr = 0;
static bool isAnimating = false;

bool firstMouse = true;
GLuint depthMapTexture;
const GLfloat near_plane = 1.0f, far_plane = 6.0f;
bool showDepthMap;
int retina_width, retina_height;
double lastFrameTime = glfwGetTime();


//Skybox
gps::SkyBox mySkyBox;
//------------------------------------------------------------
GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = (xpos - lastX) * cameraSensitivity;
	float yoffset = (lastY - ypos) * cameraSensitivity;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

}

void processRotation() {
	myCamera.rotate(pitch, yaw);
	view = myCamera.getViewMatrix();
	myBasicShader.useShaderProgram();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void cameraAnimation(double deltaTime) {
	static float animationTime = 0.0f;
	static int stage = 0;

	if (isAnimating) {
		animationTime += deltaTime;

		switch (stage) {
		case 0: // Move forward for 2.5 seconds
			if (animationTime < 2.0f) {
				myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
				view = myCamera.getViewMatrix();
				myBasicShader.useShaderProgram();
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			}
			else {
				stage++;
				animationTime = 0.0f;
			}
			break;

		case 1: // Move right for 2.5 seconds
			if (animationTime < 2.5f) {
				myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
				view = myCamera.getViewMatrix();
				myBasicShader.useShaderProgram();
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			}
			else {
				stage++;
				animationTime = 0.0f;
			}
			break;

		case 2: // Move left for 5 seconds
			if (animationTime < 3.0f) {
				myCamera.move(gps::MOVE_LEFT, cameraSpeed);
				view = myCamera.getViewMatrix();
				myBasicShader.useShaderProgram();
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			}
			else {
				stage++;
				animationTime = 0.0f;
			}
			break;

		case 3: // Move backward for 2.5 seconds to return near starting position
			if (animationTime < 2.0f) {
				myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
				view = myCamera.getViewMatrix();
				myBasicShader.useShaderProgram();
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			}
			else {
				stage = 0;
				animationTime = 0.0f;
				isAnimating = false; // End animation after completion
			}
			break;
		case 4: // Move backward for 2.5 seconds to return near starting position
			if (animationTime < 2.0f) {
				
					narutoNr += 1;
					if (narutoNr > 2) {
						narutoNr = 2;
					}

					naruto1loc += 0.1;
					if (naruto1loc > 0.4f)
						naruto1loc = 0.4f;
					naruto2loc += 0.1;
					if (naruto2loc > 0.8f)
						naruto2loc = 0.8f;
			}
			else {
				stage = 0;
				animationTime = 0.0f;
				isAnimating = false; // End animation after completion
			}
			break;
		case 5: // Move backward for 2.5 seconds to return near starting position
			if (animationTime < 2.0f) {
				
				narutoNr -= 1;
				if (narutoNr < 0) {
					narutoNr = 0;
				}
				naruto1loc -= 0.1;
				if (naruto1loc < 0.0f)
					naruto1loc = 0.0f;
				naruto2loc -= 0.1;
				if (naruto2loc < 0.0f)
					naruto2loc = 0.0f;
			
			}
			else {
				stage = 0;
				animationTime = 0.0f;
				isAnimating = false; // End animation after completion
			}
			break;
		}

	}
}
void printCameraCoordinates() {
	// Get the camera's position and front direction
	glm::vec3 cameraPos = myCamera.getPosition();
	glm::vec3 cameraTarget = myCamera.getTarget();
	glm::vec3 cameraUp = myCamera.getUpDirection();

	// Print the coordinates
	std::cout << "Camera Position: ("
		<< cameraPos.x << ", "
		<< cameraPos.y << ", "
		<< cameraPos.z << ")" << std::endl;

	std::cout << "Camera Target: ("
		<< cameraTarget.x << ", "
		<< cameraTarget.y << ", "
		<< cameraTarget.z << ")" << std::endl;

	std::cout << "Camera Up: ("
		<< cameraUp.x << ", "
		<< cameraUp.y << ", "
		<< cameraUp.z << ")" << std::endl;
}

void processMovement() {
	//Fog
	if (pressedKeys[GLFW_KEY_F]) {
		fogDensity += fogSensitivity;
		if (fogDensity > 0.28f)
			fogDensity = 0.28f;
		myBasicShader.useShaderProgram();
		glUniform1fv(fogDensityLoc, 1, &fogDensity);
	}
	if (pressedKeys[GLFW_KEY_G]) {
		fogDensity -= fogSensitivity;
		if (fogDensity < 0.0f)
			fogDensity = 0.0f;
		myBasicShader.useShaderProgram();
		glUniform1fv(fogDensityLoc, 1, &fogDensity);
	}
	//camera
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}
	//rotations
// Rotations for the camera
	if (pressedKeys[GLFW_KEY_Q]) {
		yaw -= cameraSensitivity * 10.0f; // Adjust rotation speed if needed
		processRotation(); // Update the camera's view matrix
	}

	if (pressedKeys[GLFW_KEY_E]) {
		yaw += cameraSensitivity * 10.0f; // Adjust rotation speed if needed
		processRotation(); // Update the camera's view matrix
	}

	//anime
	if (pressedKeys[GLFW_KEY_Z]) {
		narutoNr += 1;
		if (narutoNr > 2) {
			narutoNr = 2;
		}

		naruto1loc += 0.1;
		if (naruto1loc > 0.4f)
			naruto1loc = 0.4f;
		naruto2loc += 0.1;
		if (naruto2loc > 0.8f)
			naruto2loc = 0.8f;
	}
	if (pressedKeys[GLFW_KEY_X]) {
		narutoNr -= 1;
		if (narutoNr < 0) {
			narutoNr = 0;
		}
		naruto1loc -= 0.1;
		if (naruto1loc < 0.0f)
			naruto1loc = 0.0f;
		naruto2loc -= 0.1;
		if (naruto2loc < 0.0f)
			naruto2loc = 0.0f;
	}
	
	if (pressedKeys[GLFW_KEY_P]) {
		isAnimating = true;
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}
	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}
	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_3]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_4]) {
		glEnable(GL_SMOOTH);
		glShadeModel(GL_SMOOTH);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void initOpenGLWindow() {
	myWindow.Create(1024, 768, "OpenGL Project Core");
	glfwGetFramebufferSize(myWindow.getWindow(), &retina_width, &retina_height);
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
	glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
	glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
	scene.LoadModel("models/objects/myMaaap.obj");
	bridge.LoadModel("models/objects/bridge.obj");
	dragon.LoadModel("models/objects/myDragon.obj");
	naruto1.LoadModel("models/objects/naruto.obj");
	naruto2.LoadModel("models/objects/naruto.obj");
	lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
}

void initShaders() {
	mySkyboxShader.loadShader(
		"shaders/skybox.vert",
		"shaders/skybox.frag");
	mySkyboxShader.useShaderProgram();

	myBasicShader.loadShader(
		"shaders/shaderStart.vert", 
		"shaders/shaderStart.frag");
	myBasicShader.useShaderProgram();

	lightShader.loadShader(
		"shaders/lightCube.vert", 
		"shaders/lightCube.frag");
	lightShader.useShaderProgram();

	screenQuadShader.loadShader(
		"shaders/screenQuad.vert", 
		"shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();

	depthMapShader.loadShader(
		"shaders/shaderMY.vert", 
		"shaders/shaderMY.frag");
	depthMapShader.useShaderProgram();
}

void initUniforms() {
	myBasicShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 2000.0f);

	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(-12.0f, 7.7f, -2.1f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//fog
	fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
	glUniform1fv(fogDensityLoc, 1, &fogDensity);
}

void initFBO() {
	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &depthMapTexture);

	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {
	glm::vec3 ligthRotation = glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir;
	glm::mat4 lightView = glm::lookAt(ligthRotation, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = glm::ortho(-20.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void animeDragon() {
	// Increment rotation angle
	dragonRotationAngle += 0.6f;
	if (dragonRotationAngle >= 360.0f) {
		dragonRotationAngle -= 360.0f;
	}

	// Handle up-and-down motion
	static float dragonPos = 0.0f; // Starting vertical offset
	static float loc = 0.1f;       // Vertical movement increment

	// Update dragonPos and reverse direction at bounds
	dragonPos += loc;
	if (dragonPos >= 2.5f) {
		loc = -0.009f; // Reverse direction
	}
	if (dragonPos <= 0.0f) {
		loc = 0.009f; // Reverse direction
	}

	// Create transformation matrix for the dragon
	glm::mat4 dragonModel = glm::mat4(1.0f);

	// Translate the dragon to the origin
	dragonModel = glm::translate(dragonModel, glm::vec3(0.93f, -1.77f, -10.73f));

	// Rotate the dragon around its local Y-axis
	dragonModel = glm::rotate(dragonModel, glm::radians(dragonRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	// Translate the dragon back to its original position
	dragonModel = glm::translate(dragonModel, glm::vec3(-0.93f, 1.77f, 10.73f));

	// Apply up-and-down motion
	dragonModel = glm::translate(dragonModel, glm::vec3(0.0f, dragonPos, 0.0f));

	// Send the transformation matrix to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(dragonModel));
}

void animeNaruto1() {
	glm::mat4 naruto1Model = glm::mat4(1.0f);
	naruto1Model = glm::translate(naruto1Model, glm::vec3(0.0f, 0.0f, naruto1loc)); // Move along the Z-axis
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(naruto1Model));
}

void animeNaruto2() {
	glm::mat4 naruto2Model = glm::mat4(1.0f);
	naruto2Model = glm::translate(naruto2Model, glm::vec3(naruto1loc, 0.0f, 0.0)); // Move along the Z-axis
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(naruto2Model));
}

void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();

	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	}

	scene.Draw(shader);
	bridge.Draw(shader);
	animeDragon();
	dragon.Draw(shader);
	animeNaruto1();
	naruto1.Draw(shader);
	animeNaruto2();
	naruto2.Draw(shader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
}

void renderScene() {

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(depthMapShader, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height); // Use the window dimensions
		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		// Bind the depth map texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}

	else {

		// final scene rendering pass (with shadows)
		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myBasicShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myBasicShader, false);

		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);
		mySkyBox.Draw(mySkyboxShader, view, projection);
	}
}

void cleanup() {
	myWindow.Delete();
	glfwTerminate();
}

void initSkyBox() {
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/cube_right.png");
	faces.push_back("skybox/cube_left.png");
	faces.push_back("skybox/cube_up.png");
	faces.push_back("skybox/cube_down.png");
	faces.push_back("skybox/cube_back.png");
	faces.push_back("skybox/cube_front.png");

	mySkyBox.Load(faces);
}

int main(int argc, const char* argv[]) {

	try {
		initOpenGLWindow();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
	initSkyBox();
	setWindowCallbacks();

	glCheckError();

	while (!glfwWindowShouldClose(myWindow.getWindow())) {
		processRotation();
		renderScene();
		processMovement();

		double currentFrameTime = glfwGetTime(); // Get the current time
		double deltaTime = currentFrameTime - lastFrameTime; // Calculate time elapsed since last frame
		lastFrameTime = currentFrameTime; // Update the previous frame time

		if (isAnimating) {
			cameraAnimation(deltaTime);
		}
		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

	return EXIT_SUCCESS;
}
