// deformation_transfer原图.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"
#include <iostream>
#include "Mesh.h"
#include "Helper.hpp"

GLFWwindow *window;
int width = 1024; int height = 768;
Mesh *leftMesh;
Mesh *rightMesh;
float fov = 45.f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

glm::mat4 rotateMatrix = glm::mat4(1.f);
float rotateStep = .2f;

int polygon_mode_switch = 1;


bool creating_line_data = true;

// 原始数据
std::string leftMeshName = "./data/ref_horse.obj";
std::string rightMeshName = "./data/ref_camel.obj";
//std::string corrName = "./data/ref_horse_camel.cons";

// 自测数据
std::string corrName = "./data/cor2.cons";
int main()
{
	using namespace std;
	if (glfwInit() == GL_FALSE)
	{
		cerr << "Failed to initialize GLFW." << endl;
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Deformation Transfer 显示", NULL, NULL);
	if (window == NULL)
	{
		cerr << "Failed to create GLFW window." << endl;
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW." << endl;
		glfwTerminate();
		return -1;
	}


	glfwPollEvents();

	// Import data
	leftMesh = new Mesh(leftMeshName);
	rightMesh = new Mesh(rightMeshName);

	std::vector<TriMesh::Point> leftMarker;
	std::vector<TriMesh::Point> rightMarker;
	Help::readCorresVert(corrName, leftMesh, rightMesh, leftMarker, rightMarker);

	std::vector<TriMesh::Point> together;
	Help::combine(leftMarker, rightMarker, together);

	GLuint meshesVAO[2];
	glGenVertexArrays(2, meshesVAO);
	glBindVertexArray(meshesVAO[0]);

	GLuint meshesVerticesBuffers[2];
	glGenBuffers(2, meshesVerticesBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, meshesVerticesBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, leftMesh->m_vertices.size() * sizeof(TriMesh::Point), leftMesh->m_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	GLuint meshesNormalBuffers[2];
	glGenBuffers(2, meshesNormalBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, meshesNormalBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, leftMesh->m_vert_normals.size() * sizeof(TriMesh::Normal), leftMesh->m_vert_normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	

	GLuint meshesEBO[2];
	glGenBuffers(2, meshesEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshesEBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, leftMesh->m_face_indices.size() * sizeof(unsigned int), leftMesh->m_face_indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	// Right Mesh
	glBindVertexArray(meshesVAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, meshesVerticesBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, rightMesh->m_vertices.size() * sizeof(TriMesh::Point), rightMesh->m_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, meshesNormalBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, rightMesh->m_vert_normals.size() * sizeof(TriMesh::Normal), rightMesh->m_vert_normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshesEBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, rightMesh->m_face_indices.size() * sizeof(unsigned int), rightMesh->m_face_indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);

	// 
	GLuint markersVAO[2];
	glGenVertexArrays(2, markersVAO);

	glBindVertexArray(markersVAO[0]);
	GLuint markersBuffer[2];
	glGenBuffers(2, markersBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, markersBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, leftMarker.size() * sizeof(TriMesh::Point), leftMarker.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glBindVertexArray(markersVAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, markersBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, rightMarker.size() * sizeof(TriMesh::Point), rightMarker.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//
	GLuint lineVAO;


	GLuint leftMeshProgramID = LoadShaders("./Shaders/meshVertShader.glsl", "./Shaders/meshFragShader.glsl");
	GLuint rightMeshProgramID = LoadShaders("./Shaders/meshVertShader.glsl", "./Shaders/meshFragShader.glsl");
	GLuint leftMarkerProgramID = LoadShaders("./Shaders/markerVertShader.glsl", "./Shaders/markerFragShader.glsl");
	GLuint rightMarkerProgramID = LoadShaders("./Shaders/markerVertShader.glsl", "./Shaders/markerFragShader.glsl");
	GLuint lineProgramID = LoadShaders("./Shaders/lineVertShader.glsl", "./Shaders/lineFragShader.glsl");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glPointSize(5.f);

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(polygon_mode_switch == 1)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (polygon_mode_switch == 2)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (polygon_mode_switch == 3)
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / height, 0.1f, 100.f);
		glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		glUseProgram(leftMeshProgramID);
		glm::mat4 leftModel = glm::translate(glm::vec3(-0.5f, -0.5f, 0.f)) * rotateMatrix;
		glm::mat4 leftMvp = projection * view * leftModel;
		GLuint leftMvpLoc = glGetUniformLocation(leftMeshProgramID, "MVP");
		glUniformMatrix4fv(leftMvpLoc, 1, GL_FALSE, &leftMvp[0][0]);
		glBindVertexArray(meshesVAO[0]);
		glDrawElements(GL_TRIANGLES, leftMesh->m_face_indices.size(), GL_UNSIGNED_INT, (void*)0);

		glUseProgram(leftMarkerProgramID);
		GLuint leftMarkerMvpLoc = glGetUniformLocation(leftMarkerProgramID, "MVP");
		glUniformMatrix4fv(leftMarkerMvpLoc, 1, GL_FALSE, &leftMvp[0][0]);
		glBindVertexArray(markersVAO[0]);
		glDrawArrays(GL_POINTS, 0, leftMarker.size());


		glUseProgram(rightMeshProgramID);
		glm::mat4 rightModel = glm::translate(glm::vec3(0.5f, -0.5f, 0.f)) * rotateMatrix;
		glm::mat4 rightMvp = projection * view * rightModel;
		GLuint rightMvpLoc = glGetUniformLocation(rightMeshProgramID, "MVP");
		glUniformMatrix4fv(rightMvpLoc, 1, GL_FALSE, &rightMvp[0][0]);
		glBindVertexArray(meshesVAO[1]);
		glDrawElements(GL_TRIANGLES, rightMesh->m_face_indices.size(), GL_UNSIGNED_INT, (void*)0);

		glUseProgram(rightMarkerProgramID);
		GLuint rightMarkerMvpLoc = glGetUniformLocation(rightMarkerProgramID, "MVP");
		glUniformMatrix4fv(rightMarkerMvpLoc, 1, GL_FALSE, &rightMvp[0][0]);
		glBindVertexArray(markersVAO[1]);
		glDrawArrays(GL_POINTS, 0, rightMarker.size());

		if (creating_line_data == true)
		{
			for (int i = 0; i < together.size(); ++i)
			{
				TriMesh::Point thisOpenMeshPoint = together[i];
				glm::vec4 thisGlmPoint(thisOpenMeshPoint[0], thisOpenMeshPoint[1], thisOpenMeshPoint[2], 1.f);
				if (i % 2 == 0)
					thisGlmPoint = leftModel * thisGlmPoint;
				if (i % 2 != 0)
					thisGlmPoint = rightModel * thisGlmPoint;
					together[i][0] = thisGlmPoint.x;  together[i][1] = thisGlmPoint.y; together[i][2] = thisGlmPoint.z; 
			}

			
			glGenVertexArrays(1, &lineVAO);
			glBindVertexArray(lineVAO);

			GLuint lineVBO;
			glGenBuffers(1, &lineVBO);
			glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
			glBufferData(GL_ARRAY_BUFFER, together.size() * sizeof(TriMesh::Point), together.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);

			creating_line_data = false;
		}

		glUseProgram(lineProgramID);
		GLuint lineMvpLoc = glGetUniformLocation(lineProgramID, "MVP");
		glm::mat4 lineModel = glm::mat4(1.f) * rotateMatrix;
		glm::mat4 lineMvp = projection * view * lineModel;
		glUniformMatrix4fv(lineMvpLoc, 1, GL_FALSE, &lineMvp[0][0]);
		glBindVertexArray(lineVAO);
		glDrawArrays(GL_LINES, 0, together.size());


		glfwPollEvents();
		glfwSwapBuffers(window);

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);


	glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (action != GLFW_PRESS)
		return;
	switch (key)
	{
	case GLFW_KEY_UP:
		//std::cout << "UP" << std::endl;
		rotateMatrix = glm::rotate(rotateStep, glm::vec3(1.f, 0.f, 0.f));
		rotateStep += rotateStep;
		break;
	case GLFW_KEY_DOWN:
		rotateMatrix = glm::rotate(rotateStep, glm::vec3(1.f, 0.f, 0.f));
		rotateStep -= rotateStep;
		break;
	case GLFW_KEY_LEFT:
		rotateMatrix = glm::rotate(rotateStep, glm::vec3(0.f, 1.f, 0.f));
		rotateStep += rotateStep;
		break;
	case GLFW_KEY_RIGHT:
		rotateMatrix = glm::rotate(rotateStep, glm::vec3(0.f, 1.f, 0.f));
		rotateStep -= rotateStep;
		break;
	case GLFW_KEY_P:
		polygon_mode_switch++;
		if (polygon_mode_switch > 3)
			polygon_mode_switch = 1;
		break;

	

	default:
		break;
	}
	creating_line_data = true;
}