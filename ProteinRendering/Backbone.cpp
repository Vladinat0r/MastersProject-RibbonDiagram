#include "Backbone.h"

#include "../MathUtilities/MatrixRoutines.h"

Backbone::Backbone()
{
}
Backbone::~Backbone()
{
}
void Backbone::init()
{
	cylDisplayList = glGenLists(1);
	glNewList(cylDisplayList, GL_COMPILE);
			cyl = gluNewQuadric();
			gluQuadricDrawStyle(cyl, GLU_FILL);
			gluCylinder(cyl, 2, 2, 1, 20, 20);
	glEndList();

	sphDisplayList = glGenLists(1);
	glNewList(sphDisplayList, GL_COMPILE);
			sph = gluNewQuadric();
			gluQuadricDrawStyle(sph, GLU_FILL);
			gluSphere(sph, 2.2, 20, 20);
	glEndList();
}
void Backbone::addCoord(Vector3d newCoord, std::string newChainID)
{
	BackboneData newData;
	newData.coord = newCoord;
	newData.chainID = newChainID;
	listOfCoords.push_back(newData);
}
void Backbone::drawLine(Vector3d coordStart, Vector3d coordEnd)
{
	glBegin(GL_LINES);

		glVertex3f(coordStart.x, coordStart.y, coordStart.z);
		glVertex3f(coordEnd.x, coordEnd.y, coordEnd.z);
			
	glEnd();
}
void Backbone::calcDistances()
{
	int length = listOfCoords.size();
	for(int i = 0; i < length-1; i++)
	{
		BackboneData tempDataStart = listOfCoords[i];
		BackboneData tempDataEnd = listOfCoords[i+1];
		listOfCoords[i].dist = (tempDataStart.coord/10).euclideanDistance((tempDataEnd.coord/10));
		//cout << tempDataStart.dist << endl;
	}
}
void Backbone::subtractVectorFromLocalPosition(Vector3d toDo)
{
	for(unsigned i = 0; i < listOfCoords.size(); i++)
	{
		listOfCoords[i].coord.x -= toDo.x;
		listOfCoords[i].coord.y -= toDo.y;
		listOfCoords[i].coord.z -= toDo.z;

	}
}
void Backbone::render(bool colourChains)
{
	int length = listOfCoords.size();
	//cout << "length" << length << endl;
	std::string currentChain = listOfCoords[0].chainID;
	Vector3d colours[] = {Vector3d (0.0f,0.0f,1.0f), Vector3d (1.0f,0.0f,0.0f), Vector3d (0.0f,1.0f,1.0f), Vector3d (0.3f,1.0f,0.0f), Vector3d (0.8f,0.0f,1.0f),
							Vector3d (0.0f,0.5f,1.0f), Vector3d (0.5f,0.0f,1.0f), Vector3d (0.8f,0.3f,0.0f), Vector3d (0.6f,0.7f,1.0f)};
	Vector3d currentColour = colours[0];
	int current = 0;

	//glDisable(GL_CULL_FACE);
	for(int i = 0; i < length-1; i++)
		{
			

			BackboneData tempDataStart = listOfCoords[i];
			BackboneData tempDataEnd = listOfCoords[i+1];
			
			if( (tempDataStart.dist < 4) && (tempDataStart.dist > 3) )
			{
				if(  (tempDataStart.chainID.compare(currentChain) != 0) )
				{
					currentColour = colours[++current];
					currentChain = tempDataStart.chainID;
					
				}

				
				if(colourChains)
				{
					glColor3f(currentColour.x, currentColour.y, currentColour.z);
				}
				else
					glColor3f(0.5f, 0.5f, 0.5f);

				glPushMatrix();
				glTranslatef(tempDataStart.coord.x, tempDataStart.coord.y, tempDataStart.coord.z);

				glCallList(sphDisplayList);
				
				Vector3d dir = tempDataEnd.coord - tempDataStart.coord;
				dir.normalize();

				Vector3d v(1,0,0);

				Vector3d axis = Vector3d::crossProduct(dir, v);
				double angle = acos((Vector3d::dotProduct(dir, v)));

				axis.normalize();
				
				//determine yaxis
				Quaternion qY;
				qY.CreateFromAxisAngle(axis.x,axis.y,axis.z,90);

				double xPoint[3] = {1,0,0};
				double yPoint[3] = {0,0,0};
				MatrixRoutines<double>::MultiplyRotationVector(xPoint,qY.getMatrix(),yPoint);

				if(dir.x*yPoint[0]+dir.y*yPoint[1]+dir.z*yPoint[2] < 0)
				{
					angle = (2*3.1415926535897932384626433832795)-angle;
				}

				Quaternion q;

				q.CreateFromAxisAngle(axis.x, axis.y, axis.z, angle*(180.0/3.1415926535897932384626433832795));
				glMultMatrixd(q.getMatrix());
				//glRotatef(angle*(180/3.141),0,0,1);
				glRotatef(90,0,1,0);
				glScalef(1.0f,1.0f, tempDataStart.dist*10.0f);
				//cout << tempDataStart.chainID << endl;
				

				glCallList(cylDisplayList);
				glPopMatrix();
			}
			

			

		}
	//glEnable(GL_CULL_FACE);
}


