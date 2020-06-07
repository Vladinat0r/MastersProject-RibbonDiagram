#ifndef _MATRIX2D_H_
#define _MATRIX2D_H_

class Vector3d;

class Matrix2d
{
	public:
		float rotationMatrix[16];
		
		Matrix2d();
		
		Matrix2d(float *newVals);
		Matrix2d(double *newVals);

		Matrix2d operator*(Matrix2d secondMat); //matrix multiply 4x4

		Vector3d operator*(Vector3d theVector); //matrix multiply 4x4 x 3x1

		float& operator[](int index);

		void setIdentity();

		void setRow(int which, const Vector3d &vals);

		void setZero();

		void setTranslationTo(Vector3d theVector);

		float * getDataPoint() { return &rotationMatrix[0]; }

		void getFloats(float *thedata)
		{
			for (int count = 0; count < 16; count++)
			{
				thedata[count] = (float) rotationMatrix[count];
			}
		}

		void printOut();

		bool isValidRotation();

		void  axisAngle (float x, float y, float z, float theta);

		void transposeMatrix();

	//Not my code
		static void invertMatrix( Matrix2d &mat, Matrix2d &dst);

};

#endif

