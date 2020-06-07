//OpenGL 3.0 - Calculates and renders the cartoon model
//Author: Volodymyr Nazarenko (100174968)

#pragma once

using namespace std;
#include <vector>

#include "ProteinRendering\Atoms.h"
#include "GLSL/Shader.h"

#include "gl/glew.h"
#include "gl/wglew.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include <CL/cl.h>
#define MAX_SOURCE_SIZE (0x100000)


class CartoonModel {
public:
	void loadShader(const char* vertPath, const char* fragPath);
	void initCartoonModel(AtomList theList, int colourScheme);
	void drawCartoonModel(glm::mat4 ProjectionMatrix, glm::vec2 xyMove, float ZOOM, float SPIN, bool isTransparent);

private:
	Shader cartoonShader;
	glm::mat4 ModelViewMatrixCartoon;

	GLuint cartoonVBOID[3], cartoonIBO;
	unsigned int cartoonVAOID;

	void calcCubicSpline(vector<Vector3d> points, vector<glm::vec3> norms, vector<glm::vec3> &splineSeg, vector<glm::vec3> &splineSegCAO);

	bool isNextSecondaryStructureID(AtomList theList, string type, int iPos);
	void applyCartoonShaderParams(glm::mat4 ProjectionMatrix, glm::vec2 xyMove, float ZOOM, float SPIN, bool isTransparent);

	void ClearCartoonModelBufferData();
	void addCartoonParams(vector<glm::vec3>& newShapeVerts, vector<glm::vec3>& newNormals, vector<glm::vec3>& newColours);
	void renderCartoonUsingVBO(int shader);
	void renderCartoonVBO(int shader);
	void constructCartoonBuffers(int shader);

	vector<glm::vec3> cartoonVertices;
	vector<glm::vec3> cartoonNormals;
	vector<glm::vec3> cartoonColours;

	bool isCartoonVBOBuilt = false;
	
	int LOD = 10;	//LOD = LOD + 1
	unsigned int numColours = (LOD - 1) * 36;

	vector<vector<glm::vec3>> rainbowColours{
		{vector<glm::vec3>(numColours, glm::vec3(1.0f, 0.0f, 0.0f))},	//Red
		{vector<glm::vec3>(numColours, glm::vec3(1.0f, 0.5f, 0.0f))},	//Orange
		{vector<glm::vec3>(numColours, glm::vec3(1.0f, 1.0f, 0.0f))},	//Yellow
		{vector<glm::vec3>(numColours, glm::vec3(0.0f, 1.0f, 0.0f))},	//Green
		{vector<glm::vec3>(numColours, glm::vec3(0.0f, 0.0f, 1.0f))},	//Blue
		{vector<glm::vec3>(numColours, glm::vec3(0.3f, 0.0f, 0.5f))},	//Purple
		{vector<glm::vec3>(numColours, glm::vec3(0.54f, 0.0f, 0.83f))}	//Violet
	};

protected:
	
};