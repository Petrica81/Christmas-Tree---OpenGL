//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul XI - 11_01_umbra.cpp |
// ====================================
// 
//	Program ce deseneaza o casa si care suprinde efectele de umbrire folosindu-se tehnicile MODERN OpenGL; 
//	Elemente de NOUTATE (modelul de iluminare):
//	 - pentru a genera umbra unei surse este utilizata o matrice 4x4;
//	 - in shaderul de varfuri este inclusa si matricea umbrei;
//	 - in shaderul de fragment umbra este colorata separat;
//	 - sursa de lumina este punctuala(varianta de sursa directionala este comentata);
// 
//  
// 
//	Biblioteci
#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <math.h>			//	Biblioteca pentru calcule matematice;
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <ctime>
#include <chrono>
#include <iostream>
#include <GL/freeglut.h>    //	Include functii pentru: 
							//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
							//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
							//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"


//  Identificatorii obiectelor de tip OpenGL; 
GLuint
VaoIdCilinder,
VboIdCilinder,
EboIdCilinder,
VaoIdCube,
EboIdCube,
VboIdCube,
VaoIdCon,
VboIdCon,
EboIdCon,
VaoIdStar,
VboIdStar,
EboIdStar,
VaoIdSphere,
VboIdSphere,
EboIdSphere,
ColorBufferId,
ProgramId,
myMatrixLocation,
matrUmbraLocation,
viewLocation,
projLocation,
matrRotlLocation,
lightColorLocation,
lightPosLocation,
viewPosLocation,
fogColorLocation,
fogFactorLocation,
codColLocation;

int codCol;

// matrice utilizate
glm::mat4 myMatrix, matrRot;

float const PI = 3.141592f;
// Elemente pentru reprezentarea suprafetei
// intervalele pentru parametrii considerati (u si v)
float const U_MIN = -PI, U_MAX = PI, V_MIN = 0, V_MAX = 2 * PI;
// numarul de paralele/meridiane, de fapt numarul de valori ptr parametri
int const NR_PARR = 30, NR_MERID = 30;
// pasul cu care vom incrementa u, respectiv v
float step_u = (U_MAX - U_MIN) / NR_PARR, step_v = (V_MAX - V_MIN) / NR_MERID;

// alte variabile
float radius = 50, radiusSphere = 10;
int index, index_aux;

// elemente pentru matricea de vizualizare
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = PI / 8, beta = 0.0f, dist = 1500.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0, Vy = 0.0, Vz = 1.0;
glm::mat4 view;

// elemente pentru matricea de proiectie
float width = 800, height = 600, xwmin = -800.f, xwmax = 800, ywmin = -600, ywmax = 600, znear = 0.1, zfar = 1, fov = 45;
glm::mat4 projection;

// sursa de lumina
float xL = 0.f, yL = 0.f, zL = 430.f;

// matricea umbrei
float matrUmbra[4][4];
int nrUmbra = 0;
GLfloat VerticesCon[] ={
	// varfuri con
	 -40.0f, -69.28f, 0.0f, 1.0f,   0.1f, 1.0f, 0.2f, -40.0f, -69.28f, 80.0f,
	 40.0f, -69.28f, 0.0f, 1.0f,    0.1f, 1.0f, 0.2f, 40.0f, -69.28f, 80.0f,
	 80.0f, 0.0f, 0.0f, 1.0f,       0.1f, 1.0f, 0.2f, 80.0f, 0.0f, 80.0f,
	 40.0f, 69.28f, 0.0f, 1.0f,     0.1f, 1.0f, 0.2f, 40.0f, 69.28f, 80.0f,
	-40.0f, 69.28f, 0.0f, 1.0f,     0.1f, 1.0f, 0.2f, -40.0f, 69.28f, 80.0f,
	-80.0f, 0.0f,  0.0f, 1.0f,      0.1f, 1.0f, 0.2f, -80.0f, 0.0f, 80.0f,
	  0.0f, 0.0f, 100.0f, 1.0f,     0.3f, 1.0f, 0.2f, 0.0f, 0.0f, 1.0f,
};

void processNormalKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'l':
		Vx -= 0.1;
		break;
	case 'r':
		Vx += 0.1;
		break;
	case '+':
		dist += 5;
		break;
	case '-':
		dist -= 5;
		break;
	}
	if (key == 27)
		exit(0);
}
void processSpecialKeys(int key, int xx, int yy)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		beta -= 0.01;
		break;
	case GLUT_KEY_RIGHT:
		beta += 0.01;
		break;
	case GLUT_KEY_UP:
		alpha += 0.01;
		break;
	case GLUT_KEY_DOWN:
		alpha -= 0.01;
		break;
	}
}
void CreateVBOCube(void)
{
	// varfurile 
	GLfloat Vertices[] =
	{
		// coordonate                   // culori			// normale
		-50.0f,  -50.0f, 50.0f, 1.0f,   1.0f, 0.5f, 0.2f,  -1.0f, -1.0f, -1.0f,
		 50.0f,  -50.0f,  50.0f, 1.0f,  1.0f, 0.5f, 0.2f,  1.0f, -1.0f, -1.0f,
		 50.0f,  50.0f,  50.0f, 1.0f,   1.0f, 0.5f, 0.2f,  1.0f, 1.0f, -1.0f,
		-50.0f,  50.0f, 50.0f, 1.0f,    1.0f, 0.5f, 0.2f,  -1.0f, 1.0f, -1.0f,
		-50.0f,  -50.0f, 150.0f, 1.0f,  1.0f, 0.5f, 0.2f,  -1.0f, -1.0f, 1.0f,
		 50.0f,  -50.0f,  150.0f, 1.0f, 1.0f, 0.5f, 0.2f,  1.0f, -1.0f, 1.0f,
		 50.0f,  50.0f,  150.0f, 1.0f,  1.0f, 0.5f, 0.2f,  1.0f, 1.0f, 1.0f,
		-50.0f,  50.0f, 150.0f, 1.0f,   1.0f, 0.5f, 0.2f,  -1.0f, 1.0f, 1.0f,

	};

	// indicii pentru varfuri
	GLubyte Indices[] =
	{
		 1, 2, 0,   2, 0, 3,
		 2, 3, 6,   6, 3, 7,
		 7, 3, 4,   4, 3, 0,
		 4, 0, 5,   5, 0, 1,
		 1, 2, 5,   5, 2, 6,
		 5, 6, 4,   4, 6, 7,

	};

	glGenVertexArrays(1, &VaoIdCube);
	glGenBuffers(1, &VboIdCube);
	glGenBuffers(1, &EboIdCube);
	glBindVertexArray(VaoIdCube);

	glBindBuffer(GL_ARRAY_BUFFER, VboIdCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboIdCube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	// atributul 2 = normale
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}
void CreateVBOCilinder(void)
{
	// varfurile 
	glm::vec4 Vertices[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Normals[(NR_PARR + 1) * NR_MERID];
	GLushort Indices[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u; // valori pentru u si v
			float v = V_MIN + merid * step_v;
			float x_vf = radius/merid * cosf(u); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius/merid * sinf(u);
			float z_vf = v * 70;
			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors[index] = glm::vec3(0.3f, 0.1f, 0.1f);
			// Colors[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));
			Normals[index] = glm::vec3(x_vf, y_vf, z_vf);
			Indices[index] = index;
			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices[(NR_PARR + 1) * NR_MERID + index_aux] = index;
			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices[AUX + 4 * index + 1] = index2;
				Indices[AUX + 4 * index + 2] = index3;
				Indices[AUX + 4 * index + 3] = index4;
			}
		}
	};

	glGenVertexArrays(1, &VaoIdCilinder);
	glGenBuffers(1, &VboIdCilinder);
	glGenBuffers(1, &EboIdCilinder);
	glBindVertexArray(VaoIdCilinder);

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboIdCilinder);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboIdCilinder);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors) + sizeof(Normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors), sizeof(Normals), Normals);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices));
	glEnableVertexAttribArray(2); // atributul 2 = normala
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices) + sizeof(Colors)));
}
void CreateVBOCon(void)
{

	// indicii pentru varfuri
	GLubyte Indices[] =
	{
		 // fetele conului
		 0, 1, 6,
		 1, 2, 6,
		 2, 3, 6,
		 3, 4, 6,
		 4, 5, 6,
		 5, 0, 6
	};
	
	glGenVertexArrays(1, &VaoIdCon);
	glGenBuffers(1, &VboIdCon);
	glGenBuffers(1, &EboIdCon);
	glBindVertexArray(VaoIdCon);


	glBindBuffer(GL_ARRAY_BUFFER, VboIdCon);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesCon), VerticesCon, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboIdCon);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	// atributul 2 = normale
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}
void CreateVBOStar(void)
{
	// varfurile 
	GLfloat Vertices[] =
	{
		// varfuri stea
		  0.0f, 0.0f, 50.0f, 1.0f,    1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 51.0f, // sus 0
		-10.0f, 0.0f, 10.0f, 1.0f,    1.0f, 1.0f, 0.0f, -11.0f, 0.0f, 11.0f, // sus stanga interior 1
		 10.0f, 0.0f, 10.0f, 1.0f,    1.0f, 1.0f, 0.0f, 11.0f, 0.0f, 11.0f, // sus dreapta interior 2 
		-50.0f, 0.0f, 10.0f, 1.0f,    1.0f, 1.0f, 0.0f, -51.0f, 0.0f, 11.0f, // stanga sus exterior 3
		 50.0f, 0.0f, 10.0f, 1.0f,    1.0f, 1.0f, 0.0f, 51.0f, 0.0f, 11.0f, //dreapta sus exterior 4
		-20.0f, 0.0f, -15.0f, 1.0f,   1.0f, 1.0f, 0.0f, -21.0f, 0.0f, -16.0f, //  stanga jos interior 5
		 20.0f, 0.0f, -15.0f, 1.0f,   1.0f, 1.0f, 0.0f, 21.0f, 0.0f, -16.0f, // dreapta jos interior 6
		-30.0f, 0.0f, -50.0f, 1.0f,   1.0f, 1.0f, 0.0f, -31.0f, 0.0f, -51.0f, // stanga jos exterior 7
		 30.0f, 0.0f, -50.0f, 1.0f,   1.0f, 1.0f, 0.0f, 31.0f, 0.0f, -51.0f, // dreapta jos exterior 8
		  0.0f, 0.0f, -25.0f, 1.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -26.0f, // jos 9
		  0.0f, 20.0f, -5.0f, 1.0f,   1.0f, 1.0f, 0.0f, 0.0f, 21.0f, -6.0f, // mijloc fata 10
		  0.0f, -20.0f, -5.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, -21.0f, -6.0f, // mijloc spate 11
	};

	// indicii pentru varfuri
	GLubyte Indices[] =
	{
		// fetele stelei
		//fate fata
		0, 10, 1,
		0, 10, 2,
		1, 10, 3,
		2, 10, 4,
		3, 10, 5,
		4, 10, 6,
		5, 10, 7,
		6, 10, 8,
		7, 10, 9,
		8, 10, 9,
		//fate spate
		0, 11, 1,
		0, 11, 2,
		1, 11, 3,
		2, 11, 4,
		3, 11, 5,
		4, 11, 6,
		5, 11, 7,
		6, 11, 8,
		7, 11, 9,
		8, 11, 9
	};

	glGenVertexArrays(1, &VaoIdStar);
	glGenBuffers(1, &VboIdStar);
	glGenBuffers(1, &EboIdStar);
	glBindVertexArray(VaoIdStar);

	glBindBuffer(GL_ARRAY_BUFFER, VboIdStar);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboIdStar);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	// atributul 2 = normale
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}
void CreateVBOSphere(void) 
{
	// varfurile 
	// Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Normals[(NR_PARR + 1) * NR_MERID];
	GLushort Indices[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u; // valori pentru u si v
			float v = V_MIN + merid * step_v;
			float x_vf = radiusSphere * cosf(u) * cosf(v); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radiusSphere * cosf(u) * sinf(v);
			float z_vf = radiusSphere * sinf(u) + 0.05 * radius * rand() / RAND_MAX;
			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors[index] = glm::vec3(0.96 + 0.1f * sinf(u) * cosf(v), 0.71 + 0.1f * cosf(u) * sinf(v), 0.1);
			// Colors[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));
			Normals[index] = glm::vec3(x_vf, y_vf, z_vf);
			Indices[index] = index;
			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices[(NR_PARR + 1) * NR_MERID + index_aux] = index;
			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices[AUX + 4 * index + 1] = index2;
				Indices[AUX + 4 * index + 2] = index3;
				Indices[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenVertexArrays(1, &VaoIdSphere);
	glGenBuffers(1, &VboIdSphere);
	glGenBuffers(1, &EboIdSphere);
	glBindVertexArray(VaoIdSphere);

	glBindBuffer(GL_ARRAY_BUFFER, VboIdSphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboIdSphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors) + sizeof(Normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors), sizeof(Normals), Normals);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices));
	glEnableVertexAttribArray(2); // atributul 2 = normala
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices) + sizeof(Colors)));
}
void DestroyVBO(void)
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboIdCilinder);
	glDeleteBuffers(1, &EboIdCilinder);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoIdCilinder);
	glDeleteBuffers(1, &VboIdCube);
	glDeleteBuffers(1, &EboIdCube);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoIdCube);
	glDeleteBuffers(1, &VboIdCon);
	glDeleteBuffers(1, &EboIdCon);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoIdCon);
	glDeleteBuffers(1, &VboIdStar);
	glDeleteBuffers(1, &EboIdStar);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoIdStar);
	glDeleteBuffers(1, &VboIdSphere);
	glDeleteBuffers(1, &EboIdSphere);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoIdSphere);

}

void CreateShaders(void)
{
	ProgramId = LoadShaders("11_01_Shader.vert", "11_01_Shader.frag");
	glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	myMatrix = glm::mat4(1.0f);
	matrRot = glm::rotate(glm::mat4(1.0f), PI / 8, glm::vec3(0.0, 0.0, 1.0));
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	CreateVBOCilinder();
	CreateVBOCube();
	CreateVBOCon();
	CreateVBOStar();
	CreateVBOSphere();
	CreateShaders();
	// locatii pentru shader-e
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	matrUmbraLocation = glGetUniformLocation(ProgramId, "matrUmbra");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	lightColorLocation = glGetUniformLocation(ProgramId, "lightColor");
	lightPosLocation = glGetUniformLocation(ProgramId, "lightPos");
	viewPosLocation = glGetUniformLocation(ProgramId, "viewPos");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	fogFactorLocation = glGetUniformLocation(ProgramId, "fogFactor");
	fogColorLocation = glGetUniformLocation(ProgramId, "fogColor");
}

void DrawSphere(float posX = 0.f, float posY = 0.f, float posZ = 0.f, float rotX = 1.f, float rotY = 0.f, float rotZ = 0.f, float angle = 0.f, float scaleX = 1.f, float scaleY = 1.f, float scaleZ = 1.f, bool shadow = true, int col = 0)
{
	// matricea pentru umbra
	float D = -2.f - (0.1f * nrUmbra);
	matrUmbra[0][0] = zL + D; matrUmbra[0][1] = 0; matrUmbra[0][2] = 0; matrUmbra[0][3] = 0;
	matrUmbra[1][0] = 0; matrUmbra[1][1] = zL + D; matrUmbra[1][2] = 0; matrUmbra[1][3] = 0;
	matrUmbra[2][0] = -xL; matrUmbra[2][1] = -yL; matrUmbra[2][2] = D; matrUmbra[2][3] = -1;
	matrUmbra[3][0] = -D * xL; matrUmbra[3][1] = -D * yL; matrUmbra[3][2] = -D * zL; matrUmbra[3][3] = zL;
	glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);

	//desenare corp stea
	codCol = col;
	glUniform1i(codColLocation, codCol);
	glBindVertexArray(VaoIdSphere);

	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
	myMatrix = glm::rotate(myMatrix, angle, glm::vec3(rotX, rotY, rotZ));
	myMatrix = glm::scale(myMatrix, glm::vec3(scaleX, scaleY, scaleZ));
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}


	if (shadow)
	{
		nrUmbra++;
		//desenare umbra stea
		codCol = 1;
		glUniform1i(codColLocation, codCol);

		for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
		{
			if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
				glDrawElements(
					GL_QUADS,
					4,
					GL_UNSIGNED_SHORT,
					(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
		}

	}

}
void DrawCilinder(float posX = 0.f, float posY = 0.f, float posZ = 0.f, float rotX = 1.f, float rotY = 0.f, float rotZ = 0.f, float angle = 0.f, float scaleX = 1.f, float scaleY = 1.f, float scaleZ = 1.f)
{
	// matricea pentru umbra
	float D = -2.f - (0.1f * nrUmbra);
	matrUmbra[0][0] = zL + D; matrUmbra[0][1] = 0; matrUmbra[0][2] = 0; matrUmbra[0][3] = 0;
	matrUmbra[1][0] = 0; matrUmbra[1][1] = zL + D; matrUmbra[1][2] = 0; matrUmbra[1][3] = 0;
	matrUmbra[2][0] = -xL; matrUmbra[2][1] = -yL; matrUmbra[2][2] = D; matrUmbra[2][3] = -1;
	matrUmbra[3][0] = -D * xL; matrUmbra[3][1] = -D * yL; matrUmbra[3][2] = -D * zL; matrUmbra[3][3] = zL;
	glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);
	//desenare corp cilindru
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	glBindVertexArray(VaoIdCilinder);

	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
	myMatrix = glm::rotate(myMatrix, angle ,glm::vec3(rotX, rotY, rotZ));
	myMatrix = glm::scale(myMatrix, glm::vec3(scaleX,scaleY,scaleZ));
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0)
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}

	nrUmbra++;
	//desenare umbra cilindru
	codCol = 1;
	glUniform1i(codColLocation, codCol);

	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}

}
void DrawCube(float posX = 0.f, float posY = 0.f, float posZ = 0.f, float rotX = 1.f, float rotY = 0.f, float rotZ = 0.f, float angle = 0.f, float scaleX = 1.f, float scaleY = 1.f, float scaleZ = 1.f, bool shadow = true, int col = 0)
{
	// matricea pentru umbra
	float D = -2.f - (0.1f * nrUmbra);
	matrUmbra[0][0] = zL + D; matrUmbra[0][1] = 0; matrUmbra[0][2] = 0; matrUmbra[0][3] = 0;
	matrUmbra[1][0] = 0; matrUmbra[1][1] = zL + D; matrUmbra[1][2] = 0; matrUmbra[1][3] = 0;
	matrUmbra[2][0] = -xL; matrUmbra[2][1] = -yL; matrUmbra[2][2] = D; matrUmbra[2][3] = -1;
	matrUmbra[3][0] = -D * xL; matrUmbra[3][1] = -D * yL; matrUmbra[3][2] = -D * zL; matrUmbra[3][3] = zL;
	glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);
	//desenare corp cub
	codCol = col;
	glUniform1i(codColLocation, codCol);
	glBindVertexArray(VaoIdCube);

	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
	myMatrix = glm::rotate(myMatrix, angle, glm::vec3(rotX, rotY, rotZ));
	myMatrix = glm::scale(myMatrix, glm::vec3(scaleX, scaleY, scaleZ));
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void*)(0));

	if(shadow)
	{
		nrUmbra++;
		//desenare umbra cub
		codCol = 1;
		glUniform1i(codColLocation, codCol);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void*)(0));
	}

}
void DrawCon(float posX = 0.f, float posY = 0.f, float posZ = 0.f, float rotX = 1.f, float rotY = 0.f, float rotZ = 0.f, float angle = 0.f, float scaleX = 1.f, float scaleY = 1.f, float scaleZ = 1.f, bool shadow = true)
{
	// matricea pentru umbra
	float D = -2.f - (0.1f * nrUmbra);
	matrUmbra[0][0] = zL + D; matrUmbra[0][1] = 0; matrUmbra[0][2] = 0; matrUmbra[0][3] = 0;
	matrUmbra[1][0] = 0; matrUmbra[1][1] = zL + D; matrUmbra[1][2] = 0; matrUmbra[1][3] = 0;
	matrUmbra[2][0] = -xL; matrUmbra[2][1] = -yL; matrUmbra[2][2] = D; matrUmbra[2][3] = -1;
	matrUmbra[3][0] = -D * xL; matrUmbra[3][1] = -D * yL; matrUmbra[3][2] = -D * zL; matrUmbra[3][3] = zL;
	glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);
	//desenare corp con
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	glBindVertexArray(VaoIdCon);

	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
	myMatrix = glm::rotate(myMatrix, angle, glm::vec3(rotX, rotY, rotZ));
	myMatrix = glm::scale(myMatrix, glm::vec3(scaleX, scaleY, scaleZ));
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, (void*)(0));

	if (shadow)
	{
		nrUmbra++;
		//desenare umbra con
		codCol = 1;
		glUniform1i(codColLocation, codCol);

		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, (void*)(0));
	}

	int seconds = glutGet(GLUT_ELAPSED_TIME) / 1000;
	for (int i = 0; i < 6; ++i)
	{

		float spherePosX = VerticesCon[i * 10] * scaleX + posX;      // Coordonata x a vârfului inferior
		float spherePosY = VerticesCon[i * 10 + 1] * scaleY + posY;  // Coordonata y a vârfului inferior
		float spherePosZ = VerticesCon[i * 10 + 2] * scaleZ + posZ;  // Coordonata z a vârfului inferior

		// Alegere cod de culoare între 5 și 12
		int sphereColorCode = ((int)(abs(spherePosZ) + i + seconds) % 7 + 5);

		// Desenare sferă cu codul de culoare ales
		DrawSphere(spherePosX, spherePosY, spherePosZ, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, false, sphereColorCode);
	}
}
void DrawStar(float posX = 0.f, float posY = 0.f, float posZ = 0.f, float rotX = 1.f, float rotY = 0.f, float rotZ = 0.f, float angle = 0.f, float scaleX = 1.f, float scaleY = 1.f, float scaleZ = 1.f, bool shadow = true)
{
	// matricea pentru umbra
	float D = -2.f - (1.f * nrUmbra);
	matrUmbra[0][0] = zL + D; matrUmbra[0][1] = 0; matrUmbra[0][2] = 0; matrUmbra[0][3] = 0;
	matrUmbra[1][0] = 0; matrUmbra[1][1] = zL + D; matrUmbra[1][2] = 0; matrUmbra[1][3] = 0;
	matrUmbra[2][0] = -xL; matrUmbra[2][1] = -yL; matrUmbra[2][2] = D; matrUmbra[2][3] = -1;
	matrUmbra[3][0] = -D * xL; matrUmbra[3][1] = -D * yL; matrUmbra[3][2] = -D * zL; matrUmbra[3][3] = zL;
	glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);

	//desenare corp stea
	codCol = -1;
	glUniform1i(codColLocation, codCol);
	glBindVertexArray(VaoIdStar);

	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
	myMatrix = glm::rotate(myMatrix, angle, glm::vec3(rotX, rotY, rotZ));
	myMatrix = glm::scale(myMatrix, glm::vec3(scaleX, scaleY, scaleZ));
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_BYTE, (void*)(0));

	if (shadow)
	{
		// matricea pentru umbra
		nrUmbra++;
		//desenare umbra stea
		codCol = 1;
		glUniform1i(codColLocation, codCol);

		glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_BYTE, (void*)(0));
	}

}
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//pozitia observatorului
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	glUniform3f(fogColorLocation, 0.5f, 0.5f, 0.5f);
	glUniform1f(fogFactorLocation, 0.00005f);

	nrUmbra = 0;
	// matrice de vizualizare + proiectie
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);   // se schimba pozitia observatorului	
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz); // pozitia punctului de referinta
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz); // verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);


	// Variabile uniforme pentru iluminare
	glUniform3f(lightColorLocation, 1.0f, 1.0f, 0.0f);
	glUniform3f(lightPosLocation, xL, yL, zL);
	glUniform3f(viewPosLocation, Obsx, Obsy, Obsz);

	
	DrawCilinder(0.f, 0.f, -10.f);
	DrawCube(0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 7.f, 7.f, 0.0001f, false, -2);
	DrawCon(0.f, 0.f, 50.f, 1.f, 0.f, 0.f, 0.f, 2.f, 2.f, 2.f);
	DrawCon(0.f, 0.f, 200.f, 1.f, 0.f, 0.f, 0.f, 1.5f, 1.5f, 1.5f);
	DrawCon(0.f, 0.f, 300.f);
	DrawCube(300.f, 300.f, -50.f, 1.f, 0.f, 0.f, 0.f, 0.5f, 0.5f, 1.f, true, 2);
	DrawCube(-300.f, 300.f, -30.f, 1.f, 0.f, 0.f, 0.f, 0.5f, 0.3f, 0.6f, true, 3);
	DrawCube(-250.f, -150.f, -40.f, 1.f, 0.f, 0.f, 0.f, 2.f, 0.5f, 0.9f, true, 4);
	DrawCube(250.f, 200.f, -40.f, 1.f, 0.f, 0.f, 0.f, 0.5f, 0.5f, 0.8f, true, 3);
	DrawCube(200.f, 250.f, -40.f, 1.f, 0.f, 0.f, 0.f, 0.5f, 0.5f, 0.8f, true, 2);
	DrawCube(-180.f, 250.f, -50.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, true, 2);
	DrawStar(0.f, 0.f, 435.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, false);

	beta += 0.005;

	glutSwapBuffers();
	glFlush();
}
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1200, 900);
	glutCreateWindow("Iluminare - Umbre - OpenGL <<nou>>");
	glewInit();
	Initialize();
	glutIdleFunc(RenderFunction);
	glutDisplayFunc(RenderFunction);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

