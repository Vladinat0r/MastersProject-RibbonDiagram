#pragma once;

#include <iostream>
#include <map>
#include <vector>
#include "../MathUtilities/Vector3d.h"
#include "../MathUtilities/Quaternion.h"

#include "../gl/glew.h"

struct BackboneData
{
	Vector3d coord;
	std::string chainID;
	float dist;
};

class Backbone
{
public:
	Backbone();
	~Backbone();
	void init();
	void addCoord(Vector3d newCoord, std::string newChainID);
	void render(bool colourChains);
	void subtractVectorFromLocalPosition(Vector3d toDo);
	void calcDistances();
private:
	void drawLine(Vector3d coordStart, Vector3d coordEnd);
	GLuint cylDisplayList, sphDisplayList;
	GLUquadricObj* cyl, *sph;

	std::vector<BackboneData> listOfCoords;

};