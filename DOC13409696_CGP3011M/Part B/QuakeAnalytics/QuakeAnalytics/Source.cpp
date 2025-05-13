#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <vector> 

using namespace std;

#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>

//GLM
#include <glm.hpp> 
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//Position data methods 
void playerAllPositions();
void player0Positions();
void player1Positions();
void player2Positions();
void player3Positions();
void playerDeathPositions();
void drawTrajectory();
void setGrid();
void colourPallette(int colourCode);
void checkColourArray();

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void doMovement();
void playerSwitcher();


//----SHADERS----
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"out vec4 pointColor;\n" //AKA vertexColor

"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 squares;\n"

"void main()\n"
"{\n"
"gl_Position = projection * view * squares * vec4(position, 1.0f);\n"
"gl_PointSize = 5.0;\n"
"pointColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);\n"
//"ourColor = color;\n" 
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
//"in vec3 ourColor;\n"
"out vec4 color;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"color = vec4(ourColor);\n"
"}\n\0";


//----Camera stuff----
glm::vec3 cameraPos = glm::vec3(415.0f, 754.0f, 3000.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//----Arrays---- 
glm::vec3 playerAllArray[1000000];
glm::vec3 player0Array[1000000];
glm::vec3 player1Array[1000000];
glm::vec3 player2Array[1000000];
glm::vec3 player3Array[1000000];
glm::vec3 playerDeathArray[1000000];
glm::vec3 squaresArray[400 * 400];
int colourArray[400 * 400];

//Time between current frame and last frame
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLfloat red = 0.0f;
GLfloat green = 0.0f;
GLfloat blue = 0.0f;


GLint vertexColorLocation;
GLuint shaderProgram;

bool keys[1024];
int player = -1;

int main()
{

	//Read player positions from file
	playerAllPositions();
	player0Positions();
	player1Positions();
	player2Positions();
	player3Positions();
	playerDeathPositions();

	//Do colour things 
	setGrid();
	checkColourArray();


	//Initalise GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glEnable(GL_PROGRAM_POINT_SIZE);


	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(800, 800, "Quake Analytics", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, keyCallback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, 800, 800);

	//====BUILD AND COMPILE SHADERS==================================================================
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//=============================================================================================
	//Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	GLfloat vertices[] = {
		// First triangle
		2.5f, 2.5f, 2.0f,  // Top Right
		2.5f, -2.5f, 2.0f,  // Bottom Right
		-2.5f, 2.5f, 2.0f,  // Top Left 
		// Second triangle
		2.5f, -2.5f, 2.0f,  // Bottom Right
		-2.5f, -2.5f, 2.0f,  // Bottom Left
		-2.5f, 2.5f, 2.0f   // Top Left
	};

	GLfloat heatMapVertices[] = {
		// First triangle
		5.0f, 5.0f, 5.0f,  // Top Right
		5.0f, -5.0f, 5.0f,  // Bottom Right
		-5.0f, 5.0f, 5.0f,  // Top Left 
		// Second triangle
		5.0f, -5.0f, 5.0f,  // Bottom Right
		-5.0f, -5.0f, 5.0f,  // Bottom Left
		-5.0f, 5.0f, 5.0f   // Top Left
	};


	//----Squares 1---- 
	GLuint VBO[2], VAO[2];
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//----Heat Map squares----
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(heatMapVertices), heatMapVertices, GL_STATIC_DRAW);
	//Position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);



	//----Main Game loop----
	while (!glfwWindowShouldClose(window))
	{
		//Check if any events have been activiated 
		glfwPollEvents();
		doMovement();
		playerSwitcher();

		//Render & clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//View and Projection
		glm::mat4 view;
		glm::mat4 projection;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)800 / (GLfloat)800, 0.1f, 7000.0f);

		//Get View/Location uniform location 
		GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
		GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
		//Pass matrices into shader
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUseProgram(shaderProgram);


		//----Colours---- 
		vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");


		//================================================================================================
		//TRAJECTORY LOOPS
		//================================================================================================


		//----DeathMap Squares---- 
		glBindVertexArray(VAO[1]);
		for (GLuint i = 0; i < 250000; i++)
		{

			colourPallette(colourArray[i]);
			glUniform4f(vertexColorLocation, red, blue, green, 0.0f);
			glm::mat4 squares;
			squares = glm::translate(squares, squaresArray[i]);
			GLuint squaresLocation = glGetUniformLocation(shaderProgram, "squares");
			glUniformMatrix4fv(squaresLocation, 1, GL_FALSE, glm::value_ptr(squares));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}



		switch (player)
		{
			//Draws player 0 trajectory
		case 0:
			for (GLuint i = 0; i < 250000; i++)
			{
				glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 0.0f); //red
				glm::mat4 squares;
				squares = glm::translate(squares, player0Array[i]);
				GLuint squaresLocation = glGetUniformLocation(shaderProgram, "squares");
				glUniformMatrix4fv(squaresLocation, 1, GL_FALSE, glm::value_ptr(squares));
				glDrawArrays(GL_POINTS, 0, 1);

			}
			break;

			//Draws player 1 trajectory
		case 1:
			for (GLuint i = 0; i < 250000; i++)
			{
				glUniform4f(vertexColorLocation, 1.0f, 1.0f, 1.0f, 0.0f); //white 
				glm::mat4 squares;
				squares = glm::translate(squares, player1Array[i]);
				GLuint squaresLocation = glGetUniformLocation(shaderProgram, "squares");
				glUniformMatrix4fv(squaresLocation, 1, GL_FALSE, glm::value_ptr(squares));
				glDrawArrays(GL_POINTS, 0, 1);

			}
			break;

			//Draws player 2 trajectory
		case 2:
			for (GLuint i = 0; i < 250000; i++)
			{
				glUniform4f(vertexColorLocation, 0.0f, 0.0f, 1.0f, 0.0f); //blue
				glm::mat4 squares;
				squares = glm::translate(squares, player2Array[i]);
				GLuint squaresLocation = glGetUniformLocation(shaderProgram, "squares");
				glUniformMatrix4fv(squaresLocation, 1, GL_FALSE, glm::value_ptr(squares));
				glDrawArrays(GL_POINTS, 0, 1);

			}
			break;

			//Draws player 3 trajectory
		case 3:
			for (GLuint i = 0; i < 250000; i++)
			{
				glUniform4f(vertexColorLocation, 1.0f, 0.0f, 1.0f, 0.0f); 
				glm::mat4 squares;
				squares = glm::translate(squares, player3Array[i]);
				GLuint squaresLocation = glGetUniformLocation(shaderProgram, "squares");
				glUniformMatrix4fv(squaresLocation, 1, GL_FALSE, glm::value_ptr(squares));
				glDrawArrays(GL_POINTS, 0, 1);

			}
			break;

			//Draws every players' trajectory
		case 4:
			for (GLuint i = 0; i < 250000; i++)
			{
				//player 0
				glm::mat4 squares;
				GLuint squaresLocation = glGetUniformLocation(shaderProgram, "squares");
				glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 0.0f);
				squares = glm::translate(squares, player0Array[i]);
				glUniformMatrix4fv(squaresLocation, 1, GL_FALSE, glm::value_ptr(squares));
				glDrawArrays(GL_POINTS, 0, 1);

			}

			for (GLuint i = 0; i < 250000; i++)
			{
				//player 1
				glUniform4f(vertexColorLocation, 1.0f, 1.0f, 1.0f, 0.0f);
				glm::mat4 squares;
				GLuint squaresLocation = glGetUniformLocation(shaderProgram, "squares");
				squares = glm::translate(squares, player1Array[i]);
				glUniformMatrix4fv(squaresLocation, 1, GL_FALSE, glm::value_ptr(squares));
				glDrawArrays(GL_POINTS, 0, 1);

			}

			for (GLuint i = 0; i < 250000; i++)
			{

				//player 2
				glUniform4f(vertexColorLocation, 0.0f, 0.0f, 1.0f, 0.0f);
				glm::mat4 squares;
				GLuint squaresLocation = glGetUniformLocation(shaderProgram, "squares");
				squares = glm::translate(squares, player2Array[i]);
				glUniformMatrix4fv(squaresLocation, 1, GL_FALSE, glm::value_ptr(squares));
				glDrawArrays(GL_POINTS, 0, 1);
			}

			for (GLuint i = 0; i < 250000; i++)
			{
				//player 3
				glUniform4f(vertexColorLocation, 1.0f, 0.0f, 1.0f, 0.0f);
				glm::mat4 squares;
				GLuint squaresLocation = glGetUniformLocation(shaderProgram, "squares");
				squares = glm::translate(squares, player3Array[i]);
				glUniformMatrix4fv(squaresLocation, 1, GL_FALSE, glm::value_ptr(squares));
				glDrawArrays(GL_POINTS, 0, 1);

			}

			break;

		default:
			glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 0.0f);
			glDrawArrays(GL_POINTS, 0, 1);



			break;

			//================================================================================================
			//================================================================================================
		}

		glBindVertexArray(0);
		//Swap the screen buffers
		glfwSwapBuffers(window);

	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glfwTerminate();
	return 0;
}


//---Reads all data from text file---- 
void playerAllPositions()
{
	ifstream playerPosFile;
	string line;

	int i = 0;
	playerPosFile.open("QuakeLogData.txt");

	//Error check
	if (playerPosFile.fail())
	{
		cerr << "CAN'T OPEN FILE" << endl;
		exit(1);
	}

	//Start reading data 
	cout << "Reading data: ALL" << endl;
	for (string line; getline(playerPosFile, line);)
	{
		//Take in line  
		istringstream in(line);
		string linePosition;
		in >> linePosition;
		if (linePosition == "CurrentPos")
		{
			float x, y, z;
			in >> x >> y;
			playerAllArray[i] = glm::vec3(x, y, 0.0f);
			i++;
		}
	}

	i = 0;
	cout << "Successs" << endl;
}

//----Positions of player 0---- 
void player0Positions()
{
	ifstream playerPosFile;
	string line;
	playerPosFile.open("QuakeLogData.txt");

	int i = 0;

	//Start reading data 
	cout << "Reading data: Player 0" << endl;
	for (string line; getline(playerPosFile, line);)
	{
		istringstream in(line);
		string type;
		in >> type;

		if (type == "Player")
		{
			int playerNumber;
			in >> playerNumber;

			if (playerNumber == 0)
			{
				getline(playerPosFile, line);
				istringstream in(line);
				string linePosition;
				in >> linePosition;

				if (linePosition == "CurrentPos")
				{
					float x, y;
					in >> x >> y; //Don't need Z value
					player0Array[i] = glm::vec3(x, y, 0.0f); //Z value = 0 becasue HeatMap is 2D 
					i++;
				}
			}
		}
	}

	i = 0;
	cout << "Success" << endl;
}

//----Positions of player 1---- 
void player1Positions()
{
	ifstream playerPosFile;
	string line;
	playerPosFile.open("QuakeLogData.txt");

	int i = 0;


	//Start reading data 
	cout << "Reading data: Player 1" << endl;
	for (string line; getline(playerPosFile, line);)
	{
		istringstream in(line);
		string type;
		in >> type;

		if (type == "Player")
		{
			int playerNumber;
			in >> playerNumber;

			if (playerNumber == 1)
			{
				getline(playerPosFile, line);
				istringstream in(line);
				string linePosition;
				in >> linePosition;

				if (linePosition == "CurrentPos")
				{
					float x, y;
					in >> x >> y; //Don't need Z value
					player1Array[i] = glm::vec3(x, y, 0.0f); //Z value = 0 becasue HeatMap is 2D 
					i++;
				}
			}
		}
	}

	i = 0;
	cout << "Success" << endl;
}

//----Positions of player 2---- 
void player2Positions()
{
	ifstream playerPosFile;
	string line;
	playerPosFile.open("QuakeLogData.txt");

	int i = 0;

	//Start reading data 
	cout << "Reading data: Player 2" << endl;
	for (string line; getline(playerPosFile, line);)
	{
		istringstream in(line);
		string type;
		in >> type;

		if (type == "Player")
		{
			int playerNumber;
			in >> playerNumber;

			if (playerNumber == 2)
			{
				getline(playerPosFile, line);
				istringstream in(line);
				string linePosition;
				in >> linePosition;

				if (linePosition == "CurrentPos")
				{
					float x, y;
					in >> x >> y; //Don't need Z value
					player2Array[i] = glm::vec3(x, y, 0.0f); //Z value = 0 becasue HeatMap is 2D 
					i++;
				}
			}
		}
	}

	i = 0;
	cout << "Success" << endl;
}

//----Positions of player 3---- 
void player3Positions()
{

	ifstream playerPosFile;
	string line;
	playerPosFile.open("QuakeLogData.txt");

	int i = 0;


	//Start reading data 
	cout << "Reading data: Player 3" << endl;
	for (string line; getline(playerPosFile, line);)
	{
		istringstream in(line);
		string type;
		in >> type;

		if (type == "Player")
		{
			int playerNumber;
			in >> playerNumber;

			if (playerNumber == 3)
			{
				getline(playerPosFile, line);
				istringstream in(line);
				string linePosition;
				in >> linePosition;

				if (linePosition == "CurrentPos")
				{
					float x, y;
					in >> x >> y; //Don't need Z value
					player3Array[i] = glm::vec3(x, y, 0.0f); //Z value = 0 becasue HeatMap is 2D 
					i++;
				}
			}
		}
	}

	i = 0;
	cout << "Success" << endl;
}

//----Camera Controls----
void doMovement()
{

	GLfloat cameraSpeed = 100.0f;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	//follow tutorial on cameraRight 
}

//----Key press detection---- 
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
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
}

//----Decides which player to view----
void playerSwitcher()
{
	if (keys[GLFW_KEY_1])
	{
		player = 0;
	}
	if (keys[GLFW_KEY_2])
	{
		player = 1;
	}
	if (keys[GLFW_KEY_3])
	{
		player = 2;
	}
	if (keys[GLFW_KEY_4])
	{
		player = 3;
	}
	if (keys[GLFW_KEY_5])
	{
		player = 4;
	}
	if (keys[GLFW_KEY_6])
	{
		player = 5;
	}

}

void playerDeathPositions()
{

	ifstream deathPosFile;
	string line;

	int i = 0;
	deathPosFile.open("DeathAnalytics.txt");

	//Error check
	if (deathPosFile.fail())
	{
		cerr << "CAN'T OPEN FILE" << endl;
		exit(1);
	}

	//Start reading data 
	cout << "Reading data: Deaths" << endl;
	for (string line; getline(deathPosFile, line);)
	{
		//Take in line  
		istringstream in(line);
		string linePosition;
		in >> linePosition;
		if (linePosition == "DiedAt")
		{
			float x, y, z;
			in >> x >> y;
			playerDeathArray[i] = glm::vec3(x, y, 0.0f);
			i++;
		}
	}

	i = 0;
	cout << "Successs" << endl;

}

//----Populates array of squares---- 
void setGrid()
{
	//this method splits the screen up into 225*225 squares
	//this will be used for the death map 

	int x = 2000, k = 0;
	for (int i = 0; i < 400; i++)
	{
		x -= 10;

		int y = 2000;
		for (int j = 0; j < 400; j++)
		{
			y -= 10;
			squaresArray[k] = glm::vec3(x, y, 0.0f);
			k++;
		}
	}
	k = 0;
}

void checkColourArray()
{
	cout << "Checking colour array" << std::endl;
	int colourNumber = 0;
	{

		for (int i = 0; i < 400 * 400; i++) //deathSquaresCheck 
		{
			for (int j = 0; j < 100000; j++) //deathPositionsArray 
			{
				if (playerDeathArray[j].x < (squaresArray[i].x + 5) && playerDeathArray[j].y < (squaresArray[i].y + 5) && playerDeathArray[j].x >(squaresArray[i].x - 5) && playerDeathArray[j].y >(squaresArray[i].y - 5))
				{
					colourNumber++;
				}
			}

			colourArray[i] = colourNumber;
			if (colourArray[i] < 0)
			{
				colourArray[i] = 0;
			}
			colourNumber = 0;
		}
	}
}

//----Creates colours----
void colourPallette(int colourCode)
{
	switch (colourCode) {
	case 0:
		red = 0.0f;
		green = 0.0f;
		blue = 0.0f;
		break;
	case 1:
		red = 0.0f;
		green = 0.0f;
		blue = 0.2f;
		break;
	case 2:
		red = 0.0f;
		green = 0.0f;
		blue = 0.4f;
	case 3:
		red = 0.0f;
		green = 0.0f;
		blue = 0.6f;
		break;
	case 4:
		red = 0.0f;
		green = 0.0f;
		blue = 0.8f;
		break;
	case 5:
		red = 0.0f;
		green = 0.0f;
		blue = 1.0f;
		break;
	case 6:
		red = 0.0f;
		green = 0.2f;
		blue = 1.0f;
		break;
	case 7:
		red = 0.0f;
		green = 0.4f;
		blue = 1.0f;
		break;
	case 8:
		red = 0.0f;
		green = 0.6f;
		blue = 1.0f;
		break;
	case 9:
		red = 0.0f;
		green = 0.8f;
		blue = 1.0f;
		break;
	case 10:
		red = 0.0f;
		green = 1.0f;
		blue = 1.0f;
		break;
	case 11:
		red = 0.0f;
		green = 1.0f;
		blue = 0.8f;
		break;
	case 12:
		red = 0.0f;
		green = 1.0f;
		blue = 0.6f;
		break;
	case 13:
		red = 0.0f;
		green = 1.0f;
		blue = 0.4f;
		break;
	case 14:
		red = 0.0f;
		green = 1.0f;
		blue = 0.2f;
		break;
	case 15:
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
		break;
	case 16:
		red = 0.2f;
		green = 1.0f;
		blue = 0.0f;
		break;
	case 17:
		red = 0.4f;
		green = 1.0f;
		blue = 0.0f;
		break;
	case 18:
		red = 0.6f;
		green = 1.0f;
		blue = 0.0f;
		break;
	case 19:
		red = 0.8f;
		green = 1.0f;
		blue = 0.0f;
		break;
	case 20:
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
		break;
	case 21:
		red = 1.0f;
		green = 0.8f;
		blue = 0.0f;
		break;
	case 22:
		red = 1.0f;
		green = 0.6f;
		blue = 0.0f;
		break;
	case 23:
		red = 1.0f;
		green = 0.4f;
		blue = 0.0f;
		break;
	case 24:
		red = 1.0f;
		green = 0.2f;
		blue = 0.0f;
		break;
	case 25:
		red = 1.0f;
		green = 0.2f;
		blue = 0.0f;
		break;
	case 26:
		red = 1.0f;
		green = 0.2f;
		blue = 0.0f;
		break;

	case 27:
		red = 1.0f;
		green = 0.2f;
		blue = 0.0f;
		break;
	case 28:
		red = 1.0f;
		green = 0.2f;
		blue = 0.0f;
		break;
	case 29:
		red = 1.0f;
		green = 0.2f;
		blue = 0.0f;
		break;
	case 30:
		red = 1.0f;
		green = 0.2f;
		blue = 0.0f;
	
		break;
	default:
		break;
	}
}