#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(6.0f, 2.0f, 6.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;
float movCamera = 0.0f;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(-95.0f, 1.0f, -45.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

bool active;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX =PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0;

float AnimCua01 = 0.0f, rotCua01 = 0.0f, ContAnims01 = 0.0f, rotCua02 = 0.0f, ContAnims02 = 0.0f, rotMece = 0.0f;
float rotPuerta01 = 0.0f, PantallaCo01 = 0.0f, PantallaCo02 = 0.0f;
bool derMece = false, Apagador = false, Puerta01_move = false;

float tiempo;
float rotElices = 0.0f;

float speed;
float speed2;

/*Variables Dron*/
float PosicionX = 0.0f, PosicionZ = 0.0f, AnguloDron = 0.0f, rotDron = 0.0f;

float PosicionY = 0.0f, PapaloteX = 0.0f, PapaloteZ = 0.0f;
bool Derecha = false, PapaloteIzq = false;
// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(5,20,-6),
	glm::vec3(5,20,6),
	glm::vec3(7.5,3,-2.5),
	glm::vec3(0,0,0)
};

glm::vec3 Light1;
glm::vec3 LightP1;

int main()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "ProyectoFinal", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	/*=============== We load shaders / Cargamos Shaders ========================*/
	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");
	Shader Anim("Shaders/anim2.vs", "Shaders/anim2.frag");
	
	/*=============== We load Models / Cargamos Modelos ========================*/
	Model Fachada((char*)"Models/PROYECTO/Fachada.obj");

	/*Objetos*/
	Model Sillon01((char*)"Models/PROYECTO/SillonFlor.obj");
	Model Sillon02((char*)"Models/PROYECTO/SillonTriple.obj");
	Model Sillon03((char*)"Models/PROYECTO/SillonMadera01.obj");
	Model Sillon04((char*)"Models/PROYECTO/SillonMadera02.obj");

	Model Base_TV((char*)"Models/PROYECTO/Base_Tele.obj");
	Model TV((char*)"Models/PROYECTO/Tele.obj");

	Model Cuadro01((char*)"Models/PROYECTO/Cuadro01.obj");
	Model Cuadro02((char*)"Models/PROYECTO/Cuadro02.obj");
	Model Mecedora((char*)"Models/PROYECTO/Mecedora.obj");
	Model Puerta01((char*)"Models/PROYECTO/Puerta01.obj");
	Model Puerta02((char*)"Models/PROYECTO/Puerta02.obj");
	Model Puerta03((char*)"Models/PROYECTO/Puerta03.obj");

	Model Agua((char*)"Models/PROYECTO/Picina.obj");
	Model Papalote((char*)"Models/PROYECTO/Papalote.obj");

	Model Dron((char*)"Models/PROYECTO/Dron.obj");
	Model Elices((char*)"Models/PROYECTO/Elices.obj");
	// Build and compile our shader program

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1,&skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	// Load Skybox Textures / Carga las Texturas de Skybox
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/Proy/right.tga");
	faces.push_back("SkyBox/Proy/left.tga");
	faces.push_back("SkyBox/Proy/top.tga");
	faces.push_back("SkyBox/Proy/bottom.tga");
	faces.push_back("SkyBox/Proy/back.tga");
	faces.push_back("SkyBox/Proy/front.tga");
	
	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();



		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 1.0f, 1.0f, 1.0f);



		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.009f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.008f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.009f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.008f);


		glm::vec3 lightColor;
		lightColor.x = abs(Light1.x);
		lightColor.y = abs(Light1.y);
		lightColor.z = abs(Light1.z);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 4.4f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 3.2f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.8f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.9f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.32f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); //Temp



		/*==================={ Load Models / Carga Modelos }=====================*/

		//---Fachada
		view = camera.GetViewMatrix();
		glm::mat4 model(1);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Fachada.Draw(lightingShader);


		//--Objetos / Objects
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Base_TV.Draw(lightingShader);

		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		TV.Draw(lightingShader);
	

		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sillon01.Draw(lightingShader);

		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sillon02.Draw(lightingShader);

		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sillon03.Draw(lightingShader);

		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sillon04.Draw(lightingShader);


		/*==================={ Simple Animations /Animaciones Simples }=====================*/
		model = glm::mat4(1);

		model = glm::translate(model, glm::vec3(6.65f, 1.366f, -4.844f));
		model = glm::translate(model, glm::vec3(0.0f, AnimCua01, 0.0f));
		model = glm::rotate(model, glm::radians(rotCua01), glm::vec3(1.0f, 0.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cuadro01.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(8.942f, 1.725f, -4.847));
		model = glm::rotate(model, glm::radians(rotCua02), glm::vec3(0.0f, 0.0f, 1.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cuadro02.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(8.906f, 0.265f, -1.995f));
		model = glm::rotate(model, glm::radians(rotMece), glm::vec3(0.0f, 0.0f, 1.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Mecedora.Draw(lightingShader);

		/* ================= Interacciones  =====================*/

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(5.047f, 1.04f, 2.388f));
		model = glm::rotate(model, glm::radians(rotPuerta01), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Puerta01.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(5.897f, 2.786f, -4.938f));
		model = glm::rotate(model, glm::radians(-rotPuerta01), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Puerta02.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(3.604f, 2.8f, -4.934f));
		model = glm::rotate(model, glm::radians(rotPuerta01), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Puerta03.Draw(lightingShader);

		/*=============== Complex animation / Animacione Complejas 01  =================*/

		model = glm::mat4(1);
		glm::mat4 Auxiliar(1);
		
		/* Hierarchy / Jerarquizacion */
		Auxiliar = model = glm::translate(model, glm::vec3(PosicionX, 2.5f, PosicionZ));
		Auxiliar = model = glm::rotate(model, glm::radians(rotDron), glm::vec3(0.0f, 1.0f, 0.0));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Dron.Draw(lightingShader);

		model = glm::translate(Auxiliar, glm::vec3(0.066f, 0.015f, 0.052f));
		model = glm::rotate(model, glm::radians(rotElices), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Elices.Draw(lightingShader);

		model = glm::translate(Auxiliar, glm::vec3(0.066f, 0.015f, -0.052f));
		model = glm::rotate(model, glm::radians(rotElices), glm::vec3(0.0f, 1.0f, 0.0));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Elices.Draw(lightingShader);

		model = glm::translate(Auxiliar, glm::vec3(-0.066f, 0.015f, -0.052f));
		model = glm::rotate(model, glm::radians(rotElices), glm::vec3(0.0f, 1.0f, 0.0));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Elices.Draw(lightingShader);

		model = glm::translate(Auxiliar, glm::vec3(-0.066f, 0.015f, 0.052f));
		model = glm::rotate(model, glm::radians(rotElices), glm::vec3(0.0f, 1.0f, 0.0));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Elices.Draw(lightingShader);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
	
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);


		Anim.Use();
		tiempo = glfwGetTime() * speed;
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)

		modelLoc = glGetUniformLocation(Anim.Program, "model");
		viewLoc = glGetUniformLocation(Anim.Program, "view");
		projLoc = glGetUniformLocation(Anim.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::mat4(1);
		// Dibuja Picina
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(Anim.Program, "time"), tiempo);
		Agua.Draw(Anim);
		

		/*=============== Complex animation 02 / Animacione Complejas 02  =================*/
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(PapaloteX, PosicionY, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Papalote.Draw(Anim);
		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);


		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS); // Set depth function back to default




		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
			LightP1 = glm::vec3(1.0f, 0.0f, 0.0f);
		else
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	/*==========================================
	* =============== Apagador =================
	* ========================================== */

	if (keys[GLFW_KEY_SPACE])
	{
		Apagador = !Apagador;
	}
}
	/*======================================
	* Camera Controls / Control de Camara
	* ====================================== */
void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	// Change the direction of our camera / Altera la direccion de la camara
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos; 

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	/*======================================
	* Camera Controls / Control de Camara
	* ====================================== */
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	/*=======================================================================
	* simple frame animation01 / Animacion01 Simple Picture
	* ======================================================================= */

	if (AnimCua01 >= -1.051f && rotCua01 == 0.0f) {
		
		AnimCua01 -= 0.03f;
	}
	else if (rotCua01  <= 85.0f ) {
		rotCua01 += 3.0f;
	}
	else if (ContAnims01 <= 100.0f ) {
		ContAnims01 += 0.2f;
	}
	else {
		AnimCua01 = 0.0f;
		rotCua01 = 0.0f;
		ContAnims01 = 0.0f;
	}

	/*=======================================================================
	* simple frame animation02 / Animacion02 Simple Picture
	* ======================================================================= */

	if (rotCua02 >= -45.0f) {
		rotCua02 -= 0.5f;
	}
	else if(ContAnims02 <= 100.0f)
	{
		ContAnims02 += 0.2f;
	}
	else {
		rotCua02 = 0.0f;
		ContAnims02 = 0.0f;
	}

	/*==============================================================
	* simple rocking animation03 / Animacion03 Simple Mecedora
	* ============================================================== */
	if (rotMece >= -5.0f && derMece == false) {
		rotMece -= 0.07f;
		if (rotMece < -5.0f) {
			derMece = true;
		}
	}
	else if(rotMece <= 5.0f && derMece == true){
		rotMece += 0.07f;
		if (rotMece > 5.0f) {
			derMece = false;
		}
	}
	
	

	if (Apagador){
		Light1 = glm::vec3(5.0f, 5.0f, 5.0f);
	}
	else{
		Light1 = glm::vec3(0);
	}
	
	/*======================================================
	* door interaction / Interaccion Puertas
	* ====================================================== */

	if (keys[GLFW_KEY_V]) {
		
		if (!Puerta01_move && rotPuerta01 < 90.0f) {
			rotPuerta01 += 1.0f;
			if (rotPuerta01 >= 90.0f) {
				Puerta01_move = true;

			}
		}
		else if (Puerta01_move && rotPuerta01 > 0.0f) {
			rotPuerta01 -= 1.0f;
			if (rotPuerta01 <= 0.0f) {
				Puerta01_move = false;

			}
		}
	}


	speed2 = 0.002f;
	speed = 1.0f;
	

	/*===================================================
	* Drone animation / Animacion Dron 
	* ===================================================*/
	if( rotElices <= 360.0f){
		rotElices += 15.0f;
	}else{
		rotElices = 0.0f;
	}
	
	if (AnguloDron <= 360.0f) {
		AnguloDron += 0.02f;
	}
	else {
		AnguloDron = 0.0f;
		rotDron = 0.0f;
	}


	PosicionX = -2.9f + 1.1f * cos(AnguloDron);
	PosicionZ =  1.5f + 1.1f * sin(AnguloDron);

	/*=======================================================
	* kite animation / Animacion Papalote
	* =======================================================*/
	

	if (PapaloteX  <= 25.0f && Derecha == false) {
		PapaloteX += 0.05f;
		if (PapaloteX > 25.0f) {
			Derecha = true;
		}
	}
	else {
		PapaloteX -= 0.05f;
		if (PapaloteX < 0.0f) {
			Derecha = false;
		}
	}

	PosicionY = 4*sin(0.15 * PapaloteX);
		
}