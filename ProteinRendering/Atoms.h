#pragma once

#include "../MathUtilities/Vector3d.h"
#include <vector>
class Atom;
#include <string>
#include "../gl/glew.h"
#include "..\glm\glm.hpp"
#include "..\glm\gtc\matrix_transform.hpp"
#include "..\glm\gtc\type_ptr.hpp"

using namespace std;

typedef std::vector<Atom*> AtomList;

class SuperAtom
{
public:
	SuperAtom(){}
	~SuperAtom(){;}

public:
	Vector3d getMyLocalPosition();
	Vector3d* getMyLocalPositionPtr();
	static void renderSphere( float cx, float cy, float cz, float r, int p );
	static void setUpDrawElements( float cx, float cy, float cz, float r, int p );
	static void setUpGeometry( float cx, float cy, float cz, float r, int p );
	static void renderSphereDrawElements( );
	static void constructUnitSphere(int level, int shader);

	static void renderVBO(int shader);
	static void bindbuffers(int shader);
	static void Unbind(int shader);

	void renderUsingVBO(int shader, glm::mat4 ModelViewMatrix);


	


	void render(bool hydrogens, bool waters, bool shadows, AtomList &theList);
	void renderSimple();
	void renderRES(Vector3d colour, bool hydrogens, bool waters, bool shadows, AtomList &theList);
	void renderPoints();
	void init();
	void setMyPosition(const int newX, const int newY, const int newZ);
	void incMyLocalPosition(const float newX, const float newY, const float newZ);
	void readMyPosition(int &readX, int &readY, int &readZ);
	int getMyPositionInOriginalList(){return myPositionInOriginalList;}
	float getRadius(){return radius;}
	float getOriginalRadius(){return origStartingRadius;}
	
	void resetDraw(){beenDrawn = false;}
	bool needsDraw(){return !beenDrawn;}
	
	void calculateCircle(Vector3d C, float R, float r, float &final_x, float &final_r);
	bool SuperAtom::deleteIter(int neighbourID);
	void deletePoint(int firstID, int secondID);

	std::string getMyInfo(){return myInfo;}
	Vector3d getMyColour();
	Vector3d getOrigiPos(){return myOrigLocalPosition;}
	void scaleAtom(float scale, float darklight);
	void subtractVectorFromLocalPosition(Vector3d midPoint);
	void setImWater(bool newState){imWater = newState;}

	void renderH20();
	bool isWater(){return imWater;}

	std::string getEle(){return element;}
	std::string getResidue(){return residue;}
	std::string getResidueName(){return residueName;}
	std::string getChainID(){return chainID;}
	std::string getEntireRes(){return residueName + (residue) + chainID;}

	std::string getSecondaryElementID(){return secondaryElementID;}
	bool getIsLastSecondaryElementID() {return isLastSecondaryElementID;}
	void setSecondaryElementID(std::string id, bool isChainEnd) {secondaryElementID = id; isLastSecondaryElementID = isChainEnd;}
	

	static void makeList();

	static float hapticRadius;
	static float origHapticRadius;
	static float defaultHapticRadius;


	static unsigned int* triIndices;
	static float* vertices;
	static float* normals;
	static int NumberOfTriangles;
	static int NumberOfVertices;
	static GLuint sphereDisplayList;
	static GLuint HR_sphereDisplayList;

	static float* unitSphereVerts;
	static float* norms;
	static float* colors;
	static unsigned int* tInds;
	static int NumberOfVerticesUnitSphere;
	static int NumberOfTrianglesUnitSphere;
	static GLuint m_vboID[3], ibo;
	static unsigned int m_vaoID;		    // vertex array object


	

private:
	
	
protected:

	bool imWater;

	int myAmountOfCones;
	std::string myInfo;
	Vector3d myColour;
	float myDiffuseColour[3];
	float myAmbientColour[3];
	Vector3d myLocalPosition, myOrigLocalPosition;
	Vector3d oldLocalPosition;
	float radius, origRadius;

	bool inContact;
	bool needsUpdating;
	
	Vector3d normal;

	bool beenDrawn;

	static float theta1;
	static float theta2;
	static float theta3;

	static float ex;
	static float ey;
	static float ez;

	static float px;
	static float py;
	static float pz;

	float theDarklight;

	int x;
	int y;
	int z;

	float origStartingRadius;

	int myPositionInOriginalList;

	std::vector<int> myOcclusionCoordinates;

	static float max_dist;

	std::string element;
	std::string residue;
	std::string residueName;
	std::string chainID;
	std::string secondaryElementID = "N/A";
	bool isLastSecondaryElementID;

	float myScale;
};

class Atom : public SuperAtom
{
public:
	Atom(Vector3d position, Vector3d colour, std::string info, float radius, int positionInOriginalList,/* MassList &theMassList,*/ std::string me, std::string theRes, std::string resName, std::string cID);
	~Atom();

};

class HETAtom : public SuperAtom
{
	HETAtom(Vector3d position, Vector3d colour, std::string info, float radius, int positionInOriginalList/*, MassList &theMassList*/);
	~HETAtom();
};