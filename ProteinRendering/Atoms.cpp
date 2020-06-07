#include "Atoms.h"


/***********************************************************************/
#include <boost/lambda/lambda.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/config.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/const_iterator.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/detail/predicate.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
/*******************************/

#include "../MathUtilities/MatrixRoutines.h"

#include "../gl/glew.h"

const float INITIAL_SCALE = 10.0;

float SuperAtom::theta1 = 0.0f;
float SuperAtom::theta2 = 0.0f;
float SuperAtom::theta3 = 0.0f;

float SuperAtom::ex = 0.0f;
float SuperAtom::ey = 0.0f;
float SuperAtom::ez = 0.0f;

float SuperAtom::px = 0.0f;
float SuperAtom::py = 0.0f;
float SuperAtom::pz = 0.0f;

float* SuperAtom::normals = NULL;
float* SuperAtom::vertices = NULL;
unsigned int* SuperAtom::triIndices = NULL;
int SuperAtom::NumberOfTriangles = 0;
int SuperAtom::NumberOfVertices = 0;

float SuperAtom::hapticRadius = (1.52f*INITIAL_SCALE);
float SuperAtom::origHapticRadius = (1.52f*INITIAL_SCALE);
float SuperAtom::defaultHapticRadius = (1.52f*INITIAL_SCALE);

GLuint SuperAtom::sphereDisplayList = 0;
GLuint SuperAtom::HR_sphereDisplayList = 0;

float darklight[] = { -10.0f, -10.0f, -10.0f, -10.0f};
float lightSpe0[] = {1.1f, 1.1f, 1.1f, 1.1f};
const GLfloat spot_exp = 1.0f;

using boost::find_first;

const float TWOPI  = 6.28318530717958f;
const float PIDIV2 = 1.57079632679489f;

float SuperAtom::max_dist = 12.0f;

const int MAX_ARRAY_SIZE = 1000;

int SuperAtom::NumberOfTrianglesUnitSphere = 0;
int SuperAtom::NumberOfVerticesUnitSphere = 0;
float* SuperAtom::unitSphereVerts = NULL;
float* SuperAtom::colors = NULL;
float* SuperAtom::norms = NULL;
unsigned int* SuperAtom::tInds = NULL;
GLuint SuperAtom::m_vboID[3];
GLuint SuperAtom::ibo;
unsigned int SuperAtom::m_vaoID;		    // vertex array object

Vector3d SuperAtom::getMyLocalPosition()
{
	return myLocalPosition;
}
Vector3d SuperAtom::getMyColour()
{
	return myColour;
}
Vector3d* SuperAtom::getMyLocalPositionPtr()
{
	return &myLocalPosition;
}
void SuperAtom::renderH20()
{
	glPushMatrix();

		float mcolorFront[] = { 0.25f, 0.9f, 1.0f, 0.6f};
		glMaterialfv(GL_FRONT, GL_AMBIENT, mcolorFront);

		glMaterialfv(GL_BACK, GL_AMBIENT, mcolorFront);
		
		glColor4f(0.25f, 0.9f, 1.0f, 0.6f);
		glTranslatef(myLocalPosition.x, myLocalPosition.y, myLocalPosition.z);
		glScalef(SuperAtom::hapticRadius, SuperAtom::hapticRadius, SuperAtom::hapticRadius);
		glCallList(sphereDisplayList);
	glPopMatrix();

}
void SuperAtom::renderRES(Vector3d colour, bool hydrogens, bool waters, bool shadows, AtomList &theList)
{
	if(shadows)
	{
		for(unsigned j = 0; (j < 8) && (j < myOcclusionCoordinates.size() ); j++ )
		{
			int tempID = myOcclusionCoordinates[j];

			Atom* tempAtom = theList[tempID]; 
			Vector3d temp = tempAtom->getMyLocalPosition();

			if((tempAtom->getEle().compare("H")) == 0)
			{
				if(hydrogens)
				{
					float lightPos0[] = {temp.x, temp.y, temp.z, 1.0f};

					glLightfv(GL_LIGHT0 + j, GL_AMBIENT, darklight);
					glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, darklight);
					glLightfv(GL_LIGHT0 + j, GL_SPECULAR, darklight);
					glLightfv(GL_LIGHT0 + j, GL_POSITION, lightPos0);
					glLightf(GL_LIGHT0 + j, GL_QUADRATIC_ATTENUATION,  1.0f);
				}
			}
			else if(tempAtom->isWater())
			{
				if(waters)
				{
					float lightPos0[] = {temp.x, temp.y, temp.z, 1.0f};

			
					glLightfv(GL_LIGHT0 + j, GL_AMBIENT, darklight);
					glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, darklight);
					glLightfv(GL_LIGHT0 + j, GL_SPECULAR, darklight);
					glLightfv(GL_LIGHT0 + j, GL_POSITION, lightPos0);

					glLightf(GL_LIGHT0 + j, GL_QUADRATIC_ATTENUATION,  1.0f);

				}
			}
			else
			{
				float lightPos0[] = {temp.x, temp.y, temp.z, 1.0f};

				
				glLightfv(GL_LIGHT0 + j, GL_AMBIENT, darklight);
				glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, darklight);
				glLightfv(GL_LIGHT0 + j, GL_SPECULAR, darklight);
				glLightfv(GL_LIGHT0 + j, GL_POSITION, lightPos0);
				glLightf(GL_LIGHT0 + j, GL_QUADRATIC_ATTENUATION,  1.0f);
			}


		}
	}

	glPushMatrix();
		float mcolorFront[] = { colour.x, colour.y, colour.z, 1.0f };

		glMaterialfv(GL_FRONT, GL_AMBIENT, mcolorFront);

		glMaterialfv(GL_BACK, GL_AMBIENT, mcolorFront);
		
		glTranslatef(myLocalPosition.x, myLocalPosition.y, myLocalPosition.z);
		glColor3f(colour.x, colour.y, colour.z);
		
		glScalef(radius-hapticRadius, radius-hapticRadius, radius-hapticRadius);
		glCallList(sphereDisplayList);
	glPopMatrix();
}


void SuperAtom::renderUsingVBO(int shader, glm::mat4 ModelViewMatrix)
{
	//glTranslatef(myLocalPosition.x, myLocalPosition.y, myLocalPosition.z);
	//glScalef(radius-hapticRadius, radius-hapticRadius, radius-hapticRadius);

	glm::mat4 m = glm::translate(ModelViewMatrix,glm::vec3(myLocalPosition.x, myLocalPosition.y, myLocalPosition.z));
	m = glm::scale(m, glm::vec3(radius-hapticRadius,radius-hapticRadius,radius-hapticRadius));
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewMatrix"), 1, GL_FALSE, &m[0][0]);

	float Material_Ambient[4] = {myAmbientColour[0], myAmbientColour[1], myAmbientColour[2], 1.0};
	glUniform4fv(glGetUniformLocation(shader, "material_ambient"), 1, Material_Ambient);
	float Material_Diffuse[4] = {myDiffuseColour[0], myDiffuseColour[1], myDiffuseColour[2], 1.0};
	glUniform4fv(glGetUniformLocation(shader, "material_diffuse"), 1, Material_Diffuse);

	SuperAtom::renderVBO(shader);
}









void SuperAtom::renderSimple()
{
	glPushMatrix();

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, myAmbientColour);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, myDiffuseColour);

		glTranslatef(myLocalPosition.x, myLocalPosition.y, myLocalPosition.z);
		
		glScalef(radius-hapticRadius, radius-hapticRadius, radius-hapticRadius);

		glCallList(sphereDisplayList);
	glPopMatrix();
}
void SuperAtom::render(bool hydrogens, bool waters, bool shadows, AtomList &theList)
{
	float scaledVal = (float)((myScale*3.3)*(myScale*3.3)) ;
	float scaledArray[4] = {-scaledVal, -scaledVal, -scaledVal, -scaledVal};

	if(shadows)
	{
		for(unsigned j = 0; (j < 8) && (j < myOcclusionCoordinates.size() ); j++ )
		{
			int tempID = myOcclusionCoordinates[j];

			Atom* tempAtom = theList[tempID]; 
			Vector3d temp = tempAtom->getMyLocalPosition();

			if((tempAtom->getEle().compare("H")) == 0)
			{
				if(hydrogens)
				{
					float lightPos0[] = {temp.x, temp.y, temp.z, 1.0f};

					glLightfv(GL_LIGHT0 + j, GL_AMBIENT, scaledArray);
					glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, scaledArray);
					glLightfv(GL_LIGHT0 + j, GL_SPECULAR, scaledArray);
					glLightfv(GL_LIGHT0 + j, GL_POSITION, lightPos0);
					glLightf(GL_LIGHT0 + j, GL_QUADRATIC_ATTENUATION,  1.0f);
				}
			}
			else if(tempAtom->isWater())
			{
				if(waters)
				{
					float lightPos0[] = {temp.x, temp.y, temp.z, 1.0f};

			
					glLightfv(GL_LIGHT0 + j, GL_AMBIENT, scaledArray);
					glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, scaledArray);
					glLightfv(GL_LIGHT0 + j, GL_SPECULAR, scaledArray);
					glLightfv(GL_LIGHT0 + j, GL_POSITION, lightPos0);

					glLightf(GL_LIGHT0 + j, GL_QUADRATIC_ATTENUATION,  1.0f);

				}
			}
			else
			{
				float lightPos0[] = {temp.x, temp.y, temp.z, 1.0f};

				
				glLightfv(GL_LIGHT0 + j, GL_AMBIENT, scaledArray);
				glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, scaledArray);
				glLightfv(GL_LIGHT0 + j, GL_SPECULAR, scaledArray);
				glLightfv(GL_LIGHT0 + j, GL_POSITION, lightPos0);
				glLightf(GL_LIGHT0 + j, GL_QUADRATIC_ATTENUATION,  1.0f);
			}


		}
	}

	glPushMatrix();

		float mcolorFront[] = { myColour.x*0.6, myColour.y*0.6, myColour.z*0.6, 1.0f };
		float mcolorFrontD[] = { myColour.x, myColour.y, myColour.z, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mcolorFront);

		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mcolorFrontD);

		float spec[] = {1.0f,1.0f,1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
		
		glTranslatef(myLocalPosition.x, myLocalPosition.y, myLocalPosition.z);
		glScalef(radius-hapticRadius, radius-hapticRadius, radius-hapticRadius);
		glEnable(GL_NORMALIZE);
		glCallList(sphereDisplayList);
		
	glPopMatrix();

	beenDrawn = true;
	
}


void SuperAtom::init()
{
	theta1 = 0.0;
	theta2 = 0.0;
	theta3 = 0.0;

	ex = 0.0f;
	ey = 0.0f;
	ez = 0.0f;

	px = 0.0f;
	py = 0.0f;
	pz = 0.0f;
}
void SuperAtom::makeList()
{
	//setUpDrawElements(0.0f, 0.0f, 0.0f, 1.0f, 16);
	setUpGeometry(0.0f, 0.0f, 0.0f, 1.0f, 16);

	sphereDisplayList = glGenLists(1);
	glNewList(sphereDisplayList, GL_COMPILE);
		renderSphereDrawElements();
	glEndList();

	setUpGeometry(0.0f, 0.0f, 0.0f, 1.0f, 16);

	HR_sphereDisplayList = glGenLists(1);
	
	glNewList(HR_sphereDisplayList , GL_COMPILE);
		renderSphereDrawElements();
	glEndList();

	float spec[] = {1.0f,1.0f,1.0f, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
}
void SuperAtom::renderSphereDrawElements( )
{
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glNormalPointer(GL_FLOAT, 0, normals);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	//glDrawElements(GL_TRIANGLE_STRIP, NumberOfTriangles, GL_UNSIGNED_INT, triIndices);
	glDrawElements(GL_TRIANGLES, NumberOfTriangles*3, GL_UNSIGNED_INT, triIndices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}

void SuperAtom::bindbuffers(int shader)
{
	glBindVertexArray(m_vaoID);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void SuperAtom::renderVBO(int shader)
{
	glDrawElements(GL_TRIANGLES, NumberOfTrianglesUnitSphere * 3, GL_UNSIGNED_INT, 0);
}

void SuperAtom::Unbind(int shader)
{
	glBindVertexArray(0);
}

void SuperAtom::constructUnitSphere(int level, int shader)
{
	
	if(unitSphereVerts!=NULL)
		delete [] unitSphereVerts;

	int p = level;
	NumberOfVerticesUnitSphere = ((level-2)* level+2);
	NumberOfTrianglesUnitSphere = (((level-3)*(level-1) + (level-1)) * 2);

	unitSphereVerts = new float[((level-2)* level+2)*3];
	norms = new float[((level-2)* level+2)*3];
	tInds = new unsigned int[NumberOfTrianglesUnitSphere * 3];
	colors =  new float[NumberOfVerticesUnitSphere * 3];

	float theta, phi;  
	int i, j, t;  
	
	for( t=0, j=1; j<level-1; j++ )  
	{
		for( i=0; i<level; i++ )  
		{    
			theta = float(j)/(level-1) * PI;    
			phi   = float(i)/(level-1 ) * PI*2;    
			unitSphereVerts[t] =  sinf(theta) * cosf(phi);    
			unitSphereVerts[t+1] =  cosf(theta);    
			unitSphereVerts[t+2] = -sinf(theta) * sinf(phi);  

			norms[t] = unitSphereVerts[t];
			norms[t+1] = unitSphereVerts[t+1];
			norms[t+2] = unitSphereVerts[t+2];

			t+=3;
		}  
	}
	unitSphereVerts[t]=0; 
	unitSphereVerts[t+1]= 1; 
	unitSphereVerts[t+2]=0;  
	norms[t] = 0;
	norms[t+1] = 1;
	norms[t+2] = 0;
	t+=3;
	unitSphereVerts[t]=0; 
	unitSphereVerts[t+1]=-1; 
	unitSphereVerts[t+2]=0;
	norms[t] = 0;
	norms[t+1] = -1;
	norms[t+2] = 0;


	for( t=0, j=0; j<p-3; j++ )  
	{
		for(      i=0; i<p-1; i++ )  
		{    
			tInds[t] = ((j  )*p + i);    t++;
			tInds[t] = ((j+1)*p + i+1);    t++;
			tInds[t] = ((j  )*p + i+1);    t++;
			tInds[t] = ((j  )*p + i ) ;    t++;
			tInds[t] = ((j+1)*p + i ) ;    t++;
			tInds[t] = ((j+1)*p + i+1);  t++;
		}  
	}
	for( i=0; i<p-1; i++ )  
	{    
		tInds[t] = ((p-2)*p);  t++;  
		tInds[t] = (i);    t++;
		tInds[t] = (i+1);    t++;
		tInds[t] = ((p-2)*p+1);    t++;
		tInds[t] = ((p-3)*p + i+1);    t++;
		tInds[t] = ((p-3)*p + i);  t++;
	}

	// VAO allocation
	glGenVertexArrays(1, &m_vaoID);

	// First VAO setup
	glBindVertexArray(m_vaoID);

	glGenBuffers(3, m_vboID);
	

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, NumberOfVerticesUnitSphere*3*sizeof(GLfloat), unitSphereVerts, GL_STATIC_DRAW);
	GLint vertexLocation= glGetAttribLocation(shader, "in_Position"); 
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, NumberOfVerticesUnitSphere*3*sizeof(GLfloat), colors, GL_STATIC_DRAW);
	GLint colorLocation= glGetAttribLocation(shader, "in_Color"); 
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(colorLocation);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[2]);
	glBufferData(GL_ARRAY_BUFFER, NumberOfVerticesUnitSphere*3*sizeof(GLfloat), norms, GL_STATIC_DRAW);
	GLint normalLocation= glGetAttribLocation(shader, "in_Normal");
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(normalLocation);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumberOfTrianglesUnitSphere * 3 * sizeof(unsigned int), tInds, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}


void SuperAtom::setUpGeometry( float cx, float cy, float cz, float r, int p )
{
	#define PI 3.14159265358979323846f  

	int width = 32;  
	int height = 16;  
	float theta, phi;  
	int i, j, t;  
	int nvec = (height-2)* width+2;  
	int ntri = (height-2)*(width-1)*2;  

	NumberOfVertices = nvec;
	NumberOfTriangles = ntri;

	vertices = (float*) malloc( nvec * 3*sizeof(float) );  
	normals = (float*) malloc( nvec * 3*sizeof(float) );  
	triIndices =   (unsigned int*) malloc( ntri * 3*sizeof(unsigned int)   );  

	for( t=0, j=1; j<height-1; j++ )  
	{
		for(      i=0; i<width; i++ )  
		{    
			theta = float(j)/(height-1) * PI;    
			phi   = float(i)/(width-1 ) * PI*2;    
			vertices[t] =  sinf(theta) * cosf(phi);    
			vertices[t+1] =  cosf(theta);    
			vertices[t+2] = -sinf(theta) * sinf(phi);  

			ex = sinf(theta) * cosf(phi);
			ey = cosf(theta);
			ez = -sinf(theta) * sinf(phi);

			normals[t] = ex;
			normals[t+1] = ey;
			normals[t+2] = ez;

			t+=3;
		}  
	}
	vertices[t]=0; 
	vertices[t+1]= 1; 
	vertices[t+2]=0;  
	normals[t] = 0;
	normals[t+1] = 1;
	normals[t+2] = 0;
	t+=3;
	vertices[t]=0; 
	vertices[t+1]=-1; 
	vertices[t+2]=0;  
	normals[t] = 0;
	normals[t+1] = -1;
	normals[t+2] = 0;
	t+=3;
	

	for( t=0, j=0; j<height-3; j++ )  
	{
		for(      i=0; i<width-1; i++ )  
		{    
			triIndices[t++] = (j  )*width + i  ;    
			triIndices[t++] = (j+1)*width + i+1;    
			triIndices[t++] = (j  )*width + i+1;    
			triIndices[t++] = (j  )*width + i  ;    
			triIndices[t++] = (j+1)*width + i  ;    
			triIndices[t++] = (j+1)*width + i+1;  
		}  
	}
	for( i=0; i<width-1; i++ )  
	{    
		triIndices[t++] = (height-2)*width;    
		triIndices[t++] = i;    
		triIndices[t++] = i+1;    
		triIndices[t++] = (height-2)*width+1;    
		triIndices[t++] = (height-3)*width + i+1;    
		triIndices[t++] = (height-3)*width + i;  
	}
}
void SuperAtom::setUpDrawElements( float cx, float cy, float cz, float r, int p )
{
	delete [] normals;
	delete [] vertices;
	delete [] triIndices;
	normals = new float[(p/2)*(p+1)*6];
	vertices = new float[(p/2)*(p+1)*6];
	triIndices = new unsigned int[(p/2)*(p+1)*2];
	int counter = 0;

	NumberOfTriangles = 0;

	for( int i = 0; i < p/2; ++i )
	{
		theta1 = i * TWOPI / p - PIDIV2;
		theta2 = (i + 1) * TWOPI / p - PIDIV2;

		for( int j = 0; j <= p; ++j )
		{

			theta3 = j * TWOPI / p;

			ex = cosf(theta1) * cosf(theta3);
			ey = sinf(theta1);
			ez = cosf(theta1) * sinf(theta3);
			px = cx + r * ex;
			py = cy + r * ey;
			pz = cz + r * ez;

			normals[counter] = ex;
			normals[counter+1] = ey;
			normals[counter+2] = ez;


			
			vertices[counter] = px;
			vertices[counter+1] = py;
			vertices[counter+2] = pz;

			triIndices[NumberOfTriangles++] = counter/3;

			counter += 3;

			ex = cosf(theta2) * cosf(theta3);
			ey = sinf(theta2);
			ez = cosf(theta2) * sinf(theta3);
			px = cx + r * ex;
			py = cy + r * ey;
			pz = cz + r * ez;

			normals[counter] = ex;
			normals[counter+1] = ey;
			normals[counter+2] = ez;
			
			vertices[counter] = px;
			vertices[counter+1] = py;
			vertices[counter+2] = pz;

			triIndices[NumberOfTriangles++] = counter/3;

			counter += 3;
		}
	}
}
void SuperAtom::setMyPosition(const int newX, const int newY, const int newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}
void SuperAtom::incMyLocalPosition(const float newX, const float newY, const float newZ)
{
	myLocalPosition.x += newX;
	myLocalPosition.y += newY;
	myLocalPosition.z += newZ;
}
void SuperAtom::readMyPosition(int &readX, int &readY, int &readZ)
{
	readX = x;
	readY = y;
	readZ = z;
}
void SuperAtom::renderSphere( float cx, float cy, float cz, float r, int p )
{
    for( int i = 0; i < p/2; ++i )
	{
        theta1 = i * TWOPI / p - PIDIV2;
        theta2 = (i + 1) * TWOPI / p - PIDIV2;

        glBegin( GL_TRIANGLE_STRIP );
            for( int j = 0; j <= p; ++j )
            {

                theta3 = j * TWOPI / p;

				ex = cosf(theta1) * cosf(theta3);
                ey = sinf(theta1);
                ez = cosf(theta1) * sinf(theta3);
                px = cx + r * ex;
                py = cy + r * ey;
                pz = cz + r * ez;

                glNormal3f( ex, ey, ez );
                //glTexCoord2f( -(j/(float)p), 2*i/(float)p );
                glVertex3f( px, py, pz );

                ex = cosf(theta2) * cosf(theta3);
                ey = sinf(theta2);
                ez = cosf(theta2) * sinf(theta3);
                px = cx + r * ex;
                py = cy + r * ey;
                pz = cz + r * ez;

                glNormal3f( ex, ey, ez );
                //glTexCoord2f( -(j/(float)p) , 2*(i+1)/(float)p );
                glVertex3f( px, py, pz );
            }
        glEnd();
    }
}
void SuperAtom::scaleAtom(float scale, float darklight)
{
	theDarklight = darklight;
	myScale = scale;
	radius = (origRadius * scale) + (SuperAtom::origHapticRadius*scale);
	myLocalPosition = myOrigLocalPosition * scale;
}
void SuperAtom::subtractVectorFromLocalPosition(Vector3d midPoint)
{
	myOrigLocalPosition.x -= midPoint.x;
	myOrigLocalPosition.y -= midPoint.y;
	myOrigLocalPosition.z -= midPoint.z;
	myLocalPosition = myOrigLocalPosition;
}

Atom::Atom(Vector3d position, Vector3d colour, std::string info, float radius, int positionInOriginalList/*, MassList &theMassList*/, string me, string theRes, string resName, string cID)
{
	theDarklight = 1.0f;

	myScale = 1.0f;
	myAmountOfCones = 0;
	origRadius = radius-SuperAtom::origHapticRadius;
	this->radius = radius;

	myOrigLocalPosition = myLocalPosition = position;
	oldLocalPosition = myLocalPosition;
	myColour = colour;
	myAmbientColour[0] = myColour.x*0.6;
	myAmbientColour[1] = myColour.y*0.6;
	myAmbientColour[2] = myColour.z*0.6;
	myDiffuseColour[0] = myColour.x;
	myDiffuseColour[1] = myColour.y;
	myDiffuseColour[2] = myColour.z;
	myInfo = info;

	myPositionInOriginalList = positionInOriginalList;
	inContact = false;
	needsUpdating = true;

	element = me;
	imWater = false;

	residue = theRes;
	boost::trim(residue);
	residueName = resName;
	chainID = cID;

	normals = NULL;
	vertices = NULL;
	triIndices = NULL;

	beenDrawn = false;
	origStartingRadius = radius;
}
Atom::~Atom()
{
}

