#ifndef _QUATERNION_H
#define _QUATERNION_H

class Quaternion  
{
public:
	Quaternion operator *(Quaternion q);
	Quaternion operator +(Quaternion q);
	Quaternion operator -(Quaternion q);
	void CreateMatrix(double *pMatrix);
	double* getMatrix();
	void CreateFromMatrix(double *pMatrix);
	void CreateFromAxisAngle(double x, double y, double z, double degrees);
	Quaternion slerp(Quaternion qb, double t);
	Quaternion();
	Quaternion(double w, double x, double y, double z);
	virtual ~Quaternion();

//private:
	double m_w;
	double m_z;
	double m_y;
	double m_x;
	double pMatrix[16];
};

#endif _QUATERNION_H