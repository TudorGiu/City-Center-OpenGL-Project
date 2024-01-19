#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "Skybox.hpp"

#include <iostream>

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;


// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;
glm::mat4 lightRotation;


// the fog
float fogDensity;
GLint fogDensityLoc;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;


// camera
gps::Camera myCamera(
    glm::vec3(3.7f, gps::FIRST_PERSON_Y, 50.0f),
    glm::vec3(3.7f, gps::FIRST_PERSON_Y, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

gps::CAMERA_MODE cameraMode = gps::FIRST_PERSON;

GLfloat cameraSpeed = 0.3f;

GLboolean pressedKeys[1024];

// woman animation
GLfloat womanRotationAngle;
glm::vec3 womanOrbitPoint(3.15f, 1.08f, -0.4f);

// door animation
bool doorAction = false;
bool doorOpened = false;
GLfloat doorRotationAngle;
glm::vec3 doorRotationPoint = glm::vec3(4.15f, 2.0f, 4.22f);

// models
gps::Model3D ground;
gps::Model3D building1;
gps::Model3D building2[2];
gps::Model3D building3;
gps::Model3D building4;
gps::Model3D building5[3];
gps::Model3D building6[2];
gps::Model3D pteranodon;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D woman;
gps::Model3D pole1;
gps::Model3D pole2;
gps::Model3D walls;
gps::Model3D club_floor;
gps::Model3D podium;
gps::Model3D ceiling;
gps::Model3D door_frame;
gps::Model3D nightstand[2];
gps::Model3D couch1[2];
gps::Model3D couch2[2];
gps::Model3D door;

// shaders
gps::Shader myBasicShader;
gps::Shader depthMapShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader skyboxShader;

// skybox
gps::SkyBox mySkyBox;

// shadows
GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap = false;

float lightAngle=0.0f; 

// point light
glm::vec3 pointlightPos; 
GLuint pointLightPosLoc;
int turnOnLight = 0;
glm::vec3 pointLightColor;
GLuint pointLightColorLoc;

// camera animation
bool headUp = false;
bool stop = false;
float distance = 0.0f;
bool animationOn = true;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
    
}

float distanceBetween(const glm::vec3& pos1, const glm::vec3& pos2)
{
    float distance = glm::length(pos1 - pos2);
    return distance;
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        if (cameraMode == gps::FIRST_PERSON)
        {
            printf("Switched to spectator.");
            cameraMode = gps::SPECTATOR;
        }
        else if (cameraMode == gps::SPECTATOR)
        {
            printf("Switched to first person.");
            cameraMode = gps::FIRST_PERSON;
        }

    if (key == GLFW_KEY_T)
    {
        if (distanceBetween(myCamera.getCameraPosition(), doorRotationPoint) < 2.0f)
            doorAction = true;
    }
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (pressedKeys[GLFW_KEY_M]) {
        showDepthMap = !showDepthMap;
    }

    if (pressedKeys[GLFW_KEY_J]) {
        lightAngle -= 1.0f;
       
    }

    if (pressedKeys[GLFW_KEY_K]) {
        lightAngle += 1.0f;
      
    }
    if (pressedKeys[GLFW_KEY_V]) {
        
        lightDir += glm::vec3(0.0f, 1.0f, 0.0f);
    }

    if (pressedKeys[GLFW_KEY_B]) {
        
        lightDir += glm::vec3(0.0f, -1.0f, 0.0f);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }

    if (key == GLFW_KEY_1)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (key == GLFW_KEY_2)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (key == GLFW_KEY_3)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_F]) {
        fogDensity += 0.001f;
        myBasicShader.useShaderProgram();
        glUniform1f(fogDensityLoc, fogDensity);
    }

    if (pressedKeys[GLFW_KEY_G]) {
        fogDensity -= 0.001f;
        myBasicShader.useShaderProgram();
        glUniform1f(fogDensityLoc, fogDensity);
    }

    if (pressedKeys[GLFW_KEY_L]) {
        turnOnLight = 1 - turnOnLight;

        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "turnOnLight"), turnOnLight);
    }

    if (pressedKeys[GLFW_KEY_SPACE]) {
        animationOn = false;
    }
}

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400, lastY = 300;
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.12f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);

    //update view matrix
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void processMovement() {
   
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed, cameraMode);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed, cameraMode);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed, cameraMode);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed, cameraMode);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}


    if (pressedKeys[GLFW_KEY_Q]) {
    
        yaw -= 1;
        myCamera.rotate(pitch, yaw);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E]) {  
        yaw += 1;
        myCamera.rotate(pitch, yaw);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
   
}

void initOpenGLWindow() {
    myWindow.Create(1980, 1080, "Graphic Processing Semester Project");
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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


void initSkybox()
{
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/left.png");
    faces.push_back("skybox/right.png");
    faces.push_back("skybox/top.png");
    faces.push_back("skybox/bottom.png");
    faces.push_back("skybox/back.png");
    faces.push_back("skybox/front.png");
    mySkyBox.Load(faces);
}

void initModels() {
    ground.LoadModel("models/ground/ground.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    lightCube.LoadModel("models/cube/cube.obj");
    woman.LoadModel("models/woman/woman.obj");
    pole1.LoadModel("models/poles/pole1.obj");
    pole2.LoadModel("models/poles/pole2.obj");
    building1.LoadModel("models/building1/building1.obj");
    building2[0].LoadModel("models/building2/building2.obj");
    building2[1].LoadModel("models/building2_concrete/building2_concrete.obj");
    building3.LoadModel("models/building3/building3.obj");
    building4.LoadModel("models/building4/building4.obj");
    building5[0].LoadModel("models/building5/building5.obj");
    building5[1].LoadModel("models/building5_roof/building5_roof.obj");
    building5[2].LoadModel("models/building5_pipes/building5_pipes.obj");
    building6[0].LoadModel("models/building6/building6.obj");
    building6[1].LoadModel("models/building6_concrete/building6_concrete.obj");
    walls.LoadModel("models/walls/walls.obj");
    club_floor.LoadModel("models/floor/floor.obj");
    podium.LoadModel("models/podium/podium.obj");
    ceiling.LoadModel("models/ceiling/ceiling.obj");
    door_frame.LoadModel("models/door_frame/door_frame.obj");
    nightstand[0].LoadModel("models/nightstand/nightstand.obj");
    nightstand[1].LoadModel("models/nightstand/nightstand_glass.obj");
    couch1[0].LoadModel("models/couches/couch1.obj");
    couch1[1].LoadModel("models/couch_legs/couch_legs1.obj");
    couch2[0].LoadModel("models/couches/couch2.obj");
    couch2[1].LoadModel("models/couch_legs/couch_legs2.obj");
    door.LoadModel("models/door/door.obj");
}

void initShaders() {
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	myBasicShader.loadShader("shaders/basic.vert","shaders/basic.frag");
    depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}
void initFBO() {

    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
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

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
        0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = -500.1f, far_plane = 200.0f;
    glm::mat4 lightProjection = glm::ortho(-205.0f, 205.0f, -205.0f, 205.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;
}

void initUniforms() {
	myBasicShader.useShaderProgram();
   
    // create model matrix for teapot
    model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 5000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 10.0f, 10.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));


	//set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // point light
    pointlightPos = glm::vec3(3.7f, gps::FIRST_PERSON_Y, 0.0f);
    pointLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointlightPos");
    glUniform3fv(pointLightPosLoc, 1, glm::value_ptr(pointlightPos));

    //set point light color
    pointLightColor = glm::vec3(1.0f, 0.0f, 0.0f); //white light
    pointLightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor");
    // send point light color to shader
    glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));

    
    // the fog
    fogDensity = 0.0f;
    fogDensityLoc= glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
    glUniform1f(fogDensityLoc, fogDensity);

   
}

void renderObjects(gps::Shader shader, bool depthPass) {

    shader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));


    if (!depthPass)
    {
        //send objects normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
 
    // draw scene
   
   ground.Draw(shader);
   pole1.Draw(shader);
   pole2.Draw(shader);
   building1.Draw(shader);
   building2[0].Draw(shader);
   building2[1].Draw(shader);
   building3.Draw(shader);
   building4.Draw(shader);
   building5[0].Draw(shader);
   building5[1].Draw(shader);
   building5[2].Draw(shader);
   building6[0].Draw(shader);
   building6[1].Draw(shader);
   walls.Draw(shader);
   club_floor.Draw(shader);
   podium.Draw(shader);
   ceiling.Draw(shader);
   door_frame.Draw(shader);
   nightstand[0].Draw(shader);
   nightstand[1].Draw(shader);
   couch1[0].Draw(shader);
   couch1[1].Draw(shader);
   couch2[0].Draw(shader);
   couch2[1].Draw(shader);
   
}

void renderWoman(gps::Shader shader, bool depthPass)
{
    womanRotationAngle -= 1.0f;

    shader.useShaderProgram();

    glm::mat4 translateToOriginMatrix = glm::translate(glm::mat4(1.0f), -womanOrbitPoint);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(womanRotationAngle), glm::vec3(0, 1, 0));
    glm::mat4 translateBackMatrix = glm::translate(glm::mat4(1.0f), womanOrbitPoint);

    glm::mat4 modelMatrix = translateBackMatrix * rotationMatrix * translateToOriginMatrix;

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));


    if (!depthPass)
    {
        //send woman normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    woman.Draw(shader);
}

void renderDoor(gps::Shader shader, bool depthPass)
{
    // opening door
    if (doorAction && (doorOpened == false) && doorRotationAngle > -90.0f)
        doorRotationAngle -= 1.0f;

    if (doorAction && (doorOpened == false) && doorRotationAngle <= -90.0f)
    {
        doorAction = false;
        doorOpened = true;
    }

    // closing door
    if (doorAction && (doorOpened == true) && doorRotationAngle < 0.0f)
        doorRotationAngle += 1.0f;
    if (doorAction && doorRotationAngle >= 0.0f)
    {
        doorAction = false;
        doorOpened = false;
    }


    shader.useShaderProgram();

    glm::mat4 translateToOriginMatrix = glm::translate(glm::mat4(1.0f), -doorRotationPoint);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(doorRotationAngle), glm::vec3(0, 1, 0));
    glm::mat4 translateBackMatrix = glm::translate(glm::mat4(1.0f), doorRotationPoint);

    glm::mat4 modelMatrix = translateBackMatrix * rotationMatrix * translateToOriginMatrix;

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));


    if (!depthPass)
    {
        //send door normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    door.Draw(shader);
}


void renderLightCube(gps::Shader shader) {

    shader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 model = lightRotation;
   
    model = glm::translate(model, 1.0f * (lightDir));
    model = glm::translate(model, glm::vec3(0.0f, 100.0f, 110.0f));
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    lightCube.Draw(shader);

}


void renderSceneToDepthBuffer()
{
    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderObjects(depthMapShader, true);
    renderWoman(depthMapShader, true);
    renderDoor(depthMapShader, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderScene() {
	
    renderSceneToDepthBuffer();

    if (showDepthMap) {
        glViewport(300, 0, 800, 800);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glViewport(0, 0, (float)myWindow.getWindowDimensions().width, (float)myWindow.getWindowDimensions().height);

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


        renderWoman(myBasicShader, false);
        renderDoor(myBasicShader, false);
        renderObjects(myBasicShader, false);
        renderLightCube(lightShader);


        mySkyBox.Draw(skyboxShader, view, projection);
    }
}

void cleanup() {
    myWindow.Delete();
}


void cameraAnimation()
{
    if (stop == false)
    {

        distance += 0.1f;
        myCamera.move(gps::MOVE_MINUS_Z, cameraSpeed/4, cameraMode);

    }
    
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
 

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    
    if (distance < 14.0f)
    {
        yaw -= 0.77f;
        pitch += 0.33f;

    }
    else if (distance < 30.0f)
    {
        yaw += 0.99f;
            if(pitch < 87)
        pitch += 0.33f;
    }
    else if (distance < 40)
    {
        if (yaw > -86)
            yaw -= 0.77f;
        if (pitch > 1)
            pitch -= 0.88f;
    }
    else if (distance < 50)
    {
        yaw += 0.33f;
    }
    else if (distance < 63)
    {
        if (int(distance) == 55)
        {
            doorAction = true;
            turnOnLight = 1;
            myBasicShader.useShaderProgram();
            glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "turnOnLight"), turnOnLight);
        }
        yaw -= 0.33f;
    }
    else if (distance > 63)
    {
        stop = true;
        
        if (headUp)
        {
            pitch += 0.2;
            if (pitch > 1)
                headUp = !headUp;
        }
            
        if (!headUp)
        {
            pitch -= 0.2;
            if (pitch < -20)
                headUp = !headUp;
        }
    }

    myCamera.rotate(pitch, yaw);

    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    
    initOpenGLState();
	initModels();
	initShaders();
    initSkybox();
	initUniforms();
    initFBO();
    setWindowCallbacks();

	glCheckError();

	while (!glfwWindowShouldClose(myWindow.getWindow())) {

        if(animationOn == true)
            cameraAnimation();
         
        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
