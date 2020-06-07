#include "matrix2d.h"
#include "vector3d.h"



Matrix2d::Matrix2d()
{
	setZero();
}

Matrix2d::Matrix2d(float *newVals)
{
	for (int count = 0; count < 16; count++)
	{
		rotationMatrix[count] = (float) newVals[count];
	}
}

Matrix2d::Matrix2d(double *newVals)
{
	for (int count = 0; count < 16; count++)
	{
		rotationMatrix[count] = (float) newVals[count];
	}
}
		
Matrix2d Matrix2d::operator*(Matrix2d secondMat) //matrix multiply 4x4
{
	Matrix2d result; 
	
	for (int count1 = 0; count1 < 4; count1++) //Row
	{
		for (int count2 = 0; count2 < 4; count2++) //Column
		{
			for (int count3 = 0; count3 <4; count3++)
			{
				result[(count1*4)+count2] +=  rotationMatrix[(count3*4)+count2] * secondMat.rotationMatrix[(count1*4)+count3];
			}
		}
	}

	return result;
}

float& Matrix2d::operator[](int index)
{
	return rotationMatrix[index];
}

Vector3d Matrix2d::operator*(Vector3d theVector) //matrix multiply 4x4 x 3x1
{
	Vector3d result; 
	
	result.x = rotationMatrix[0] * theVector.x + rotationMatrix[4] * theVector.y 
		+ rotationMatrix[8] * theVector.z + rotationMatrix[12];
	result.y = rotationMatrix[1] * theVector.x + rotationMatrix[5] * theVector.y 
		+ rotationMatrix[9] * theVector.z + rotationMatrix[13];
	result.z = rotationMatrix[2] * theVector.x + rotationMatrix[6] * theVector.y 
		+ rotationMatrix[10] * theVector.z + rotationMatrix[14];
	
	return result;
}


void Matrix2d::setZero()
{
	for (int count = 0; count < 16; count++)
	{
		rotationMatrix[count] = 0;
	}
}

void Matrix2d::setIdentity()
{
	setZero();
	rotationMatrix[0] = 1;
	rotationMatrix[5] = 1;
	rotationMatrix[10] = 1;
	rotationMatrix[15] = 1;
}

void Matrix2d::setTranslationTo(Vector3d theVector)
{
	rotationMatrix[12] += theVector.x;
	rotationMatrix[13] += theVector.y;
	rotationMatrix[14] += theVector.z;
}


void Matrix2d::printOut()
{
	printf("\n(%f, %f, %f, %f)\n",	rotationMatrix[0], rotationMatrix[4], rotationMatrix[8],  rotationMatrix[12]);
	printf("(%f, %f, %f, %f)\n",	rotationMatrix[1], rotationMatrix[5], rotationMatrix[9],  rotationMatrix[13]);
	printf("(%f, %f, %f, %f)\n",	rotationMatrix[2], rotationMatrix[6], rotationMatrix[10], rotationMatrix[14]);
	printf("(%f, %f, %f, %f)\n\n",	rotationMatrix[3], rotationMatrix[7], rotationMatrix[11], rotationMatrix[15]);
}


//---------------------------------------------------------------------------
void  Matrix2d::axisAngle (float x, float y, float z, float theta)
{
   /* This function performs an axis/angle rotation. (x,y,z) is any 
      vector on the axis. For greater speed, always use a unit vector 
      (length = 1). In this version, we will assume an arbitrary 
      length and normalize. */

   float length;
   float c,s,t;

   // normalize
   length = sqrt(x*x + y*y + z*z);

   // too close to 0, can't make a normalized vector
   if (length < .000001)
      return;

   x /= length;
   y /= length;
   z /= length;

   // do the trig
   c = cos(theta);
   s = sin(theta);
   t = 1-c;   

   // build the rotation matrix
   rotationMatrix[0] = t*x*x + c;
   rotationMatrix[1] = t*x*y - s*z;
   rotationMatrix[2] = t*x*z + s*y;
   rotationMatrix[3] = 0;

   rotationMatrix[4] = t*x*y + s*z;
   rotationMatrix[5] = t*y*y + c;
   rotationMatrix[6] = t*y*z - s*x;
   rotationMatrix[7] = 0;

   rotationMatrix[8] = t*x*z - s*y;
   rotationMatrix[9] = t*y*z + s*x;
   rotationMatrix[10] = t*z*z + c;
   rotationMatrix[11] = 0;

   // build the transform
 //  MatrixMultiply(ViewRotationMatrix,ViewMoveMatrix,WorldTransform); */
}



/*
R is normalized: the squares of the elements in any row or column sum to 1. 
R is orthogonal: the dot product of any pair of rows or any pair of columns is 0. 
The rows of R represent the coordinates in the original space of unit vectors along the coordinate axes of the rotated space. (Figure 1). 
The columns of R represent the coordinates in the rotated space of unit vectors along the axes of the original space. 
 */
bool Matrix2d::isValidRotation()
{
	float firstCol = pow(rotationMatrix[0],2) + pow(rotationMatrix[1],2) + pow(rotationMatrix[2],2) + pow(rotationMatrix[3],2);
	float secondCol = pow(rotationMatrix[4],2) + pow(rotationMatrix[5],2) + pow(rotationMatrix[6],2) + pow(rotationMatrix[7],2);
	float thirdCol = pow(rotationMatrix[8],2) + pow(rotationMatrix[9],2) + pow(rotationMatrix[10],2) + pow(rotationMatrix[11],2);
	float fourthCol = pow(rotationMatrix[12],2) + pow(rotationMatrix[13],2) + pow(rotationMatrix[14],2) + pow(rotationMatrix[15],2);

	if ((abs(firstCol) < 0.90) || (abs(secondCol) < 0.90) || (abs(thirdCol) < 0.90) || (abs(fourthCol) < 0.90))
	{
		printOut();
		return false;
	}
	return true;
}



//************************************** FROM AN INTEL PAPER - SLOW EXAMPLE ********************

/************************************************************
*
* input:
* mat - pointer to array of 16 floats (source matrix)
* output:
* dst - pointer to array of 16 floats (invert matrix)
*
*************************************************************/
void Matrix2d::invertMatrix( Matrix2d &mat, Matrix2d &dst)
{
	float tmp[12]; /* temp array for pairs */
	float src[16]; /* array of transpose source matrix */
	float det; /* determinant */

	/* transpose matrix */
	for ( int i = 0; i < 4; i++) 
	{
		src[i] = mat[i*4];
		src[i + 4] = mat[i*4 + 1];
		src[i + 8] = mat[i*4 + 2];
		src[i + 12] = mat[i*4 + 3];
	}

	/* calculate pairs for first 8 elements (cofactors) */
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];

	/* calculate first 8 elements (cofactors) */
	dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
	dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
	dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
	dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
	dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
	dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
	dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
	dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
	dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
	dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
	dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
	dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
	dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
	dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
	dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
	dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = src[2]*src[7];
	tmp[1] = src[3]*src[6];
	tmp[2] = src[1]*src[7];
	tmp[3] = src[3]*src[5];
	tmp[4] = src[1]*src[6];
	tmp[5] = src[2]*src[5];
	tmp[6] = src[0]*src[7];
	tmp[7] = src[3]*src[4];
	tmp[8] = src[0]*src[6];
	tmp[9] = src[2]*src[4];
	tmp[10] = src[0]*src[5];
	tmp[11] = src[1]*src[4];

	/* calculate second 8 elements (cofactors) */
	dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
	dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
	dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
	dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
	dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
	dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
	dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
	dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
	dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
	dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
	dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
	dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
	dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
	dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
	dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
	dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

	/* calculate determinant */
	det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];

	/* calculate matrix inverse */
	det = 1/det;
	for ( int j = 0; j < 16; j++)
	{
		dst[j] *= det;
	}
}

void Matrix2d::setRow(int which, const Vector3d &vals)
{
	//if (which ==0)
	{
		//rotationMatrix[0+which] = vals.x;
		//rotationMatrix[4+which] = vals.y;
		//rotationMatrix[8+which] = vals.z;
		//rotationMatrix[12+which] = 0;
		rotationMatrix[0+which*4] = vals.x;
		rotationMatrix[1+which*4] = vals.y;
		rotationMatrix[2+which*4] = vals.z;
		rotationMatrix[3+which*4] = 0;
	}
}

void Matrix2d::transposeMatrix()
{
	float result[16];
	for (int count = 0; count < 4; count++)
	{
		result[0+count*4] = rotationMatrix[0+count];
		result[1+count*4] = rotationMatrix[4+count];
		result[2+count*4] = rotationMatrix[8+count];
		result[3+count*4] = rotationMatrix[12+count];
	}
	memcpy(rotationMatrix, result, sizeof(float)*16);
}

