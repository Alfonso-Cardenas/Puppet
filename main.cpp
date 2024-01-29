#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "EBO.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "Camera.h"
#include "Texture.h"

const unsigned int DOF = 16;			//numero de grados de libertad
const unsigned int BASE_DIM = 360;		//cantidad de elementos guardados en la primitiva
const unsigned int PARTS = 18;			//cantidad de partes del cuerpo

const unsigned int width = 1200;		//ancho de la ventana
const unsigned int height = 800;		//alto de la ventana

//indices que ocupa cada parte del cuerpo en nuestro arreglo de partes del cuerpo
const unsigned int HEAD				= 0;
const unsigned int NOSE				= 1;
const unsigned int NECK				= 2;
const unsigned int BODY				= 3;
const unsigned int LEFT_EYE			= 4;
const unsigned int RIGHT_EYE		= 5;
const unsigned int LEFT_ARM			= 6;
const unsigned int RIGHT_ARM		= 7;
const unsigned int LEFT_FOREARM		= 8;
const unsigned int RIGHT_FOREARM	= 9;
const unsigned int LEFT_UPLEG		= 10;
const unsigned int RIGHT_UPLEG		= 11;
const unsigned int LEFT_DOWNLEG		= 12;
const unsigned int RIGHT_DOWNLEG	= 13;
const unsigned int LEFT_HAND		= 14;
const unsigned int RIGHT_HAND		= 15;
const unsigned int LEFT_FOOT		= 16;
const unsigned int RIGHT_FOOT		= 17;

//vector que guarda la rotacion actual en cada articulacion
float currRotation[] =
{
	0.0f,	//HEADX
	0.0f,	//HEADY
	0.0f,	//HEADZ
	0.0f,	//ARMX
	0.0f,	//ARMY
	0.0f,	//ARMZ
	0.0f,	//FARMX
	0.0f,	//FARMZ
	0.0f,	//HANDY
	0.0f,	//HANDZ
	0.0f,	//ULEGX
	0.0f,	//ULEGY
	0.0f,	//ULEGZ
	0.0f,	//LLEGX
	0.0f,	//FEETX
	0.0f,	//FEETY
	0.0f,	//ARMX
	0.0f,	//ARMY
	0.0f,	//ARMZ
	0.0f,	//FARMX
	0.0f,	//FARMZ
	0.0f,	//HANDY
	0.0f,	//HANDZ
	0.0f,	//ULEGX
	0.0f,	//ULEGY
	0.0f,	//ULEGZ
	0.0f,	//LLEGX
	0.0f,	//FEETX
	0.0f	//FEETY
};

//rotaciones maximas fuera del angulo inicial, guarda la rotacion minima, 
//un cero basura y la rotacion maxima por cada articulacion y angulo de rotacion
const float MAX_ROTATION[][3] ={
	{ -20.0f, 0.0f, 20.0f },	//HEADX
	{ -70.0f, 0.0f, 70.0f },	//HEADY
	{ -20.0f, 0.0f, 20.0f },	//HEADZ
	{  0.0f, 0.0f, 130.0f },	//ARMX
	{   0.0f, 0.0f, 90.0f },	//ARMY
	{ -60.0f, 0.0f,  0.0f },	//ARMZ
	{ -180.0f, 0.0f, 0.0f },	//FARMX
	{  0.0f, 0.0f, 140.0f },	//FARMZ
	{ -20.0f, 0.0f, 20.0f },	//HANDY
	{ -45.0f, 0.0f, 45.0f },	//HANDZ
	{ -20.0f, 0.0f, 90.0f },	//ULEGX
	{ -45.0f, 0.0f,  0.0f },	//ULEGY
	{   0.0f, 0.0f, 45.0f },	//ULEGZ
	{ -130.0f, 0.0f, 0.0f },	//LLEGX
	{ -20.0f, 0.0f, 20.0f },	//FEETX
	{ -20.0f, 0.0f, 20.0f },	//FEETY
	{  0.0f, 0.0f, 130.0f },	//ARMX
	{   0.0f, 0.0f, 90.0f },	//ARMY
	{ -60.0f, 0.0f,  0.0f },	//ARMZ
	{ -180.0f, 0.0f, 0.0f },	//FARMX
	{  0.0f, 0.0f, 140.0f },	//FARMZ
	{ -20.0f, 0.0f, 20.0f },	//HANDY
	{ -45.0f, 0.0f, 45.0f },	//HANDZ
	{ -20.0f, 0.0f, 90.0f },	//ULEGX
	{ -45.0f, 0.0f,  0.0f },	//ULEGY
	{   0.0f, 0.0f, 45.0f },	//ULEGZ
	{ -130.0f, 0.0f, 0.0f },	//LLEGX
	{ -20.0f, 0.0f, 20.0f },	//FEETX
	{ -20.0f, 0.0f, 20.0f }		//FEETY
};

//lista de los vectores que utiliza cada articulacion para rotar
const glm::vec3 rotVector[] =
{
	glm::vec3(1.0f, 0.0f, 0.0f),	//HEADX
	glm::vec3(0.0f, 1.0f, 0.0f),	//HEADY
	glm::vec3(0.0f, 0.0f, 1.0f),	//HEADZ
	glm::vec3(1.0f, 0.0f, 0.0f),	//LARMX
	glm::vec3(0.0f, 1.0f, 0.0f),	//LARMY
	glm::vec3(0.0f, 0.0f,-1.0f),	//LARMZ
	glm::vec3(1.0f, 0.0f, 0.0f),	//LFARMX
	glm::vec3(0.0f, 0.0f,-1.0f),	//LFARMZ
	glm::vec3(0.0f, 1.0f, 0.0f),	//LHANDY
	glm::vec3(0.0f, 0.0f,-1.0f),	//LHANDZ
	glm::vec3(-1.0f, 0.0f, 0.0f),	//LULEGX
	glm::vec3(0.0f, 1.0f, 0.0f),	//LULEGY
	glm::vec3(0.0f, 0.0f,-1.0f),	//LULEGZ
	glm::vec3(-1.0f, 0.0f, 0.0f),	//LLLEGX
	glm::vec3(-1.0f, 0.0f, 0.0f),	//LFEETX
	glm::vec3(0.0f, 1.0f, 0.0f),	//LFEETY
	glm::vec3(1.0f, 0.0f, 0.0f),	//RARMX
	glm::vec3(0.0f,-1.0f, 0.0f),	//RARMY
	glm::vec3(0.0f, 0.0f, 1.0f),	//RARMZ
	glm::vec3(1.0f, 0.0f, 0.0f),	//RFARMX
	glm::vec3(0.0f, 0.0f, 1.0f),	//RFARMZ
	glm::vec3(0.0f,-1.0f, 0.0f),	//RHANDY
	glm::vec3(0.0f, 0.0f, 1.0f),	//RHANDZ
	glm::vec3(-1.0f, 0.0f, 0.0f),	//RULEGX
	glm::vec3(0.0f,-1.0f, 0.0f),	//RULEGY
	glm::vec3(0.0f, 0.0f, 1.0f),	//RULEGZ
	glm::vec3(-1.0f, 0.0f, 0.0f),	//RLLEGX
	glm::vec3(-1.0f, 0.0f, 0.0f),	//RFEETX
	glm::vec3(0.0f, -1.0f, 0.0f)	//RFEETY
};

//lista de las coordenadas de cada articualcion, las partes que no se mueven tienen
//guardado el centro de su figura, pero no se utiliza.
glm::vec3 joint[] =
{
	glm::vec3(0.0f,  0.7f, 0.0f),	   //HEAD		
	glm::vec3(0.0f, 0.87f, 0.2f),	   //NOSE - not relevant	
	glm::vec3(0.0f, 0.58f, 0.0f),	   //NECK - not relevant
	glm::vec3(0.0f, 0.0f, 0.0f),	   //BODY - not relevant
	glm::vec3(-0.1f, 1.0f, 0.2f),	   //LEFT_EYE	- not relevant
	glm::vec3(0.1f, 1.0f, 0.2f),	   //RIGHT_EYE	- not relevant
	glm::vec3(-0.07f, 0.44f, 0.0f),	   //LEFT_ARM	
	glm::vec3(0.07f,  0.44f, 0.0f),	   //RIGHT_ARM	
	glm::vec3(-0.53f,0.44f, 0.0f),	   //LEFT_FOREARM
	glm::vec3(0.53f, 0.44f, 0.0f),	   //RIGHT_FOREARM
	glm::vec3(-0.1f,-0.44f, 0.0f),	   //LEFT_UPLEG	
	glm::vec3(0.1f, -0.44f, 0.0f),	   //RIGHT_UPLEG
	glm::vec3(-0.1f,-0.94f, 0.0f),	   //LEFT_DOWNLEG
	glm::vec3(0.1f, -0.94f, 0.0f),	   //RIGHT_DOWNLEG
	glm::vec3(-0.9f, 0.44f, 0.0f),	   //LEFT_HAND	
	glm::vec3(0.9f,  0.44f, 0.0f),	   //RIGHT_HAND	
	glm::vec3(-0.1f, -1.45f, 0.0f),	   //LEFT_FOOT	
	glm::vec3(0.1f,  -1.45f, 0.0f)	   //RIGHT_FOOT	
};

//arreglo que guarda la parte del cuerpo a la que debe de seguir para 
//obedecer la jerarquia, aquellas que no se mueven o no son hijos, guardan -1
const int parent[] =
{
	-1,				//HEAD			
	HEAD,			//NOSE			
	-1,				//NECK			
	-1,				//BODY			
	HEAD,			//LEFT_EYE		
	HEAD,			//RIGHT_EYE	
	-1,				//LEFT_ARM		
	-1,				//RIGHT_ARM	
	LEFT_ARM,		//LEFT_FOREARM	
	RIGHT_ARM,		//RIGHT_FOREARM
	-1,				//LEFT_UPLEG	
	-1,				//RIGHT_UPLEG	
	LEFT_UPLEG,		//LEFT_DOWNLEG	
	RIGHT_UPLEG,	//RIGHT_DOWNLEG
	LEFT_FOREARM,	//LEFT_HAND	
	RIGHT_FOREARM,	//RIGHT_HAND	
	LEFT_DOWNLEG,	//LEFT_FOOT	
	RIGHT_DOWNLEG	//RIGHT_FOOT	
};

//guarda -1 si se va a mover las partes del cuerpo del lado izq y 1
//si se van a mover las del lado derecho
int currSide = -1;

//funcion recursiva que ayuda a multiplicar las matrices de transformacion de una parte del cuerpo
//por las transformaciones de las partes que son sus padres
glm::mat4 multiplyParents(glm::mat4 *liveModel, int idx)
{
	if (parent[idx] == -1) return liveModel[idx];
	else return  multiplyParents(liveModel, parent[idx]) * liveModel[idx];
}

//recibe el indice correspondiente a alguna rotacion, intepreta a que parte del cuerpo corresponde, hace la rotacion correspondiente
//y actualiza la rotacion actual que se ha hecho
void rotateBodypart(glm::mat4* model, glm::mat4* rot, int rotationIdx, int rotationSign)
{
	int appendix = HEAD;
	int offset = 0;
	
	if (rotationIdx < 3) appendix = HEAD;
	else if (rotationIdx < 6)appendix = RIGHT_ARM;
	else if (rotationIdx < 8) appendix = RIGHT_FOREARM;
	else if (rotationIdx < 10) appendix = RIGHT_HAND;
	else if (rotationIdx < 13) appendix = RIGHT_UPLEG;
	else if (rotationIdx < 14) appendix = RIGHT_DOWNLEG;
	else if (rotationIdx < 16) appendix = RIGHT_FOOT;
	if (currSide == 1 && appendix != HEAD) offset = 13;
	else if (currSide == -1 && appendix != HEAD) appendix--;

	if (currRotation[rotationIdx+offset] * rotationSign > rotationSign * MAX_ROTATION[rotationIdx][rotationSign + 1]) return;

	model[appendix] = glm::translate(model[appendix], joint[appendix]);
	model[appendix] = glm::rotate(model[appendix], glm::radians(rotationSign * 1.0f), rotVector[rotationIdx+offset]);
	rot[appendix] = glm::rotate(rot[appendix], glm::radians(rotationSign * 1.0f), rotVector[rotationIdx+offset]);
	model[appendix] = glm::translate(model[appendix], -joint[appendix]);
	currRotation[rotationIdx+offset] += rotationSign * 1.0f;
}

/*
lee inputs dados por el usuario a traves del teclado o click a los botones de la UI hace lo siguiente:
W:			rota a la marioneta en el eje x
S:			W pero en sentido contrario
A:			rota a la marioneta en el eje y
D:			A pero en sentido contrario
Q:			rota a la marioneta en el eje z
E:			Q pero en sentido contrario
C:			define que va a rotar el lado izquierdo
V:			define que va a rotar el lado derecho
Click:		lee si el mouse esta posicionado sobre un boton y rota la parte del cuerpo correspondiente al mismo
UP:			traslada la marioneta en -z
DOWN:		traslada la marioneta en +z
LEFT:		traslada la marioneta en -x
RIGHT:		traslada la marioneta en +x
LEFT_CONROL:traslada la marioneta en -y
SPACE:		traslada la marioneta en +y
R:			regresa la marioneta a su rotacion original
T:			regresa la marioneta a su posicion original
P:			regresa la marioneta a su pose original (T-pose)
*/
void input(GLFWwindow* window, glm::mat4* liveModel, glm::mat4* liveRot, glm::mat4* globalModel, glm::mat4* globalRot, glm::mat4* globalTrans)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalModel[i] = glm::rotate(globalModel[i], glm::radians(-1.0f), glm::vec3(1.0f,0.0f,0.0f));
			globalRot[i] = glm::rotate(globalRot[i], glm::radians(-1.0f), glm::vec3(1.0f,0.0f,0.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalModel[i] = glm::rotate(globalModel[i], glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			globalRot[i] = glm::rotate(globalRot[i], glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalModel[i] = glm::rotate(globalModel[i], glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			globalRot[i] = glm::rotate(globalRot[i], glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalModel[i] = glm::rotate(globalModel[i], glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			globalRot[i] = glm::rotate(globalRot[i], glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalModel[i] = glm::rotate(globalModel[i], glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			globalRot[i] = glm::rotate(globalRot[i], glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalModel[i] = glm::rotate(globalModel[i], glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			globalRot[i] = glm::rotate(globalRot[i], glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		currSide = -1;
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		currSide = 1;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		//std::cout << mouseX << " " << mouseY << std::endl;

		if (mouseX >= 790 && mouseX <= 820)
		{
			for (int i = 0; i < DOF; i++)
			{
				if (mouseY >= 43 + 45 * i && mouseY <= 72 + 45 * i) 
				{
					rotateBodypart(liveModel, liveRot, i, -1);
					break;
				}
			}
		}
		else if(mouseX >= 1100 && mouseX <= 1130)
		{
			for (int i = 0; i < DOF; i++)
			{
				if (mouseY >= 43 + 45 * i && mouseY <= 72 + 45 * i)
				{
					rotateBodypart(liveModel, liveRot, i,  1);
					break;
				}
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalTrans[i] = glm::translate(globalTrans[i], glm::vec3(0.0f, 0.0f, -0.02f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalTrans[i] = glm::translate(globalTrans[i], glm::vec3(0.0f, 0.0f,  0.02f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalTrans[i] = glm::translate(globalTrans[i], glm::vec3(0.02f, 0.0f, 0.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalTrans[i] = glm::translate(globalTrans[i], glm::vec3(-0.02f, 0.0f, 0.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalTrans[i] = glm::translate(globalTrans[i], glm::vec3(0.0f, 0.02f, 0.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalTrans[i] = glm::translate(globalTrans[i], glm::vec3(0.0f,-0.02f, 0.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalModel[i] = glm::mat4(1.0f);
			globalRot[i] = glm::mat4(1.0f);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			globalTrans[i] = glm::mat4(1.0f);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		for (int i = 0; i < PARTS; i++)
		{
			liveModel[i] = glm::mat4(1.0f);
			liveRot[i] = glm::mat4(1.0f);
		}
		for (int i = 0; i < DOF + 13; i++)
		{
			currRotation[i] = 0.0f;
		}
	}
}

int main() {
	//inicia GLFW
	glfwInit();

	//le dice a GLFW la version de openGL que se esta usando 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//le dice a GLFW que estamos usando el perfil de core (openGL moderno)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//vertices para dibujar la interfaz
	GLfloat ui[] =
	{
		1.7f,-1.87f, 0.0f,		0.0f, 0.0f,
		1.7f, 1.87f, 0.0f,		0.0f, 1.0f,
		3.8f, 1.87f, 0.0f,		1.0f, 1.0f,
		3.8f,-1.87f, 0.0f,		1.0f, 0.0f
	};

	//orden de los indices de la interfaz
	GLuint uiIndices[] =
	{
		0, 1, 2, 
		0, 3, 2
	};

	//coordenadas de la figura base
	GLfloat baseFigure[] =
	{ //     COORDINATES     /        COLORS         /        NORMALS       //
		-0.5f,-0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	-1.0f,  0.0f, 0.0f, // Bottom side
		-0.5f,-0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	-1.0f,  0.0f, 0.0f, // Bottom side
		-0.5f, 0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	-1.0f,  0.0f, 0.0f, // Bottom side
		-0.5f, 0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	-1.0f,  0.0f, 0.0f, // Bottom side

		-0.5f, 0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	-0.8f, 0.5f,  0.0f, // Left Side
		-0.5f, 0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	-0.8f, 0.5f,  0.0f, // Left Side
		 0.0f, 1.2f,  0.0f,     0.83f, 0.70f, 0.44f,	-0.8f, 0.5f,  0.0f, // Left Side

		-0.5f, 0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.5f, -0.8f, // Non-facing side
		 0.5f, 0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.5f, -0.8f, // Non-facing side
		 0.0f, 1.2f,  0.0f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.5f, -0.8f, // Non-facing side

		 0.5f, 0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.8f, 0.5f,  0.0f, // Right side
		 0.5f, 0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	 0.8f, 0.5f,  0.0f, // Right side
		 0.0f, 1.2f,  0.0f,     0.83f, 0.70f, 0.44f,	 0.8f, 0.5f,  0.0f, // Right side

		 0.5f, 0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.5f,  0.8f, // Facing side
		-0.5f, 0.4f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.5f,  0.8f, // Facing side
		 0.0f, 1.2f,  0.0f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.5f,  0.8f, // Facing side

		 0.5f,-0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	 1.0f,  0.0f, 0.0f, // Bottom side
		 0.5f,-0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 1.0f,  0.0f, 0.0f, // Bottom side
		 0.5f, 0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 1.0f,  0.0f, 0.0f, // Bottom side
		 0.5f, 0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	 1.0f,  0.0f, 0.0f, // Bottom side

		 0.5f,-0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,  0.0f,-1.0f, // Bottom side
		-0.5f,-0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,  0.0f,-1.0f, // Bottom side
		-0.5f, 0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,  0.0f,-1.0f, // Bottom side
		 0.5f, 0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,  0.0f,-1.0f, // Bottom side

		 0.5f,-0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,  0.0f, 1.0f, // Bottom side
		-0.5f,-0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,  0.0f, 1.0f, // Bottom side
		-0.5f, 0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,  0.0f, 1.0f, // Bottom side
		 0.5f, 0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,  0.0f, 1.0f, // Bottom side

		-0.5f,-0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	-0.8f,-0.5f,  0.0f, // Left Side
		-0.5f,-0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	-0.8f,-0.5f,  0.0f, // Left Side
		 0.0f,-1.2f,  0.0f,     0.83f, 0.70f, 0.44f,	-0.8f,-0.5f,  0.0f, // Left Side

		-0.5f,-0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,-0.5f, -0.8f, // Non-facing side
		 0.5f,-0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,-0.5f, -0.8f, // Non-facing side
		 0.0f,-1.2f,  0.0f,     0.83f, 0.70f, 0.44f,	 0.0f,-0.5f, -0.8f, // Non-facing side

		 0.5f,-0.4f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.8f,-0.5f,  0.0f, // Right side
		 0.5f,-0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	 0.8f,-0.5f,  0.0f, // Right side
		 0.0f,-1.2f,  0.0f,     0.83f, 0.70f, 0.44f,	 0.8f,-0.5f,  0.0f, // Right side

		 0.5f,-0.4f,  0.5f,     0.83f, 0.70f, 0.44f,	 0.0f,-0.5f,  0.8f, // Facing side
		-0.5f,-0.4f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f,-0.5f,  0.8f, // Facing side
		 0.0f,-1.2f,  0.0f,     0.83f, 0.70f, 0.44f,	 0.0f,-0.5f,  0.8f // Facing side
	};

	//indices de la figura base
	GLuint indices[] =
	{
		0, 1, 2,	// cara cuadrada
		0, 2, 3,	// cara cuadrada
		4, 6, 5,	// Left side
		7, 9, 8,	// Non-facing side
		10, 12, 11, // Right side
		13, 15, 14, // Facing side
		16, 17, 18, // cara cuadrada
		16, 18, 19, // cara cuadrada
		20, 21, 22, // cara cuadrada
		20, 22, 23, // cara cuadrada
		24, 25, 26, // cara cuadrada
		24, 26, 27, // cara cuadrada
		28, 30, 29,
		31, 33, 32,
		34, 36, 35,
		37, 39, 38
	};

	//copiamos la figura base una vez por cada parte del cuerpo
	GLfloat puppet[PARTS][BASE_DIM];
	for (int i = 0; i < PARTS; i++)
	{
		for (int j = 0; j < BASE_DIM; j++)
		{
			puppet[i][j] = baseFigure[j];
		}
	}

	//crea una ventana de width por height
	GLFWwindow* window = glfwCreateWindow(width, height, "Proyecto Alfonso Cardenas", NULL, NULL);
	//mensaje de error en caso que no se pueda crear la ventana, regresa -1
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//mete la ventana al contexto de GLFW
	glfwMakeContextCurrent(window);

	//hace que se actualice la ventana solo cuando lo hace la pantalla
	glfwSwapInterval(1);

	//carga glad
	gladLoadGL();

	//da las dimensiones de openGL en nuestra ventana
	glViewport(0, 0, width, height);

	//crea un shader, VAO, VBO y EBO para cada parte del cuerpo
	Shader shaderPrograms[PARTS];
	VAO puppetVAO[PARTS];
	VBO puppetVBO[PARTS];
	EBO puppetEBO[PARTS];
	for (int i = 0; i < PARTS; i++)
	{
		shaderPrograms[i].constructShader("default.vert", "default.frag");

		puppetVAO[i].Bind();

		puppetVBO[i].constructVBO(puppet[i], sizeof(baseFigure));
		puppetEBO[i].constructEBO(indices, sizeof(indices));

		puppetVAO[i].LinkAttrib(puppetVBO[i], 0, 3, GL_FLOAT, 9 * sizeof(float), (void*)0);
		puppetVAO[i].LinkAttrib(puppetVBO[i], 1, 3, GL_FLOAT, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		puppetVAO[i].LinkAttrib(puppetVBO[i], 2, 3, GL_FLOAT, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		puppetVAO[i].Unbind();
		puppetVBO[i].Unbind();
		puppetEBO[i].Unbind();
	}

	//crea un shader, VAO, VBO y EBO para la UI
	Shader uiShader("UI.vert", "UI.frag");
	VAO uiVAO;
	uiVAO.Bind();
	VBO uiVBO(ui, sizeof(ui));
	EBO uiEBO(uiIndices, sizeof(uiIndices));
	uiVAO.LinkAttrib(uiVBO, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	uiVAO.LinkAttrib(uiVBO, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	//definimos el color de la luz y su posicion
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(2.0f, 1.5f, 1.5f);

	//vector de traslaciones, escala y rotaciones necesarias para pasar de las primitivas originales a la marioneta 
	glm::vec3 pos[] =
	{
		glm::vec3(0.0f,  1.0f, 0.0f),	   //HEAD		
		glm::vec3(0.0f, 0.87f, 0.2f),	   //NOSE		
		glm::vec3(0.0f, 0.58f, 0.0f),	   //NECK		
		glm::vec3(0.0f, 0.0f, 0.0f),	   //BODY		
		glm::vec3(-0.1f, 1.0f, 0.2f),	   //LEFT_EYE	
		glm::vec3(0.1f, 1.0f, 0.2f),	   //RIGHT_EYE	
		glm::vec3(-0.3f, 0.44f, 0.0f),	   //LEFT_ARM	
		glm::vec3(0.3f,  0.44f, 0.0f),	   //RIGHT_ARM	
		glm::vec3(-0.77f,0.44f, 0.0f),	   //LEFT_FOREARM
		glm::vec3(0.77f, 0.44f, 0.0f),	   //RIGHT_FOREARM
		glm::vec3(-0.1f,-0.64f, 0.0f),	   //LEFT_UPLEG	
		glm::vec3(0.1f, -0.64f, 0.0f),	   //RIGHT_UPLEG
		glm::vec3(-0.1f,-1.23f, 0.0f),	   //LEFT_DOWNLEG
		glm::vec3(0.1f, -1.23f, 0.0f),	   //RIGHT_DOWNLEG
		glm::vec3(-1.0f, 0.44f, 0.0f),	   //LEFT_HAND	
		glm::vec3(1.0f,  0.44f, 0.0f),	   //RIGHT_HAND	
		glm::vec3(-0.1f, -1.52f, 0.1f),	   //LEFT_FOOT	
		glm::vec3(0.1f,  -1.52f, 0.1f)	   //RIGHT_FOOT	
	};
	glm::vec3 scale[] = 
	{
		glm::vec3(0.4f, 0.4f, 0.4f),		//HEAD		
		glm::vec3(0.1f, 0.1f, 0.1f),		//NOSE		
		glm::vec3(0.15f, 0.2f, 0.2f),		//NECK		
		glm::vec3(0.3f, 0.5f, 0.1f),		//BODY		
		glm::vec3(0.07f, 0.07f, 0.07f),		//LEFT_EYE	
		glm::vec3(0.07f, 0.07f, 0.07f),		//RIGHT_EYE	
		glm::vec3(0.2f, 0.1f, 0.1f),		//LEFT_ARM	
		glm::vec3(0.2f, 0.1f, 0.1f),		//RIGHT_ARM	
		glm::vec3(0.2f, 0.1f, 0.1f),		//LEFT_FOREARM
		glm::vec3(0.2f, 0.1f, 0.1f),		//RIGHT_FOREARM
		glm::vec3(0.1f, 0.25f, 0.1f),		//LEFT_UPLEG	
		glm::vec3(0.1f, 0.25f, 0.1f),		//RIGHT_UPLEG
		glm::vec3(0.1f, 0.25f, 0.1f),		//LEFT_DOWNLEG
		glm::vec3(0.1f, 0.25f, 0.1f),		//RIGHT_DOWNLEG
		glm::vec3(0.1f,  0.2f, 0.2f),		//LEFT_HAND	
		glm::vec3(0.1f,  0.2f, 0.2f),		//RIGHT_HAND	
		glm::vec3(0.1f, 0.05f, 0.2f),		//LEFT_FOOT	
		glm::vec3(0.1f, 0.05f, 0.2f)		//RIGHT_FOOT	
	};
	glm::vec3 rotationAxis[] =
	{
		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),	//HEAD		
		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),	//NOSE		
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//NECK		
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//BODY		
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//LEFT_EYE	
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//RIGHT_EYE	
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//LEFT_ARM	
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//RIGHT_ARM	
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//LEFT_FOREARM
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//RIGHT_FOREARM
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//LEFT_UPLEG	
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//RIGHT_UPLEG
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//LEFT_DOWNLEG
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//RIGHT_DOWNLEG
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//LEFT_HAND	
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//RIGHT_HAND	
		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),	//LEFT_FOOT	
		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)	//RIGHT_FOOT	
	};
	float rotationAngle[] =
	{
		0.0f,					0.0f,					//HEAD		
		0.0f,					0.0f,					//NOSE		
		glm::radians(90.0f),	glm::radians(45.0f),	//NECK		
		0.0f,					0.0f,					//BODY		
		glm::radians(90.0f),	0.0f,					//LEFT_EYE	
		glm::radians(90.0f),	0.0f,					//RIGHT_EYE	
		glm::radians(90.0f),	0.0f,					//LEFT_ARM	
		glm::radians(-90.0f),	0.0f,					//RIGHT_ARM	
		glm::radians(90.0f),	0.0f,					//LEFT_FOREARM
		glm::radians(-90.0f),	0.0f,					//RIGHT_FOREARM
		0.0f,					0.0f,					//LEFT_UPLEG	
		0.0f,					0.0f,					//RIGHT_UPLEG
		0.0f,					0.0f,					//LEFT_DOWNLEG
		0.0f,					0.0f,					//RIGHT_DOWNLEG
		glm::radians(90.0f),	0.0f,					//LEFT_HAND	
		glm::radians(-90.0f),	0.0f,					//RIGHT_HAND	
		glm::radians(90.0f),	0.0f,					//LEFT_FOOT	
		glm::radians(-90.0f),	0.0f					//RIGHT_FOOT	
	};

	glm::mat4 model[PARTS];			//arreglo de matrices de transformacion para obtener las partes del cuerpo

	glm::mat4 rot[PARTS];			//arreglo de matrices de rotacion para las normales de las caras

	glm::mat4 liveModel[PARTS];		//arreglo de matrices de transformacion para guardar las rotaciones de las partes del cuerpo
									//dadas por el usuario

	glm::mat4 liveRot[PARTS];		//arreglo de matrices de transformacion para guardar las rotaciones de las normales
									//dadas por el usuario

	glm::mat4 globalModel[PARTS];	//arreglo de matrices de las rotaciones del cuerpo completo de la marioneta 

	glm::mat4 globalRot[PARTS];		//arreglo de matrices de las rotaciones de las normales del cuerpo completo
	
	glm::mat4 globalTrans[PARTS];	//arreglo de matrices de las traslaciones del cuerpo completo

	//inicializamos todas las matrices anteriores a la identidad y aplicamos las transformaciones 
	//de las primitivas para obtener las partes del cuerpo
	for (int i = 0; i < PARTS; i++)
	{
		model[i] = glm::mat4(1.0f);
		rot[i] = glm::mat4(1.0f);
		liveModel[i] = glm::mat4(1.0f);
		liveRot[i] = glm::mat4(1.0f);
		globalModel[i] = glm::mat4(1.0f);
		globalRot[i] = glm::mat4(1.0f);
		globalTrans[i] = glm::mat4(1.0f);
		model[i] = glm::translate(model[i], pos[i]);
		model[i] = glm::scale(model[i], scale[i]);
		model[i] = glm::rotate(model[i], rotationAngle[i * 2 + 1], rotationAxis[i * 2 + 1]);
		rot[i] = glm::rotate(rot[i], rotationAngle[i * 2 + 1], rotationAxis[i * 2 + 1]);
		model[i] = glm::rotate(model[i], rotationAngle[i * 2], rotationAxis[i * 2]);
		rot[i] = glm::rotate(rot[i], rotationAngle[i * 2], rotationAxis[i * 2]);
	}
	
	//activamos los shaders de cada parte del cuerpo y les pasamos las matrices y vectores necesarios para cada una de ellas
	for (int i = 0; i < PARTS; i++)
	{
		shaderPrograms[i].Activate();
		glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[i].ID, "model"), 1, GL_FALSE, glm::value_ptr(model[i]));
		glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[i].ID, "rot"), 1, GL_FALSE, glm::value_ptr(rot[i]));
		glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[i].ID, "liveModel"), 1, GL_FALSE, glm::value_ptr(liveModel[i]));
		glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[i].ID, "liveRot"), 1, GL_FALSE, glm::value_ptr(liveRot[i]));
		glUniform4f(glGetUniformLocation(shaderPrograms[i].ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(shaderPrograms[i].ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	}

	//iniciamos el z-buffer para no dibujar partes que no se vean
	glEnable(GL_DEPTH_TEST);

	//definimos la posicion de la camara
	Camera camera(width, height, glm::vec3(1.0f, 0.0f, 4.5f));
	camera.updateMatrix(45.0f, 0.1f, 100.0f);

	//texturas para la UI
	Texture uiTex("Texture.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	uiTex.texUnit(uiShader, "tex0", 0);

	//mientras no se cierre la ventana, se encarga de los eventos de la misma
	//en cuanto se cierre la ventana terminara nuestro programa
	while (!glfwWindowShouldClose(window)) {
		//color del fondo
		glClearColor(0.52f, 0.20f, 0.74f, 1.0f);
		//limpiamos los buffers de color y de profundidad para volverlos a utilizar
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//recibimos inputs del usuario
		input(window, liveModel, liveRot, globalModel, globalRot, globalTrans);

		//actualizamos los modelos de cada parte del cuerpo, le pasamos su informacion a la camara y la pintamos
		for (int i = 0; i < PARTS; i++)
		{
			shaderPrograms[i].Activate();
			glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[i].ID, "liveModel"), 1, GL_FALSE, glm::value_ptr(multiplyParents(liveModel, i)));
			glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[i].ID, "liveRot"), 1, GL_FALSE, glm::value_ptr(multiplyParents(liveRot, i)));
			glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[i].ID, "globalModel"), 1, GL_FALSE, glm::value_ptr(globalModel[i]));
			glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[i].ID, "globalRot"), 1, GL_FALSE, glm::value_ptr(globalRot[i]));
			glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[i].ID, "globalTrans"), 1, GL_FALSE, glm::value_ptr(globalTrans[i]));
			glUniform3f(glGetUniformLocation(shaderPrograms[i].ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
			camera.Matrix(shaderPrograms[i], "camMatrix");
			puppetVAO[i].Bind();
			glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		}

		//limpiamos el z-buffer para poder poner la UI enfrente
		glClear(GL_DEPTH_BUFFER_BIT);

		//le pasamos la informacion de la UI a la camara y la dibujamos con su textura
		uiShader.Activate();
		camera.Matrix(uiShader, "camMatrix");
		uiTex.Bind();
		uiVAO.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(uiIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

		//pintamos el buffer y hacemos las instrucciones de la ventana
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//elimina los objetos creados
	for (int i = 0; i < PARTS; i++)
	{
		puppetVAO[i].Delete();
		puppetVBO[i].Delete();
		puppetEBO[i].Delete();
		shaderPrograms[i].Delete();
	}
	uiVAO.Delete();
	uiVBO.Delete();
	uiEBO.Delete();
	uiShader.Delete();

	//elimina la ventana antes de terminar con el programa
	glfwDestroyWindow(window);
	//termina con GLFW antes de salir
	glfwTerminate();

	return 0;
}