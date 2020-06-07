#ifndef _MATRIX_ROUTINES_H
#define _MATRIX_ROUTINES_H

#include <math.h>
#include <iostream>
using namespace std;

#define PI 3.1415926535897932384626433832795
const double ToRadians = PI / 180.0;
template <class T>
class MatrixRoutines
{
public:
	static void setToIdentity(T m[16])
	{
		for(int i =0; i<4; i++)
		{
			for(int j=0; j<4; j++)
			{
				if(i==j)
					m[i*4+j] = 1.0;
				else
					m[i*4+j] = 0.0;
			}
		}
	}
	static void setToIdentityWithPosition(T x, T y, T z, T m[16])
	{
		setToIdentity(m);

		m[12] = x;
		m[13] = y;
		m[14] = z;
	}
	static void setToRotationX(T spinInDegrees, T m[16])
	{
		setToIdentity(m);

		m[5] = cos(spinInDegrees*ToRadians); 
		m[6] = -sin(spinInDegrees*ToRadians);
		m[9] = sin(spinInDegrees*ToRadians);
		m[10]= cos(spinInDegrees*ToRadians);
	}
	static void setToRotationY(T spinInDegrees, T m[16])
	{
		setToIdentity(m);

		m[0] = cos(spinInDegrees*ToRadians); 
		m[2] = sin(spinInDegrees*ToRadians);
		m[8] = -sin(spinInDegrees*ToRadians);
		m[10]= cos(spinInDegrees*ToRadians);
	}
	static void setToRotationZ(T spinInDegrees, T m[16])
	{
		setToIdentity(m);

		m[0] = cos(spinInDegrees*ToRadians); 
		m[1] = -sin(spinInDegrees*ToRadians);
		m[4] = sin(spinInDegrees*ToRadians);
		m[5]= cos(spinInDegrees*ToRadians);
	}
	static void matrixMultiply4x4RigidBody(T A[16], T B[16], T C[16])
	{
		C[0] = A[0]*B[0] + A[4]*B[1] + A[8]*B[2];
		C[1] = A[1]*B[0] + A[5]*B[1] + A[9]*B[2];
		C[2] = A[2]*B[0] + A[6]*B[1] + A[10]*B[2];
		C[3] = 0;

		C[4] = A[0]*B[4] + A[4]*B[5] + A[8]*B[6];
		C[5] = A[1]*B[4] + A[5]*B[5] + A[9]*B[6];
		C[6] = A[2]*B[4] + A[6]*B[5] + A[10]*B[6];
		C[7] = 0;

		C[8] = A[0]*B[8] + A[4]*B[9] + A[8]*B[10];
		C[9] = A[1]*B[8] + A[5]*B[9] + A[9]*B[10];
		C[10] = A[2]*B[8] + A[6]*B[9] + A[10]*B[10];
		C[11] = 0;

		C[12] = A[0]*B[12] + A[4]*B[13] + A[8]*B[14] + A[12];
		C[13] = A[1]*B[12] + A[5]*B[13] + A[9]*B[14] + A[13];
		C[14] = A[2]*B[12] + A[6]*B[13] + A[10]*B[14] + A[14];
		C[15] = 1;
	}
	static void multiplyPoint(T point[3], T matrix[16], T resultingPoint[3])
	{
		resultingPoint[0] = matrix[0]*point[0] + matrix[4]*point[1] + matrix[8]*point[2] + matrix[12];
		resultingPoint[1] = matrix[1]*point[0] + matrix[5]*point[1] + matrix[9]*point[2] + matrix[13];
		resultingPoint[2] = matrix[2]*point[0] + matrix[6]*point[1] + matrix[10]*point[2] + matrix[14];
	}
	static void MultiplyRotationVector(T point[3], T matrix[16], T resultingPoint[3])
	{
		resultingPoint[0] = matrix[0]*point[0] + matrix[4]*point[1] + matrix[8]*point[2];
		resultingPoint[1] = matrix[1]*point[0] + matrix[5]*point[1] + matrix[9]*point[2];
		resultingPoint[2] = matrix[2]*point[0] + matrix[6]*point[1] + matrix[10]*point[2];
	}
	static void FindMatrixFromAtoB(T matA[16], T matB[16], T AtoB[16])
	{
		AtoB[0] = matA[0]*matB[0] + matA[1]*matB[1]+ matA[2]*matB[2];
		AtoB[4] = matA[4]*matB[0] + matA[5]*matB[1] + matA[6]*matB[2];
		AtoB[8] = matA[8]*matB[0] + matA[9]*matB[1] + matA[10]*matB[2];
		AtoB[12] = matA[12]*matB[0] + matA[13]*matB[1] + matA[14]*matB[2] + (-matB[0]*matB[12]-matB[1]*matB[13]-matB[2]*matB[14]);

		AtoB[1] = matA[0]*matB[4] + matA[1]*matB[5] + matA[2]*matB[6];
		AtoB[5] = matA[4]*matB[4] + matA[5]*matB[5] + matA[6]*matB[6];
		AtoB[9] = matA[8]*matB[4] + matA[9]*matB[5] + matA[10]*matB[6];
		AtoB[13] = matA[12]*matB[4] + matA[13]*matB[5] + matA[14]*matB[6] + (-matB[4]*matB[12]-matB[5]*matB[13]-matB[6]*matB[14]);

		AtoB[2] = matA[0]*matB[8] + matA[1]*matB[9] + matA[2]*matB[10];
		AtoB[6] = matA[4]*matB[8] + matA[5]*matB[9] + matA[6]*matB[10];
		AtoB[10] = matA[8]*matB[8] + matA[9]*matB[9] + matA[10]*matB[10];
		AtoB[14] = matA[12]*matB[8] + matA[13]*matB[9] + matA[14]*matB[10] + (-matB[8]*matB[12]-matB[9]*matB[13]-matB[10]*matB[14]);

		AtoB[3] = 0.0;
		AtoB[7] = 0.0;
		AtoB[11] = 0.0;
		AtoB[15] = 1.0;
	}
	static void ToLocal(const T p[3], T matrix[16], T newP[3])
	{
		T a=matrix[0], b=matrix[1], c=matrix[2], d=matrix[4], e=matrix[5],f=matrix[6],g=matrix[8],h=matrix[9], j=matrix[10];
		T k=matrix[12], l=matrix[13], m=matrix[14];

		newP[0] = a*p[0] + b*p[1] + c*p[2] + (a*-k + b*-l + c*-m);
		newP[1] = d*p[0] + e*p[1] + f*p[2] + (d*-k + e*-l + f*-m);
		newP[2] = g*p[0] + h*p[1] + j*p[2] + (g*-k + h*-l + j*-m);
	}
};

#endif _MATRIX_ROUTINES_H